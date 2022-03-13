#include "stream.h"
#include "instance.h"
#include "application.h"

// "Picture Sent to Output" preprocessing callback
static void picture_output_callback(context_t context, const hevc_picture_t* picture) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);

    if (instance.poc >= instance.stream->range.max.poc) {
        instance.stop = true;
    }
    else if (picture->slice_hdr[0]->slice_type == Picture::I) {
        if (instance.gop == instance.application->offset)
            instance.stream->range.min.poc = instance.poc;
        else if (instance.gop == instance.application->offset + instance.application->gops)
            instance.stream->range.max.poc = instance.poc;

        ++instance.gop;
    }

    AccessUnit& au = instance.au(pid(instance, picture));
    au.poc = instance.poc++;
    au.lat = au.poc - au.pdc + picture->sps->sps_max_num_reorder_pics[0] + 1;
}

// "AU Chunk Parsed" preprocessing callback
static void picture_parsed_callback(context_t context, const hevc_picture_t* picture) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    instance.pictures[pid(instance, picture)].pdc = instance.pdc;
    instance.stream->aus.emplace_back(picture->access_unit_info.size, picture->access_unit_info.offset, instance.pdc++, 0, 0);
}

// "NALU Chunk Parsed" preprocessing callback
static void nalu_chunk_parsed_callback(context_t context, const hevc_picture_t* picture, const hevc_nalu_t* nalu) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    instance.stream->nalus.emplace_back(nalu->size, nalu->offset);
}

// "Error" preprocessing callback
static void error_callback(context_t context, const hevc_picture_t* picture, const hevc_runtime_error_t code) noexcept
{
    Instance& instance = *reinterpret_cast<Instance*>(context.p);
    if (instance.application->pedantic && instance.validPID(pid(instance, picture)))
        printf("The execution was interrupted by error code %d (refer to 'hevc_runtime_error_e' enumeration for more details)\n", instance.error = code);
}

bool Stream::parse(Application* application)
{
    Instance* instance = new Instance(application, this);
    instance->callbacks.error_callback = error_callback;
    instance->callbacks.pic_callback = picture_parsed_callback;
    instance->callbacks.pic_output_callback = picture_output_callback;
    instance->callbacks.nalu_callback = nalu_chunk_parsed_callback;

    if (!instance->open()) {
        printf("Failed to create decoder instance\n");
        delete instance;
        return false;
    }

    if (instance->auxinfo(application->reorder ? INTERN_REORDERING_FLAG : 0, PARSE_OPTIONS) != BS_OK || instance->auxinfo(SKIP_IPB, PARSE_FRAMES) != BS_OK) {
        delete instance;
        return false;
    }

    size_t size = 0;
    for (uint8_t* buffer = read(&size); instance->error == NO_RUNTIME_ERROR && size; read(&size))
        instance->consume(buffer, size);

    instance->flush();
    instance->close();

    const bool valid = instance->error == NO_RUNTIME_ERROR && fseek(handle, 0, SEEK_SET) == 0;
    delete instance;
    return valid;
}
