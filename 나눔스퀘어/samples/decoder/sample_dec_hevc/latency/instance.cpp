#include "instance.h"
#include "application.h"
#include "stream.h"

Instance::Instance(Application* application, Stream* stream) noexcept : application(application), stream(stream), callbacks(), index(0) {}

Instance::Instance(uint8_t index, const callbacks_decoder_hevc_t& callbacks, Application* application, Stream* stream) noexcept
  : application(application), stream(stream), callbacks(callbacks), index(index)
{
    stream->instances.push_back(this);
}

bool Instance::open() noexcept
{
    close();

    stop = false;
    callbacks_t callbacks{};
    callbacks.context.p = this;

    // By default, each decoder instance creates lock-free threadpool internally
    stream_params_t parameters{};
    parameters.nodeset = -1;

    if (application->threadpool.shared) {
        // Share the single threadpool by all decoder instances
        if (application->threadpool.opened())
            parameters.threadpool = application->threadpool.handle;
    }
    else if (application->threadpool.type != -1) {
        // Create an external threadpool for each decoder instance individually
        m_threadpool.type = application->threadpool.type;
        if (m_threadpool.open(&callbacks)) {
            parameters.threadpool = m_threadpool.handle;
        }
        else {
            printf("Failed to create external threadpool type %d\n", m_threadpool.type);
        }
    }

    decoder = createDecoderHEVC(&callbacks, &this->callbacks, &parameters);
    return decoder != nullptr;
}

AccessUnit& Instance::au(uint32_t pid) noexcept { return stream->aus[pictures[pid].pdc % stream->aus.size()]; }

const AccessUnit& Instance::au(uint32_t pid) const noexcept { return stream->aus[pictures[pid].pdc % stream->aus.size()]; }

bool Instance::validPOC(uint32_t poc) const noexcept { return poc >= stream->range.min.poc && poc < stream->range.max.poc; }

bool Instance::validPID(uint32_t pid) const noexcept
{
    const uint32_t pdc(pictures[pid].pdc);
    return pdc < stream->aus.size() && validPOC(stream->aus[pdc].poc);
}

bool Instance::first(uint32_t pid) const noexcept
{
    const uint32_t pdc(pictures[pid].pdc);
    return pdc < stream->aus.size() && stream->aus[pdc].poc == stream->range.min.poc;
}

bool Instance::last(uint32_t pid) const noexcept
{
    const uint32_t pdc(pictures[pid].pdc);
    return pdc < stream->aus.size() && stream->aus[pdc].poc == stream->range.max.poc - 1;
}

void Instance::record(const uint32_t pid, const hevc_picture_t* picture) noexcept
{
    static std::atomic<uint32_t> counter(0);
    uint32_t count = ++counter;

    if (application->milliseconds != 0) {
        static TimeTick start;
        TimeTick current;

        current.freeze();
        if (count == 1)
            start = current;

        if (TimeSpan::compute(current.value, start.value) >= application->milliseconds) {
            this->stop = true;
            return;
        }
    }

    if (application->progress) {
        std::lock_guard<std::mutex> lock(application->mutex);

        if (picture->access_unit_info.num_nal_units > 0) {
            uint16_t i = m_progress_prc % MAX_COUNT_PICTURES;
            if (application->poc)
                i = pictures[pid].poc % MAX_COUNT_PICTURES;
            else if (application->pdc)
                i = pictures[pid].pdc % MAX_COUNT_PICTURES;

            progress[i].initialized = true;
            progress[i].counters = pictures[pid];

            if (validPID(pid)) {
                for (uint8_t j = 0; j < Axis::COUNT; ++j)
                    for (uint8_t k = 0; k < Stages::COUNT; ++k)
                        progress[i].timespans[j][k] = stages.timespans[pid][j][k][View::PICTURE];

                progress[i].sps = picture->sps->sps_id;
                progress[i].pps = picture->pps->pps_id;
                progress[i].type = picture->slice_hdr[0]->slice_type;
            }
        }

        while (true) {
            Progress& p = progress[m_progress_poc % MAX_COUNT_PICTURES];
            if (!p.initialized)
                break;

            if (frames && m_progress_poc >= frames) {
                this->stop = true;
                break;
            }

            if (validPOC(p.counters.poc))
                logger(*this, p);

            p.initialized = false;
            ++m_progress_poc;
        }

        ++m_progress_prc;
    }
    else {
        printf("%05d frames decoded by %zd instance(s) in %d loop(s)\r", count, application->instances.size(), application->loops);
    }
}
