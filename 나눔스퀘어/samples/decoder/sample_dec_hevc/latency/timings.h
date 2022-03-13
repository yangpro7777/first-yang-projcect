#ifndef UUID_9DDCE2D7_2ABC_40DB_8EE6_516C2958F7C9
#define UUID_9DDCE2D7_2ABC_40DB_8EE6_516C2958F7C9

#define __STDC_FORMAT_MACROS

#if defined(__APPLE__)
#include <time.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined(__linux__)
#include <time.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <inttypes.h>
#include <limits>
#include <atomic>

// Time tick representation in nanoseconds
struct TimeTick
{
    static constexpr uint64_t NANOSECONDS_IN_SECOND = 1000000000;

    uint64_t value = 0;

    const TimeTick& freeze() noexcept
    {
#if defined(__APPLE__)
        value = mach_absolute_time();
#elif defined(__linux__)
        struct timespec time = { 0 };
        clock_gettime(CLOCK_REALTIME, &time);
        value = time.tv_sec * NANOSECONDS_IN_SECOND + time.tv_nsec;
#else
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        value = li.QuadPart;
#endif
        return *this;
    }

    static uint64_t frequency() noexcept
    {
#if defined(__APPLE__)
        mach_timebase_info_data_t info = { 0 };
        mach_timebase_info(&info);
        return NANOSECONDS_IN_SECOND * info.denom / info.numer;
#elif defined(__linux__)
        return NANOSECONDS_IN_SECOND;
#else
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        return li.QuadPart;
#endif
    }

    static void sleep(uint32_t milliseconds) noexcept
    {
#if defined(__APPLE__)
        sleep(milliseconds);
#elif defined(__linux__)
        usleep(1000 * milliseconds);
#else
        Sleep(milliseconds);
#endif
    }
};

// Time span (between two ticks) representation in milliseconds
struct TimeSpan
{
    TimeSpan(const uint64_t max_tick, const uint64_t min_tick) noexcept
    {
        val = all = min = max = compute(max_tick, min_tick);
        hits = 1;
    }

    TimeSpan() noexcept
    {
        min = (std::numeric_limits<double>::max)();
        max = (std::numeric_limits<double>::min)();
        val = 0.;
        all = 0.;
        hits = 0;
    }

    TimeSpan(const TimeSpan& timespan) noexcept
    {
        min = timespan.min;
        max = timespan.max;
        val = timespan.val;
        all = timespan.all;
        hits = timespan.hits.load();
    }

    void operator=(const TimeSpan& timespan) noexcept
    {
        min = timespan.min;
        max = timespan.max;
        val = timespan.val;
        all = timespan.all;
        hits = timespan.hits.load();
    }

    double average() const noexcept { return all / hits; }

    double accumulate(const uint64_t max_tick, const uint64_t min_tick) noexcept
    {
        val = compute(max_tick, min_tick);
        all += val;
        ++hits;

        if (val < min)
            min = val;
        if (val > max)
            max = val;

        return val;
    }

    double accumulate(const TimeSpan& timespan) noexcept
    {
        val = timespan.all;
        all += val;
        hits += timespan.hits;

        if (timespan.min < min)
            min = timespan.min;
        if (timespan.max > max)
            max = timespan.max;

        return val;
    }

    static double compute(const uint64_t max_tick, const uint64_t min_tick) noexcept
    {
        static const double FREQUENCY = 1000.0 / TimeTick::frequency();
        return double(max_tick - min_tick) * FREQUENCY;
    }

    double val;
    double all;
    double min;
    double max;
    std::atomic<uint32_t> hits;
};

#endif
