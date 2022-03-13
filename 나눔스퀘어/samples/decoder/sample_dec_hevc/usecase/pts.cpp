/**
@brief simple example of how to use the HEVC API

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/
#include <stdio.h>
#include <stdlib.h>
#include "dec_hevc.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

const size_t READ_BUFFER_SIZE = 64 * 1024;
//#define TEST_REORDER

#ifndef TEST_REORDER
const char* INPUT_FILE_NAME = "CAINIT_F_SHARP_3.bit";
int pts_list[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 };

static size_t access_units_size[] = { 15867, 90, 1019, 1131, 3334, 871, 1276, 961, 3509, 899, 1374, 1176, 3690, 1018, 1454, 1002, 3935, 1240, 1502, 1070, 4044,
    980, 1392, 1093, 4103, 1016, 1487, 1140, 4348, 1060, 1617, 1255, 4158, 1141, 1694, 1371, 4308, 1148, 1521, 1234, 4279, 1158, 1706, 1341, 4567, 1263, 1802,
    1358, 4659, 1109, 1 };
#else
const char* INPUT_FILE_NAME = "INITQP_A_Sony_1.bit";
int pts_list[] = { 0, 2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 14, 13, 16, 15, 18, 17, 20, 19, 22, 21, 24, 23, 26, 25, 28, 27, 30, 29, 32, 31, 34, 33, 36, 35, 38,
    37, 40, 39, 42, 41, 44, 43, 46, 45, 48, 47, 50, 49, 52, 51, 54, 53, 56, 55, 58, 57, 59 };
static size_t access_units_size[] = { 23526, 14055, 1341, 9571, 822, 9491, 635, 10593, 603, 9088, 641, 9489, 618, 9234, 719, 9139, 506, 9281, 507, 8511, 556,
    8949, 471, 7044, 601, 9339, 538, 7965, 564, 8041, 644, 7963, 595, 7731, 549, 6596, 451, 6005, 503, 7510, 436, 7597, 543, 8604, 499, 7234, 472, 9109, 449,
    7218, 418, 7560, 419, 7205, 483, 7830, 546, 7548, 468, 3491, 1 };
#endif

static const uint32_t PTS_COUNT = sizeof(pts_list) / sizeof(pts_list[0]);

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
        frame_tt frame = { 0 };
        if (BS_OK == decoder->auxinfo(decoder, 0, GET_PIC, &frame, sizeof(frame_tt))) {

            frame_colorspace_info_tt cs_info;
            if (0 != get_frame_colorspace_info(&cs_info, frame.width, frame.height, frame.four_cc, 0))
                return false;

            for (uint8_t plane = 0; plane < cs_info.planes; plane++)
                for (uint32_t h = 0; h < cs_info.plane_height[plane]; h++)
                    fwrite(frame.plane[plane] + frame.stride[plane] * h, 1, cs_info.stride[plane], output_file);
        }

        PIC_Params* picPara = 0;
        decoder->auxinfo(decoder, 0, GET_PIC_PARAMSP, &picPara, sizeof(*picPara));
        // GET the Output PTS
        int32_t* get_pts;
        if (BS_OK != decoder->auxinfo(decoder, 0, GET_LAST_PTS, &get_pts, sizeof(int)))
            printf("  -GET_LAST_PTS failed, POC: %d\n", picPara->temporal_reference);
        else
            printf("  -GET_LAST_PTS %2d, POC: %d\n", *get_pts, picPara->temporal_reference);
    }

    return true;
}

const char* OUTPUT_FILE_NAME = "output.yuv";

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

    bufstream_tt* decoder = createDecoderHEVC(0, 0, 0);

    uint8_t buffer[READ_BUFFER_SIZE];
    size_t bytes_available = 0;
    bool abort = false;
    uint32_t access_unit = 0;

    // While we have file data, feed it to the decoder
    while (!abort && (bytes_available = fread(buffer, 1, access_units_size[access_unit], input_file))) {
        uint32_t consumed = 0;

        if (access_unit < PTS_COUNT) {
            // SET PTS time Before every NAL unit or access unit copy
            if (BS_OK != decoder->auxinfo(decoder, 0, SET_PIC_PTS, &pts_list[access_unit], sizeof(int)))
                printf("+  SET_PIC_PTS %d failed\n", pts_list[access_unit]);
            else
                printf("+  SET_PIC_PTS %d\n", pts_list[access_unit]);

            access_unit++;
        }
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
            bytes_left = decoder->copybytes(decoder, NULL, 0); // Decode
            abort = !process(decoder, input_file, output_file);
        } while (!abort && bytes_left);
    }

    close_bufstream(decoder, 0);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
