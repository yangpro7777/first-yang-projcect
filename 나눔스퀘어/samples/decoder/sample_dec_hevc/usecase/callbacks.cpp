/**
@brief simple example of how to use the HEVC API

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/
#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "dec_hevc.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

static const size_t READ_BUFFER_SIZE = 64 * 1024;

void sps_parsed_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_seq_par_set_t* sps)
{
    fprintf(stderr, "sps_callback id %d\n", sps->sps_id);
}

void pps_parsed_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_pic_par_set_t* pps)
{
    fprintf(stderr, "pps_callback id %d\n", pps->pps_id);
}

void vps_parsed_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_vid_par_set_t* vps)
{
    fprintf(stderr, "vps_callback id %d\n", vps->vps_id);
}

void sei_parsed_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_sei_payload_t* messages, const uint32_t count)
{
    fprintf(stderr, "sei_callback count %d\n", count);
}

void slice_header_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_slice_hdr_t* slice_hdr)
{
    static const char* types[] = { "B", "P", "I" };
    fprintf(stderr, "slice_header_callback %s\n", types[slice_hdr->slice_type]);
}

void nalu_parsed_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_nalu_t* nalu)
{
    fprintf(stderr, "nalu_callback offset %llu size %llu\n", nalu->offset, nalu->size);
}

void picture_parsed_callback(context_t context, const hevc_picture_t* hevc_pic) { fprintf(stderr, "pic_callback poc %d\n", hevc_pic->poc); }

void picture_decoded_callback(context_t context, const hevc_picture_t* hevc_pic) { fprintf(stderr, "pic_decoded_callback poc %d\n", hevc_pic->poc); }

//! [Process output picture]
void picture_output_callback(context_t context, const hevc_picture_t* hevc_pic)
{
    FILE* output_file = reinterpret_cast<FILE*>(context.p);
    if (hevc_pic->skipped)
        return;

    if (hevc_pic->pixel[0] == 0) {
        // DEMO ONLY: pixels and hardware surface are not directly available in demo mode. \see sample_dec_hevc_basic_demo
        fprintf(stderr, "pic_output_callback poc %d (no pixels in demo version)\n", hevc_pic->poc);
        return;
    }

    fprintf(stderr, "pic_output_callback poc %d\n", hevc_pic->poc);

    frame_colorspace_info_tt cs_info;
    if (0 != get_frame_colorspace_info(&cs_info, hevc_pic->width, hevc_pic->height, hevc_pic->fourcc, 0))
        return;

    for (uint8_t plane = 0; plane < cs_info.planes; ++plane)
        for (uint32_t h = 0; h < cs_info.plane_height[plane]; ++h)
            fwrite(hevc_pic->pixel[plane] + hevc_pic->stride[plane] * h, 1, cs_info.stride[plane], output_file);
}
//! [Process output picture]

void picture_skipped_callback(context_t context, const hevc_picture_t* hevc_pic) { fprintf(stderr, "pic_skipped_callback poc %d\n", hevc_pic->poc); }

void error_callback(context_t context, const hevc_picture_t* hevc_pic, const hevc_runtime_error_t code) { fprintf(stderr, "error_callback code %d\n", code); }

static const char* INPUT_FILE_NAME = "input.hevc";
static const char* OUTPUT_FILE_NAME = "output.yuv";

//! [Callback functions dispatcher]
void get_callback(callbacks_decoder_hevc_t& decoder_hevc)
{
    decoder_hevc.sps_callback = sps_parsed_callback;
    decoder_hevc.pps_callback = pps_parsed_callback;
    decoder_hevc.vps_callback = vps_parsed_callback;
    decoder_hevc.sei_callback = sei_parsed_callback;
    decoder_hevc.slice_header_callback = slice_header_callback;
    decoder_hevc.nalu_callback = nalu_parsed_callback;
    decoder_hevc.pic_callback = picture_parsed_callback;
    decoder_hevc.pic_decoded_callback = picture_decoded_callback;
    decoder_hevc.pic_output_callback = picture_output_callback;
    decoder_hevc.pic_skipped_callback = picture_skipped_callback;
    decoder_hevc.error_callback = error_callback;
}
//! [Callback functions dispatcher]

int main(int argc, char* argv[])
{
    FILE* input_file = fopen(INPUT_FILE_NAME, "rb");

    if (!input_file) {
        printf("Error opening input file: %s\n", INPUT_FILE_NAME);
        return -1;
    }

    FILE* output_file = fopen(OUTPUT_FILE_NAME, "wb");

    if (!output_file) {
        printf("Error opening output file: %s\n", OUTPUT_FILE_NAME);
        return -1;
    }
    //! [Open bufstream interface]
    callbacks_t callbacks{};
    callbacks_decoder_hevc_t callbacks_dec_hevc{};
    callbacks.context.p = output_file;
    get_callback(callbacks_dec_hevc);
    bufstream_tt* decoder = createDecoderHEVC(&callbacks, &callbacks_dec_hevc, 0);
    //! [Open bufstream interface]

    // Set concurrent SMP mode in order to get deterministic output. For demonstration purposes only.
    if (decoder->auxinfo(decoder, HEVCVD_SMP_CONCURRENT, SET_SMP_MODE, 0, 0) != BS_OK) {
        printf("Failed to set SMP mode\n");
        return -1;
    }

    uint8_t buffer[READ_BUFFER_SIZE];
    size_t bytes_available = 0;
    bool abort = false;

    // While we have file data, feed it to the decoder
    while (!abort && (bytes_available = fread(buffer, 1, READ_BUFFER_SIZE, input_file))) {
        uint32_t consumed = 0;

        do {
            // Feed data to the decoder
            consumed += decoder->copybytes(decoder, buffer + consumed, static_cast<uint32_t>(bytes_available - consumed));

            const uint32_t state = decoder->auxinfo(decoder, 0, CLEAN_PARSE_STATE, NULL, 0);
            if (state & INTERNAL_ERROR) {
                abort = true;
                break;
            }

        } while (!abort && bytes_available - consumed);
    }

    int32_t bytes_left = 0;

    // Once all data is read from the file, decode any remaining data left in the decoder
    if (!abort) {
        do {
            bytes_left = decoder->copybytes(decoder, NULL, 0);

            const uint32_t state = decoder->auxinfo(decoder, 0, CLEAN_PARSE_STATE, NULL, 0);
            if (state & INTERNAL_ERROR)
                abort = true;

        } while (!abort && bytes_left);
    }

    close_bufstream(decoder, 0);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
