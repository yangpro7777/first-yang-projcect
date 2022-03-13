#ifndef UUID_99744C6F_8E1E_428E_B28D_70E2866D759A
#define UUID_99744C6F_8E1E_428E_B28D_70E2866D759A
#include <algorithm>
#include <vector>

#include "dec_hevc.h"

struct FrameInfo
{
    uint64_t size;
    uint64_t offset;
    uint32_t output_order;
    std::vector<hevc_nalu_t> nal_units;

    FrameInfo(uint64_t size, uint64_t offset, uint32_t output_order) : size(size), offset(offset), output_order(output_order) {}
};

struct StreamInfo
{
    std::vector<FrameInfo> frames_info;
    std::vector<hevc_nalu_t> nalu_info;
    uint32_t frame_count;
};

static void pic_output_callback(context_t context, const hevc_picture_t* hevc_pic)
{
    StreamInfo* stream_info = reinterpret_cast<StreamInfo*>(context.p);
    stream_info->frames_info.emplace_back(hevc_pic->access_unit_info.size, hevc_pic->access_unit_info.offset, stream_info->frame_count++);
}

static void nalu_callback(context_t context, const hevc_picture_t* picture, const hevc_nalu_t* nalu)
{
    StreamInfo* stream_info = reinterpret_cast<StreamInfo*>(context.p);
    stream_info->nalu_info.emplace_back(*nalu);
}

static std::vector<FrameInfo> getStreamInfo(FILE* input_file)
{
    enum
    {
        READ_BUFFER_SIZE = 64 * 1024
    };
    StreamInfo frames_info_getter;

    callbacks_decoder_hevc_t callbacks_decoder_hevc{};
    callbacks_t callbacks{};
    callbacks.context.p = &frames_info_getter;
    callbacks_decoder_hevc.pic_output_callback = pic_output_callback;
    callbacks_decoder_hevc.nalu_callback = nalu_callback;
    bufstream_tt* decoder = createDecoderHEVC(&callbacks, &callbacks_decoder_hevc, 0);

    // Switch to parsing mode, without decoding
    decoder->auxinfo(decoder, 0, PARSE_SEQ_HDR, 0, 0);

    uint8_t buffer[READ_BUFFER_SIZE];
    size_t bytes_available;
    while (0 != (bytes_available = fread(buffer, 1, READ_BUFFER_SIZE, input_file))) {
        uint32_t consumed = 0;

        do {
            consumed += decoder->copybytes(decoder, buffer + consumed, (uint32_t)(bytes_available - consumed));

        } while (bytes_available - consumed);
    }

    while (decoder->copybytes(decoder, 0, 0))
        ;

    close_bufstream(decoder, 0);
    fseek(input_file, 0, SEEK_SET);

    auto compare = [](const FrameInfo& a, const FrameInfo& b) { return a.offset < b.offset; };
    std::sort(frames_info_getter.frames_info.begin(), frames_info_getter.frames_info.end(), compare);

    uint32_t i = 0;
    for (auto nalu : frames_info_getter.nalu_info) {
        if (i < frames_info_getter.frame_count - 1 && nalu.offset == frames_info_getter.frames_info[i + 1].offset)
            ++i;
        frames_info_getter.frames_info[i].nal_units.emplace_back(nalu);
    }

    return frames_info_getter.frames_info;
}

#endif
