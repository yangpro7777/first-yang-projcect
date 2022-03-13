/**
@brief provide basic framework for researching latencies in the decoding pipeline

@verbatim
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.

MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
@endverbatim
**/

#include <stdio.h>
#include <stdlib.h>
#include <condition_variable>
#include <thread>
#include <map>
#include <mutex>

#include "latency/application.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

// clang-format off
#define RECORD      " |*| S2P %7.2f | P2D %7.2f | D2U %7.2f | U2O %7.2f |*| S2O %7.2f | S2S %7.2f | D2D %7.2f | O2O %7.2f\n"
#define STATISTICS1 " |*| SEGMENT %3d | TILE%7d | ROW   %5d | RATE, fps: "
#define STATISTICS2 " |*| TP  %7.2f | IR  %7.2f --> %6.2f%% --> OR  %7.2f\n"
#define IDENTIFIERS " | ID=%-4d LAT=%-2d POC=%-4d PDC=%-4d"
#define SEPARATOR   "--------------------------------------------------------------------------------------------------------------------------------------------------------\n"
// clang-format on

// Picture decoded status
static std::pair<std::mutex, std::condition_variable> PICTURE_DECODED[MAX_COUNT_INSTANCES][MAX_COUNT_PICTURES];

// The mapping of picture's memory address to picture's unique identifier
static std::map<const hevc_picture_t*, uint32_t> MAP_PICTURE_TO_ID[MAX_COUNT_INSTANCES];

// Retrieve picture ID
uint32_t pid(const Instance& instance, const hevc_picture_t* picture) noexcept
{
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    static uint32_t ID[MAX_COUNT_INSTANCES]{};

    std::map<const hevc_picture_t*, uint32_t>::iterator it = MAP_PICTURE_TO_ID[instance.index].find(picture);
    std::map<const hevc_picture_t*, uint32_t>::const_iterator end = MAP_PICTURE_TO_ID[instance.index].end();
    if (it != end)
        return it->second;

    uint32_t out_pid = ID[instance.index];
    if (ID[instance.index] == MAX_COUNT_PICTURES) {
        uint32_t min_pdc = (std::numeric_limits<uint32_t>::max)();
        std::map<const hevc_picture_t*, uint32_t>::iterator min_pdc_it;
        for (it = MAP_PICTURE_TO_ID[instance.index].begin(); it != end; ++it) {
            if (instance.pictures[it->second].pdc < min_pdc) {
                min_pdc = instance.pictures[it->second].pdc;
                min_pdc_it = it;
                out_pid = it->second;
            }
        }

        MAP_PICTURE_TO_ID[instance.index].erase(min_pdc_it);
        ID[instance.index]--;
    }
    MAP_PICTURE_TO_ID[instance.index][picture] = out_pid;
    ID[instance.index]++;
    return out_pid;
}

// "SPS Parsed" callback
static void sps_callback(context_t context, const hevc_picture_t* picture, const hevc_seq_par_set_t* sps) noexcept
{
    const Instance& instance = *reinterpret_cast<Instance*>(context.p);
    if (instance.application->headers) {
        std::lock_guard<std::mutex> lock(instance.application->mutex);
        printf("%s  | SPS    ID=%02d   ", SEPARATOR, sps->sps_id);
        instance.application->record(sps);
        printf("%s", SEPARATOR);
    }
}

// "PPS Parsed" callback
static void pps_callback(context_t context, const hevc_picture_t* picture, const hevc_pic_par_set_t* pps) noexcept
{
    const Instance& instance = *reinterpret_cast<Instance*>(context.p);
    if (instance.application->headers) {
        std::lock_guard<std::mutex> lock(instance.application->mutex);
        printf("%s  | PPS    ID=%02d            ", SEPARATOR, pps->pps_id);
        instance.application->record(pps);
        printf("%s", SEPARATOR);
    }
}

// "NALU Parsed" callback
static void nalu_parsed_callback(context_t context, const hevc_picture_t* picture, const hevc_nalu_t* nalu) noexcept
{
    if (picture->access_unit_info.num_nal_units == 1) {
        Instance& instance = *reinterpret_cast<Instance*>(context.p);
        const uint32_t id = pid(instance, picture);

        instance.pictures[id].pdc = instance.pdc++;
        if (instance.pdc <= instance.stream->aus.size() * instance.application->loops)
            instance.pictures[id].pdc %= instance.stream->aus.size();

        instance.pictures[id].pid = id;
        instance.pictures[id].replacement.poc = -1;
        instance.pictures[id].replacement.pdc = -1;

        ++instance.stages.consumption;

        instance.pictures[id].first = instance.first(id);
        if (instance.pdc == 1 && instance.pictures[id].first)
            instance.stages.setup();

        instance.stages.freeze(instance, picture, id, Stages::ACQUIRE);
        instance.stages.freeze(instance, picture, id, Stages::START, instance.application->framerate.input);
    }
}

