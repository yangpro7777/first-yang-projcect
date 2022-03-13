/**
@file basic_external_threadpool.cpp
@brief simple example of how to use the HEVC API with external threadpool

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
#include <mcruntime.h>

#define READ_BUFFER_SIZE (64 * 1024)

#define bool uint8_t
#define true 1
#define false 0

bool process(bufstream_tt* decoder, FILE* input_file, FILE* output_file)
{
    // Check the results of the copybytes() call
    uint32_t state = decoder->auxinfo(decoder, 0, CLEAN_PARSE_STATE, NULL, 0);
    uint32_t h;
    uint8_t plane;

    const uint32_t pic_decoded = PIC_DECODED_FLAG | PIC_VALID_FLAG | PIC_FULL_FLAG;
    if (state & PARSE_ERR_FLAG) {
        return false;
    }
    else if ((state & pic_decoded) == pic_decoded) // Success
    {
        //! [Obtain raw buffer of picture]
        frame_tt frame = { 0 };
        frame.four_cc = 0; // configure to raw outputs, no need allocate buffers
        if (BS_OK == decoder->auxinfo(decoder, 0, GET_PIC, &frame, sizeof(frame_tt))) {

            frame_colorspace_info_tt cs_info;
            if (0 != get_frame_colorspace_info(&cs_info, frame.width, frame.height, frame.four_cc, 0))
                return false;
            // ....
            //! [Obtain raw buffer of picture]

            for (plane = 0; plane < cs_info.planes; plane++)
                for (h = 0; h < cs_info.plane_height[plane]; h++)
                    fwrite(frame.plane[plane] + frame.stride[plane] * h, 1, cs_info.stride[plane], output_file);
        }
    }

    return true;
}

const char* INPUT_FILE_NAME = "input.hevc";
const char* OUTPUT_FILE_NAME = "output.yuv";

int main(int argc, char* argv[])
{
    int32_t type = 0;
    if (argc == 1 || (type = atoi(argv[1])) < 0 || type > TP_TYPE_GCD) {
        printf("Usage: %s <type>\n", argv[0]);
        printf("Supported threadpool types:\n"
               "\t%d - High performance scalable threadpool\n"
               "\t%d - Basic threadpool\n"
               "\t%d - GCD threadpool (macOS only)\n",
               TP_TYPE_SCALABLE, TP_TYPE_BASIC, TP_TYPE_GCD
        );
        return -1;
    }

    // Create external threadpool
    mcr_thread_pool_t threadpool;
    if (threadpoolCreate(0, (tp_type_t)type, nullptr, &threadpool) != MCR_ERROR_OK) {
        printf("Threadpool type %d is not supported on this platform.", type);
        return -1;
    }

    stream_params_t stream_params{};
    stream_params.threadpool = threadpool;

    // Open bufstream interface;
    bufstream_tt* decoder = createDecoderHEVC(0, 0, &stream_params);

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
            consumed += decoder->copybytes(decoder, buffer + consumed, (uint32_t)(bytes_available - consumed));

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

    threadpoolDestroy(threadpool, nullptr);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
