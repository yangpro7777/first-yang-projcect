#ifndef UUID_7FD2475E_1544_43FD_919C_82B0D388AF02
#define UUID_7FD2475E_1544_43FD_919C_82B0D388AF02

#include "defines.h"
#include "timings.h"
#include <array>
#include <vector>
#include <string.h>

class Instance;

// Latency measurement axes
struct Axis
{
    typedef enum
    {
        INTER,  // The time is measured between two successive or the last and the first stages of the same picture in the decoding pipeline
        INTRA,  // The time is measured between the same stages of two successive pictures in the decoding pipeline
        OUTPUT, // The time is measured between any given and output stages of the same picture in the decoding pipeline
        COUNT
    } Type;
};

// Latency measurement viewpoints
struct View
{
    typedef enum
    {
        PICTURE,   // Indexed by picture's index in DPB
        DPB,       // Indexed by picture's reordering latency in DPB
        HISTOGRAM, // Indexed by histogram logarithmic ranges by power of 2
        INSTANCE,  // Indexed by instance
        OVERALL
    } Type;
};

// The picture preceeding the current one in the decoding pipeline
struct Predecessor
{
    uint32_t pid{};
    uint32_t serial{};
    TimeTick timetick{};
};

// Decoding pipeline stages for a single decoder instance
struct Stages
{
    enum
    {
        ACQUIRE,  // A free picture is taken for work
        START,    // The very first NALU of a picture is received
        PARSE,    // All NALUs of a picture are received and parsed
        DECODE,   // A picture is decoded
        REORDER,  // A picture is moved from DPB to the output queue
        COPYBACK, // A picture is copied back from GPU to CPU
        UCC,      // A picture is post-processed by UCC
        OUTPUT,   // A picture is sent to the output
        RELEASE,  // A picture is released and can be taken for work again
        COUNT
    };

    // Let's simplify the compile-time task of creating default-initialized arrays on the stack. The effect is most noticeable for GCC on Linux
    typedef std::array<std::array<TimeTick, Stages::COUNT>, Axis::COUNT> TimeTickView;
    typedef std::array<std::array<std::array<TimeSpan, View::OVERALL>, Stages::COUNT>, Axis::COUNT> TimeSpanView;

    Stages() noexcept
    {
        timeticks.resize(MAX_COUNT_PICTURES);
        timespans.resize(MAX_COUNT_PICTURES);
    }

    void setup() noexcept
    {
        begin.freeze();
        for (uint8_t stage = 0; stage < COUNT; ++stage)
            latest[stage].timetick = begin;

        memset(timeticks.data(), 0, sizeof(timeticks));
        for (uint8_t picture = 0; picture < MAX_COUNT_PICTURES; ++picture)
            for (uint8_t axis = 0; axis < Axis::COUNT; ++axis)
                timeticks[picture][axis][RELEASE] = begin;
    }

    void reset(uint32_t pid) noexcept
    {
        for (uint8_t stage = 0; stage < RELEASE; ++stage)
            for (uint8_t axis = 0; axis < Axis::COUNT; ++axis)
                timeticks[pid][axis][stage].value = 0;
    }

    TimeTick& freeze(const Instance& instance, const struct hevc_picture_s* picture, uint32_t pid, uint8_t stage, uint16_t framerate = 0) noexcept;

    TimeTick begin{};
    TimeTick end{};
    std::vector<TimeTickView> timeticks{};
    std::vector<TimeSpanView> timespans{};
    Predecessor latest[Stages::COUNT]{};
    uint32_t consumption{};

private:
    void accumulate(uint32_t pid, uint8_t instance, uint8_t latency, uint8_t axis, uint8_t stage, const TimeSpan& timespan) noexcept
    {
        uint8_t range(0);
        for (uint64_t value = uint64_t(timespan.val * 1000000); value > 0; ++range)
            value >>= 1;

        const uint32_t index[View::OVERALL] = { pid, latency, range, instance };
        for (uint8_t view = 0; view < View::OVERALL; ++view)
            timespans[index[view]][axis][stage][view].accumulate(timespan);
    }

    TimeTick& measure(uint32_t pid, uint8_t stage, uint16_t framerate)
    {
        TimeTick& timetick = timeticks[pid][Axis::INTER][stage];
        TimeTick& previous = timeticks[pid][Axis::INTRA][stage];

        previous = latest[stage].timetick;
        latest[stage].timetick = timetick.freeze();
        ++latest[stage].serial;
        latest[stage].pid = pid;

        if (framerate) {
            const double target(1000.0 * latest[stage].serial / framerate), rate(TimeSpan::compute(timetick.value, begin.value));
            if (rate < target) {
                TimeTick::sleep(uint32_t(target - rate));
                latest[stage].timetick = timetick.freeze();
            }
        }

        return timetick;
    }
};
#endif