// "Picture Parsed" callback
static void picture_parsed_callback(context_t context, const hevc_picture_t* picture) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    instance.stages.freeze(instance, picture, pid(instance, picture), Stages::PARSE);
}

// "Picture Decoded" callback
static void picture_decoded_callback(context_t context, const hevc_picture_t* picture) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    const uint32_t id = pid(instance, picture);

    // Make sure to freeze stages prior to the output and final cleanup
    PICTURE_DECODED[instance.index][id].first.lock();

    instance.stages.freeze(instance, picture, id, Stages::DECODE, instance.application->framerate.decode);
    instance.stages.freeze(instance, picture, id, Stages::REORDER);
    instance.stages.freeze(instance, picture, id, Stages::COPYBACK);

    if (instance.pdc && instance.validPID(id))
        instance.pictures[id].consumption = instance.stages.consumption;

    PICTURE_DECODED[instance.index][id].first.unlock();
    PICTURE_DECODED[instance.index][id].second.notify_all();
}

// "Picture Sent to Output" callback
static void picture_output_callback(context_t context, const hevc_picture_t* picture) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    const uint32_t id = pid(instance, picture);

    instance.pictures[id].poc = instance.poc++;
    if (instance.pictures[id].poc >= instance.stream->range.max.poc)
        instance.stop = true;

    if (instance.validPID(id)) {
        ++instance.pictures[id].hits;
        instance.pictures[id].reordering = picture->sps->sps_max_num_reorder_pics[0];
        instance.pictures[id].buffering = picture->sps->sps_max_dec_pic_buffering[0];

        if (instance.application->poc)
            instance.pictures[id].consumption = instance.stages.consumption;

        const uint8_t log2_ctu_size = picture->sps->log2_min_coding_block_size + picture->sps->log2_diff_max_min_coding_block_size;
        const uint32_t rows = (picture->sps->pic_height_in_luma_samples + (1 << log2_ctu_size) - 1) >> log2_ctu_size;

        // Increment counters
        instance.pictures[id].rows += picture->pps->entropy_coding_sync_enabled_flag ? rows : 0;
        instance.pictures[id].tiles += picture->pps->num_tile_columns * picture->pps->num_tile_rows;
        instance.pictures[id].slices += picture->slices_count;
        instance.pictures[id].segments += picture->segments_count;
        instance.pictures[id].messages += picture->sei.num_messages;
        instance.pictures[id].filters += (picture->sps->sample_adaptive_offset_enabled_flag > 0) + (picture->pps->pps_deblocking_filter_disabled_flag == 0);
        instance.pictures[id].latency += instance.au(id).lat;
    }

    // Final output and cleanup must happen only when decoding is done
    std::unique_lock<std::mutex> lock(PICTURE_DECODED[instance.index][id].first);
    PICTURE_DECODED[instance.index][id].second.wait(lock, [&instance, id] { return instance.stages.timeticks[id][Axis::INTER][Stages::COPYBACK].value > 0; });

    instance.stages.freeze(instance, picture, id, Stages::UCC);

    if (instance.application->copyback) {
        frame_tt frame = { 0 };
        frame.four_cc = picture->fourcc;
        frame.width = picture->sps->pic_width_in_luma_samples;
        frame.height = picture->sps->pic_height_in_luma_samples;

        frame_colorspace_info_tt colorspace = { 0 };
        if (get_frame_colorspace_info(&colorspace, frame.width, frame.height, frame.four_cc, 0))
            return;

        static uint32_t size = colorspace.frame_size;
        static void* buffer = malloc(colorspace.frame_size + 15);

        if (size != colorspace.frame_size) {
            size = colorspace.frame_size;
            buffer = realloc(buffer, colorspace.frame_size + 15);
        }

        uint8_t* const pointer = reinterpret_cast<uint8_t*>(uintptr_t((uint8_t*)buffer + 15) & ~15);
        fill_frame_from_colorspace_info(&colorspace, pointer, &frame);

        instance.auxinfo(0, GET_PIC, &frame, sizeof(frame_tt));
    }

    --instance.stages.consumption;
    instance.stages.end = instance.stages.freeze(instance, picture, id, Stages::OUTPUT, instance.application->framerate.output);
    instance.stages.end = instance.stages.freeze(instance, picture, id, Stages::RELEASE);

    // Output progress info
    instance.record(id, picture);

    // Reset time records
    instance.stages.reset(id);
}

