#include "stages.h"
#include "instance.h"
#include "stream.h"

TimeTick& Stages::freeze(const Instance& instance, const hevc_picture_t* picture, uint32_t pid, uint8_t stage, uint16_t framerate) noexcept
{
    TimeTick& timetick = measure(pid, stage, framerate);
    if (instance.validPID(pid)) {
        const AccessUnit& au = instance.au(pid);

        // Axis INTRA
        accumulate(pid, instance.index, au.lat, Axis::INTRA, stage, { timetick.value, timeticks[pid][Axis::INTRA][stage].value });

        // Axis INTER
        if (stage != RELEASE || picture->access_unit_info.num_nal_units > 0) {
            const uint8_t predecessor = stage ? (stage == Stages::REORDER ? Stages::PARSE : stage - 1) : COUNT - 1;
            accumulate(pid, instance.index, au.lat, Axis::INTER, stage, { timetick.value, timeticks[pid][Axis::INTER][predecessor].value });
        }

        // Axis OUTPUT
        if (stage == OUTPUT) {
            for (uint8_t i = 0; i < RELEASE; ++i)
                accumulate(pid, instance.index, au.lat, Axis::OUTPUT, i, { timetick.value, timeticks[pid][Axis::INTER][i].value });
        }
    }

    return timetick;
}
