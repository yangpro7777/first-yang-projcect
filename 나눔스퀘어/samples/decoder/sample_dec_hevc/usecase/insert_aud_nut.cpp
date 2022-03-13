/**
@brief This sample shows how to improve frame decoding latency with Access Unit Delimiter NAL Units.

 When Access Units are transmitted and fed to the decoder NALU by NALU, this approach achieves the same
 latencies as feeding full Access Units to decoder in HEVCVD_CP_AU chunk parse mode (in HEVCVD_SMP_CONCURRENT
 SMP mode).

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/
#include <cstdio>
#include <cassert>

#include "../stream_info.h"

struct LocalContext
{
    uint32_t pictures_queued;
    uint32_t pictures_decoded;
};

static void pictureQueuedCallback(context_t context, const hevc_picture_t* hevc_pic)
{
    LocalContext* p = reinterpret_cast<LocalContext*>(context.p);
    printf("Picture %d with POC = %d was queued for decode\n", p->pictures_queued++, hevc_pic->poc);
}

static void pictureDecodedCallback(context_t context, const hevc_picture_t* hevc_pic)
{
    LocalContext* p = reinterpret_cast<LocalContext*>(context.p);
    printf("Picture %d with POC = %d was decoded\n", p->pictures_decoded++, hevc_pic->poc);
}

int main(int argc, char* argv[])
{
    FILE* f = nullptr;
    if (argc < 2 || !(f = fopen(argv[1], "rb"))) {
        printf("Usage: sample_dec_hevc_insert_aud_nut input_stream\n");
        return 0;
    }

    auto frames = getStreamInfo(f);

    if (!frames.size()) {
        printf("Error: no frames in input file\n");
        return -1;
    }

    uint64_t max_buffer_size = 0LL;
    for (auto frame : frames)
        if (frame.size > max_buffer_size)
            max_buffer_size = frame.size;

    // can be HEVCVD_SMP_OFF as well
    const uint32_t smp_mode = HEVCVD_SMP_CONCURRENT;

    LocalContext context{};
    callbacks_t callbacks{};
    callbacks.context.p = &context;
    callbacks_decoder_hevc_t hevc_callbacks{};
    hevc_callbacks.pic_callback = pictureQueuedCallback;
    if (smp_mode == HEVCVD_SMP_CONCURRENT || smp_mode == HEVCVD_SMP_OFF)
        hevc_callbacks.pic_decoded_callback = pictureDecodedCallback;

    bufstream_tt* decoder = createDecoderHEVC(&callbacks, &hevc_callbacks, nullptr);
    decoder->auxinfo(decoder, smp_mode, SET_SMP_MODE, 0, 0);
    decoder->auxinfo(decoder, HEVCVD_CP_NALU, SET_CHUNK_PARSE_MODE, 0, 0);

    uint8_t* buffer = reinterpret_cast<uint8_t*>(malloc(max_buffer_size));
    // AUD_NUT, nuh_layer_id = 0, temporal_layer_id = 0, pic_type = 2
    uint8_t aud_nalu[] = { 0, 0, 0, 1, 70, 1, 80 };

    for (int i = 0; i < frames.size(); i++) {
        const auto& frame = frames[i];
        uint64_t consumed = 0;
        uint64_t bytes = fread(buffer, sizeof(uint8_t), frame.size, f);
        uint8_t* p = buffer;
        for (const auto& nalu : frame.nal_units) {
            do {
                consumed = decoder->copybytes(decoder, p, static_cast<uint32_t>(nalu.size));
            } while (!consumed);
            assert(nalu.size == consumed);
            p += consumed;
        }

        printf("Frame %d fed to decoder (%zd NAL units). Feeding Access Unit Delimiter NALU\n", i, frame.nal_units.size());

        do {
            consumed = decoder->copybytes(decoder, aud_nalu, sizeof(aud_nalu));
        } while (!consumed);
        assert(sizeof(aud_nalu) == consumed);
    }

    while (decoder->copybytes(decoder, nullptr, 0))
        ;

    close_bufstream(decoder, false);

    return 0;
}