// "Error" callback
static void error_callback(context_t context, const hevc_picture_t* picture, const hevc_runtime_error_t code) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    if (instance.application->pedantic && instance.validPID(pid(instance, picture))) {
        instance.error = code;
        printf("The execution was interrupted by error code %d (refer to 'hevc_runtime_error_e' enumeration for more details)\n", code);
    }
}

// Print progress statistics
static void progress(const Instance& instance, const Progress& progress) noexcept
{
    const AccessUnit& au = instance.stream->aus[progress.counters.pdc];
    printf("%s%s", progress.type == Picture::I ? SEPARATOR : "", PICTURE_TYPE[progress.type]);
    printf(IDENTIFIERS RECORD, instance.index, au.lat, progress.counters.poc, progress.counters.pdc, progress.timespans[Axis::INTER][Stages::PARSE].val,
        progress.timespans[Axis::INTER][Stages::DECODE].val, progress.timespans[Axis::INTER][Stages::UCC].val,
        progress.timespans[Axis::INTER][Stages::OUTPUT].val, progress.timespans[Axis::OUTPUT][Stages::START].val,
        progress.timespans[Axis::INTRA][Stages::START].val, progress.timespans[Axis::INTRA][Stages::DECODE].val,
        progress.timespans[Axis::INTRA][Stages::OUTPUT].val);
}

// Print final statistics
static void finalize(const Application& application) noexcept
{
    TimeSpan overall[Axis::COUNT][Stages::COUNT]{};
    uint32_t picture_count = 0;
    uint32_t filter_count = 0;
    uint32_t segment_count = 0;
    uint32_t slice_count = 0;
    uint32_t tile_count = 0;
    uint32_t row_count = 0;
    uint32_t latency = 0;

    for (auto& instance : application.instances) {
        for (uint8_t j = 0; j < MAX_COUNT_PICTURES; ++j) {
            const Picture& picture = instance->pictures[j];
            if (picture.hits) {
                picture_count += picture.hits;
                filter_count += picture.filters;
                segment_count += picture.segments;
                slice_count += picture.slices;
                tile_count += picture.tiles;
                row_count += picture.rows;
                latency += picture.latency;

                for (uint8_t axis = 0; axis < Axis::COUNT; ++axis)
                    for (uint8_t stage = 0; stage < Stages::COUNT; ++stage)
                        overall[axis][stage].accumulate(instance->stages.timespans[j][axis][stage][View::PICTURE]);
            }
        }
    }

    printf("%s                             MIN, ms" RECORD, SEPARATOR, overall[Axis::INTER][Stages::PARSE].min, overall[Axis::INTER][Stages::DECODE].min,
        overall[Axis::INTER][Stages::UCC].min, overall[Axis::INTER][Stages::OUTPUT].min, overall[Axis::OUTPUT][Stages::START].min,
        overall[Axis::INTRA][Stages::START].min, overall[Axis::INTRA][Stages::COPYBACK].min, overall[Axis::INTRA][Stages::OUTPUT].min);
    printf("STATISTICS                   AVG, ms" RECORD, overall[Axis::INTER][Stages::PARSE].average(), overall[Axis::INTER][Stages::DECODE].average(),
        overall[Axis::INTER][Stages::UCC].average(), overall[Axis::INTER][Stages::OUTPUT].average(), overall[Axis::OUTPUT][Stages::START].average(),
        overall[Axis::INTRA][Stages::START].average(), overall[Axis::INTRA][Stages::COPYBACK].average(), overall[Axis::INTRA][Stages::OUTPUT].average());
    printf("                             MAX, ms" RECORD "%s", overall[Axis::INTER][Stages::PARSE].max, overall[Axis::INTER][Stages::DECODE].max,
        overall[Axis::INTER][Stages::UCC].max, overall[Axis::INTER][Stages::OUTPUT].max, overall[Axis::OUTPUT][Stages::START].max,
        overall[Axis::INTRA][Stages::START].max, overall[Axis::INTRA][Stages::COPYBACK].max, overall[Axis::INTRA][Stages::OUTPUT].max, SEPARATOR);

    char toolset[7] = "[CPU] ";
    if (application.toolset > HEVCVD_DECODING_TOOLSET_CPU)
        sprintf(toolset, "[GPU%1d]", application.adapter);

    const double timespan = TimeSpan::compute(application.instances.back()->stages.end.value, application.instances.front()->stages.begin.value);
    printf("DURATION %s          %8.0f ms" STATISTICS1 STATISTICS2 "%s", toolset, timespan, segment_count / picture_count, tile_count / picture_count,
        row_count / picture_count, 1000.0 / overall[Axis::OUTPUT][Stages::START].average(), 1000.0 / overall[Axis::INTRA][Stages::START].average(),
        100 * (1 - overall[Axis::INTRA][Stages::OUTPUT].all / overall[Axis::INTRA][Stages::START].all), 1000.0 / overall[Axis::INTRA][Stages::OUTPUT].average(),
        SEPARATOR);

    if (application.legend) {
        // Print the legend
        printf("\nProcess Metrics\n");
        printf("    IPB  --  picture type                                     POC  --  picture output counter\n");
        printf("    LAT  --  picture reordering latency                       PDC  --  picture decoding counter\n");

        printf("\nLatency Metrics\n");
        printf("    S2P  --  time to receive all NALUs and parse a picture    D2U  --  time to reorder the DPB\n");
        printf("    P2D  --  time to decode a picture                         U2O  --  time to post-process an output picture by UCC\n");

        printf("\nPerformance Metrics\n");
        printf("    S2O  --  time to receive, decode, and output a picture, a.k.a. ThroughPut  (TP) [ S2P + P2D + D2U + U2O ]\n");
        printf("    S2S  --  time between two successive input pictures,    a.k.a. Input Rate  (IR)\n");
        printf("    D2D  --  time between two successive decoded pictures,  a.k.a. Decode Rate (DR)\n");
        printf("    O2O  --  time between two successive output pictures,   a.k.a. Output Rate (OR)\n");
    }
}

