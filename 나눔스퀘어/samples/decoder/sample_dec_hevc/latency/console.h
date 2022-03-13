#ifndef UUID_AF13B250_A5AC_4699_A979_85E4153A852C
#define UUID_AF13B250_A5AC_4699_A979_85E4153A852C

#include "application.h"

struct Summary
{
    // Let's simplify the compile-time task of creating default-initialized arrays on the stack. The effect is most noticeable for GCC on Linux
    typedef std::array<std::array<TimeSpan, Stages::COUNT>, Axis::COUNT> Columns;

    Summary() noexcept
    {
        timespans.resize(MAX_COUNT_VIEWS);
        rows.resize(MAX_COUNT_VIEWS);
    }

    void accumulate(uint8_t row, uint8_t axis, uint8_t stage, const TimeSpan& timespan) noexcept
    {
        if (!timespan.hits)
            return;

        if (!rows[row].hits)
            ++groups;

        timespans[row][axis][stage].accumulate(timespan);
        columns[axis][stage].accumulate(timespan);
        rows[row].accumulate(timespan);
        overall.accumulate(timespan);
        hits += timespan.hits;
    }

    std::vector<Columns> timespans{};
    std::vector<TimeSpan> rows{};
    Columns columns{};
    TimeSpan overall{};
    uint32_t groups{};
    uint32_t hits{};
};

class Console
{
public:
    Console(const Application& application) noexcept : m_application(application)
    {
        for (auto& instance : application.instances) {
            for (uint8_t row = 0; row < MAX_COUNT_VIEWS; ++row) {
                const Picture& picture = instance->pictures[row];
                if (picture.hits) {
                    pictures += picture.hits;
                    filters += picture.filters;
                    segments += picture.segments;
                    slices += picture.slices;
                    tiles += picture.tiles;
                    ctu_rows += picture.rows;
                    latencies += picture.latency;
                }

                for (uint8_t view = 0; view <= View::OVERALL; ++view) {
                    const uint8_t v = view == View::OVERALL ? View::PICTURE : view;
                    const uint8_t r = view == View::OVERALL ? 0 : row;

                    for (uint8_t axis = 0; axis < Axis::COUNT; ++axis)
                        for (uint8_t stage = 0; stage < Stages::COUNT; ++stage)
                            complete[view].accumulate(r, axis, stage, instance->stages.timespans[row][axis][stage][v]);
                }
            }
        }
    }

    Summary complete[View::OVERALL + 1]{};

    uint32_t pictures = 0;
    uint32_t filters = 0;
    uint32_t segments = 0;
    uint32_t slices = 0;
    uint32_t tiles = 0;
    uint32_t ctu_rows = 0;
    uint32_t latencies = 0;

protected:
    const Application& m_application;
};
#endif
