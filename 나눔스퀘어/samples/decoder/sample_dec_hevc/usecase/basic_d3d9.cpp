/**
@brief simple example of how to use the HEVC API

@verbatim
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.

MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
@endverbatim
**/

#include <stdio.h>
#include <stdlib.h>
#include "dec_hevc.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

const size_t READ_BUFFER_SIZE = 64 * 1024;

bool process(bufstream_tt* decoder, FILE* input_file, FILE* output_file)
{
    // Check the results of the copybytes() call
    uint32_t state = decoder->auxinfo(decoder, 0, CLEAN_PARSE_STATE, NULL, 0);

    const uint32_t pic_decoded = PIC_DECODED_FLAG | PIC_VALID_FLAG | PIC_FULL_FLAG;
    if (state & PARSE_ERR_FLAG) {
        return false;
    }
    else if ((state & pic_decoded) == pic_decoded) // Success
    {
        SEQ_ParamsEx* sps;
        // Get resolution for decoded frame
        if (BS_OK != decoder->auxinfo(decoder, 0, GET_SEQ_PARAMSP, &sps, sizeof(SEQ_ParamsEx)))
            return false;

        frame_tt frame = { 0 };
        frame.width = sps->horizontal_size;
        frame.height = sps->vertical_size;

        // Get colorspace information for decoded frame
        if (BS_OK != decoder->auxinfo(decoder, 0, GET_NATIVE_FOURCC, &frame.four_cc, sizeof(frame.four_cc)))
            return false;

        frame_colorspace_info_tt cs_info;
        if (0 != get_frame_colorspace_info(&cs_info, frame.width, frame.height, frame.four_cc, 0))
            return false;

        // Allocate memory with additional 15 bytes for aligning.
        uint8_t* buffer = reinterpret_cast<uint8_t*>(malloc(cs_info.frame_size + 15));

        // Fill pixel buffers and strides in output frame on aligned memory
        fill_frame_from_colorspace_info(&cs_info, reinterpret_cast<uint8_t*>((size_t)(buffer + 15) & ~15), &frame);

        if (BS_OK == decoder->auxinfo(decoder, 0, GET_PIC, &frame, sizeof(frame_tt))) {
            for (uint8_t plane = 0; plane < cs_info.planes; plane++)
                for (uint32_t h = 0; h < cs_info.plane_height[plane]; h++)
                    fwrite(frame.plane[plane] + frame.stride[plane] * h, 1, cs_info.stride[plane], output_file);
        }

        free(buffer);
    }

    return true;
}

const char* INPUT_FILE_NAME = "input.hevc";
const char* OUTPUT_FILE_NAME = "output.nv12";

int main(int argc, char* argv[])
{
    //! [HEVC Decoder configuration]
    // Open bufstream interface;
    bufstream_tt* decoder = createDecoderHEVC(0, 0, 0);

    // Also can be initialized with HEVCVD_DECODING_TOOLSET_D3D11
    if (BS_OK != decoder->auxinfo(decoder, HEVCVD_DECODING_TOOLSET_D3D9, HWACC_SET_ACC_MODE, 0, 0)) {
        printf("Error initializing d3d9 toolset: %s\n", INPUT_FILE_NAME);
        return -1;
    }
    //! [HEVC Decoder configuration]

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

    uint8_t buffer[READ_BUFFER_SIZE];
    size_t bytes_available = 0;
    bool abort = false;

    // While we have file data, feed it to the decoder
    while (!abort && (bytes_available = fread(buffer, 1, READ_BUFFER_SIZE, input_file))) {
        uint32_t consumed = 0;

        do {
            // Feed data to the decoder
            consumed += decoder->copybytes(decoder, buffer + consumed, static_cast<uint32_t>(bytes_available - consumed));

            abort = !process(decoder, input_file, output_file);
        } while (!abort && bytes_available - consumed);
    }

    int32_t bytes_left = 0;

    // Once all data is read from the file, decode any remaining data left in the decoder
    if (!abort) {
        do {
            bytes_left = decoder->copybytes(decoder, NULL, 0);
            abort = !process(decoder, input_file, output_file);
        } while (!abort && bytes_left);
    }

    close_bufstream(decoder, 0);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
