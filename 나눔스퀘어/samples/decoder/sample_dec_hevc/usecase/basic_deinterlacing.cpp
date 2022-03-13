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
#include <unordered_map>
#include <string>

#include "dec_hevc.h"
#include "mcfourcc.h"
#include "mccolorspace.h"
#include "../stream_info.h"

//! [Retrieve and dump decoded frame or field]
bool writeToFile(bufstream_tt* decoder, hevc_get_pic_t pic_type, FILE* output_file)
{
    //! [Obtain raw buffer of picture]
    frame_tt frame{};
    frame.four_cc = 0; // configure to raw outputs, no need allocate buffers
    if (BS_OK != decoder->auxinfo(decoder, pic_type, GET_PIC, &frame, sizeof(frame_tt)))
        return false;

    frame_colorspace_info_tt cs_info;
    if (0 != get_frame_colorspace_info(&cs_info, frame.width, frame.height, frame.four_cc, 0))
        return false;

    // Get the sequence header for width and height
    SEQ_ParamsEx* sps;

    std::unordered_map<hevc_get_pic_t, std::string, std::hash<int>> pic_type_names{
        { HEVCVD_GET_PIC_FRAME, "Frame" },
        { HEVCVD_GET_PIC_TOP_FIELD, "Top field" },
        { HEVCVD_GET_PIC_BOTTOM_FIELD, "Bottom field" },
    };

    uint32_t* user_data = nullptr;
    if (BS_OK != decoder->auxinfo(decoder, pic_type, GET_SEQ_PARAMSPEX, &sps, sizeof(SEQ_ParamsEx)))
        return false;

    printf("%-12s %dx%d", pic_type_names[pic_type].c_str(), sps->horizontal_size, sps->vertical_size);
    if (BS_OK == decoder->auxinfo(decoder, pic_type, GET_USER_DATAP, &user_data, sizeof(user_data))) {
        printf(", user data: %d", *user_data);
    }

    if (BS_OK == decoder->auxinfo(decoder, pic_type, GET_LAST_PTS, &user_data, sizeof(user_data))) {
        printf(", pts: %d", *user_data);
    }

    printf("\n");

    for (uint8_t plane = 0; plane < cs_info.planes; plane++)
        for (uint32_t h = 0; h < cs_info.plane_height[plane]; h++)
            fwrite(frame.plane[plane] + frame.stride[plane] * h, 1, cs_info.stride[plane], output_file);

    return true;
}
//! [Retrieve and dump decoded frame or field]

bool process(bufstream_tt* decoder, FILE* input_file, FILE* output_frame_file, FILE* output_field_file)
{
    // Check the results of the copybytes() call
    uint32_t state = decoder->auxinfo(decoder, 0, CLEAN_PARSE_STATE, NULL, 0);

    const uint32_t pic_decoded = PIC_DECODED_FLAG | PIC_VALID_FLAG | PIC_FULL_FLAG;
    if (state & PARSE_ERR_FLAG) {
        return false;
    }
    else if ((state & pic_decoded) == pic_decoded) // Success
    {
        writeToFile(decoder, HEVCVD_GET_PIC_FRAME, output_frame_file);
        writeToFile(decoder, HEVCVD_GET_PIC_TOP_FIELD, output_field_file);
        writeToFile(decoder, HEVCVD_GET_PIC_BOTTOM_FIELD, output_field_file);
    }

    return true;
}

const char* INPUT_FILE_NAME = "input.hevc";
const char* OUTPUT_FRAME_FILE_NAME = "output_by_frames.yuv";
const char* OUTPUT_FIELD_FILE_NAME = "output_by_fields.yuv";

int main(int argc, char* argv[])
{
    enum
    {
        READ_BUFFER_SIZE = 64 * 1024
    };

    // Open bufstream interface;
    bufstream_tt* decoder = createDecoderHEVC(0, 0, 0);
    decoder->auxinfo(decoder, HEVCVD_DEINTERLACING_WEAVE, HEVCVD_SET_DEINTERLACING_MODE, 0, 0);

    FILE* input_file = fopen(INPUT_FILE_NAME, "rb");

    if (!input_file) {
        printf("Error opening input file: %s\n", INPUT_FILE_NAME);
        return -1;
    }

    const std::vector<FrameInfo>& stream_info = getStreamInfo(input_file);

    FILE* output_frame_file = fopen(OUTPUT_FRAME_FILE_NAME, "wb");
    FILE* output_field_file = fopen(OUTPUT_FIELD_FILE_NAME, "wb");

    if (!output_frame_file) {
        printf("Error opening output file: %s\n", OUTPUT_FRAME_FILE_NAME);
        return -1;
    }

    if (!output_field_file) {
        printf("Error opening output file: %s\n", OUTPUT_FIELD_FILE_NAME);
        return -1;
    }

    uint8_t buffer[READ_BUFFER_SIZE];
    size_t bytes_available = 0;
    bool abort = false;

    // While we have file data, feed it to the decoder
    for (const FrameInfo& frame_info : stream_info) {

        uint64_t frame_size = frame_info.size;

        decoder->auxinfo(decoder, 0, SET_PIC_PTS, const_cast<uint32_t*>(&frame_info.output_order), sizeof(frame_info.output_order));
        decoder->auxinfo(decoder, 0, SET_USER_DATA, const_cast<uint32_t*>(&frame_info.output_order), sizeof(frame_info.output_order));
        while (frame_size) {

            bytes_available = fread(buffer, 1, std::min<size_t>(READ_BUFFER_SIZE, static_cast<size_t>(frame_size)), input_file);
            frame_size -= bytes_available;
            uint32_t consumed = 0;

            do {
                // Feed data to the decoder
                consumed += decoder->copybytes(decoder, buffer + consumed, (uint32_t)(bytes_available - consumed));

                abort = !process(decoder, input_file, output_frame_file, output_field_file);
            } while (!abort && bytes_available - consumed);
        }
    }

    int32_t bytes_left = 0;

    // Once all data is read from the file, decode any remaining data left in the decoder
    if (!abort) {
        do {
            bytes_left = decoder->copybytes(decoder, NULL, 0);
            abort = !process(decoder, input_file, output_frame_file, output_field_file);
        } while (!abort && bytes_left);
    }

    close_bufstream(decoder, 0);

    fclose(input_file);
    fclose(output_frame_file);
    fclose(output_field_file);

    return abort ? -1 : 0;
}
