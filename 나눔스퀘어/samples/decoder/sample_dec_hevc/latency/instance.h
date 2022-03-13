#ifndef UUID_BC059CB5_8CBD_4B6F_B2D6_E038221AD171
#define UUID_BC059CB5_8CBD_4B6F_B2D6_E038221AD171

#include <inttypes.h>
#include <functional>
#include "stages.h"
#include "dec_hevc.h"

class Application;
class Stream;

// Threadpool instance
struct Threadpool
{
    mcr_thread_pool_t handle = nullptr;
    int32_t type = -1;
    int32_t shared = 0;
    uint32_t ordering = 0;
    uint16_t producer_count = 1;

    bool open(const callbacks_t* callbacks = nullptr) noexcept { return threadpoolCreate(callbacks, (tp_type_t)type, nullptr, &handle) == MCR_ERROR_OK; }
    bool opened() const noexcept { return handle != nullptr; }

    void close() noexcept
    {
        if (handle)
            threadpoolDestroy(handle, nullptr);
    }
};

// Decoder progress information ordered either by POC, PDC, or PRC
struct Progress
{
    TimeSpan timespans[Axis::COUNT][Stages::COUNT]{};
    Picture counters{};
    uint8_t sps{};
    uint8_t pps{};
    uint8_t type{};
    bool initialized = false;
};

// Single decoder instance. Wrapper around HEVC Decoder MC API
class Instance
{
public:
    typedef std::function<void(const Instance&, const Progress&)> Hook;

    Instance(Application* application, Stream* stream) noexcept;
    Instance(uint8_t index, const callbacks_decoder_hevc_t& callbacks, Application* application, Stream* stream) noexcept;

    bool open() noexcept;

    void close() noexcept
    {
        if (decoder) {
            close_bufstream(decoder, 0);
            decoder = nullptr;
        }

        m_threadpool.close();
    }

    void flush() noexcept
    {
        while (!error && copybytes()) {
        }
    }

    void consume(uint8_t* buffer, size_t size) noexcept
    {
        for (size_t consumed = 0; !stop && error == NO_RUNTIME_ERROR && size != consumed;)
            consumed += copybytes(buffer + consumed, size - consumed);
    }

    size_t copybytes(uint8_t* buffer = nullptr, size_t size = 0) noexcept
    {
        const size_t consumed = decoder->copybytes(decoder, buffer, (uint32_t)size);
        if (auxinfo(0, CLEAN_PARSE_STATE) & INTERNAL_ERROR)
            error = PARSE_ERROR;

        return consumed;
    }

    uint32_t auxinfo(uint32_t offs, uint32_t id, void* data = nullptr, uint32_t size = 0) noexcept { return decoder->auxinfo(decoder, offs, id, data, size); }

    AccessUnit& au(uint32_t pid) noexcept;
    const AccessUnit& au(uint32_t pid) const noexcept;

    bool validPOC(uint32_t poc) const noexcept;
    bool validPID(uint32_t pid) const noexcept;

    bool first(uint32_t pid) const noexcept;
    bool last(uint32_t pid) const noexcept;

    void record(const uint32_t pid, const hevc_picture_t* picture) noexcept;

    Application* const application{};
    Stream* const stream{};

    bufstream_tt* decoder{};

    Stages stages{};
    Hook logger{};

    Progress progress[MAX_COUNT_PICTURES]{};
    Picture pictures[MAX_COUNT_PICTURES]{};

    callbacks_decoder_hevc_t callbacks{};
    hevc_runtime_error_t error = NO_RUNTIME_ERROR;

    // Decoding pipeline counters
    uint32_t pdc{};
    uint32_t poc{};
    uint32_t prc{};
    uint32_t gop{};
    uint32_t frames{};

    const uint8_t index;
    bool stop = false;

private:
    Threadpool m_threadpool{};
    uint32_t m_progress_poc{};
    uint32_t m_progress_prc{};
};

// Getting a picture identifier from a decoder instance
extern uint32_t pid(const Instance& instance, const struct hevc_picture_s* picture) noexcept;
#endif