// Read and decode the input stream by multiple instances in the loop. Basic asynchronous implementation
static void async_fork_point(Application& application, Stream& stream) noexcept
{
    size_t offset{};
    const std::function<void(const Stream&, uint8_t*, size_t)> consume = [&](const Stream& stream, uint8_t* buffer, size_t size) noexcept
    {
        std::vector<std::thread> threads;
        for (size_t count = stream.instances.size(), limit = count + offset, i = offset++; i < limit; ++i)
            threads.emplace_back([&]() { stream.instances[i % count]->consume(buffer, size); });

        for (auto& thread : threads)
            thread.join();
    };

    // Start reading the input stream in the loop
    for (stream.loop = 0; stream.loop < application.loops && fseek(stream.handle, 0, SEEK_SET) == 0; ++stream.loop) {
        if (application.chunks == HEVCVD_CP_AU) {
            for (const AccessUnit& chunk : stream.aus)
                consume(stream, stream.read(chunk.size), chunk.size);
        }
        else if (application.chunks == HEVCVD_CP_NALU) {
            for (const NalUnit& chunk : stream.nalus)
                consume(stream, stream.read(chunk.size), chunk.size);
        }
        else {
            size_t size = 0;
            for (uint8_t* buffer = stream.read(&size); size > 0; stream.read(&size))
                consume(stream, buffer, size);
        };
    }
};

// Main entry point
int main(int argc, char* argv[])
{
    callbacks_decoder_hevc_t callbacks{};
    callbacks.nalu_callback = nalu_parsed_callback;
    callbacks.pic_callback = picture_parsed_callback;
    callbacks.pic_decoded_callback = picture_decoded_callback;
    callbacks.pic_output_callback = picture_output_callback;
    callbacks.error_callback = error_callback;
    callbacks.sps_callback = sps_callback;
    callbacks.pps_callback = pps_callback;

    Application application(argc, argv, callbacks);

    const std::function<void(Application & application)> loop = [](Application & application) noexcept
    {
        // Choose the appropriate fork point
        const Application::Fork routine = application.async_input ? async_fork_point : Application::default_fork_point;

        if (application.streams.size() > 1) {
            // Read each input stream in a dedicated thread
            std::thread THREADS[MAX_COUNT_STREAMS]{};
            for (uint8_t i = 0; i < application.streams.size(); ++i)
                THREADS[i] = std::thread(routine, std::ref(application), std::ref(*application.streams[i]));

            for (uint8_t i = 0; i < application.streams.size(); ++i)
                THREADS[i].join();
        }
        else {
            // If only one input stream, read it in the main thread
            routine(application, *application.streams.front());
        }
    };

    return application.main(loop, finalize, progress);
}
