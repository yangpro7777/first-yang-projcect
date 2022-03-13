/**
@brief Simple example of how to use colorimetry information in the HEVC Decoder
and shows only HDR to HDR and HDR to SDR conversion by conversion between transfer characteristics

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include "dec_hevc.h"
#include "mccolorimetry.h"
#include "mccolorspace.h"

enum ConversionModes
{
    HLG_TO_PQ = 1,
    PQ_TO_HLG,
    PQ_TO_SDR,
};

struct Context
{
    FILE* output_file;
    bufstream_tt* decoder;
    int32_t license_file_size;

    frame_tt frame;
    uint8_t* buffer;
    uint32_t buffer_size;
    ConversionModes mode;
    bool error = false;
};

static const char INPUT_FILE_NAME[] = "input.hevc";
static const char OUTPUT_FILE_NAME[] = "output.yuv";
static const char LICENSE_FILE_NAME[] = "license.lic";
static const uint32_t BUFFER_SIZE = 64 * 1024;

int32_t getLicenceData(context_t context, void* pData)
{
    Context* ctx = reinterpret_cast<Context*>(context.p);
    FILE* license_file = fopen(LICENSE_FILE_NAME, "rb");
    if (!license_file)
        return 0;

    if (pData) {
        if (ctx->license_file_size != fread(pData, sizeof(uint8_t), ctx->license_file_size, license_file))
            return 0;
    }
    else {
        size_t file_size = 0;
        ctx->license_file_size = 0;

        if (license_file && !fseek(license_file, 0, SEEK_END) && (file_size = ftell(license_file)) && file_size < INT32_MAX)
            ctx->license_file_size = static_cast<int32_t>(file_size);
        else
            printf("Error opening license file: %s\n", LICENSE_FILE_NAME);
    }

    fclose(license_file);
    return ctx->license_file_size;
}

bool isBufferSizeEnough(Context* ctx, const frame_colorspace_info_tt& colorspace_info) { return ctx->buffer && ctx->buffer_size >= colorspace_info.frame_size; }

bool setColorimteryMode(bufstream_tt* decoder, ConversionModes mode)
{
    colorimetry_t input_colorimetry;
    set_colorimetry_from_color_description(&input_colorimetry, COLOR_DESCRIPTION_AUTO);
    colorimetry_t output_colorimetry = input_colorimetry;

    // For the sake of simplicity in this example, the input and output colorimetries are hard-coded
    // In real life cases, the input colorimetry should be read from the stream
    switch (mode) {
        case HLG_TO_PQ:
            input_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2100_HLG;
            output_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_SMPTE_ST_2084;
            // For example set max content light level to 1000 and max picture-average light level to 300
            output_colorimetry.content_light_level_present = 1;
            output_colorimetry.content_light_level.max_content_light_level = 1000;
            output_colorimetry.content_light_level.max_pic_average_light_level = 300;
            break;
        case PQ_TO_HLG:
            input_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_SMPTE_ST_2084;
            output_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2100_HLG;
            break;
        case PQ_TO_SDR:
            input_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_SMPTE_ST_2084;
            output_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2020_10BIT;
            break;
        default:
            return false;
    }

    colorimetry_t* native_colorimetry;
    if (BS_OK != decoder->auxinfo(decoder, 0, GET_NATIVE_COLORIMETRY, &native_colorimetry, sizeof(colorimetry_t))) {
        printf("Can't get native colorimetry.");
        return false;
    }

    if (native_colorimetry->transfer_characteristics != input_colorimetry.transfer_characteristics) {
        printf("Native transfer characteristics for the stream is %d, but will be override by %d\n", native_colorimetry->transfer_characteristics,
            input_colorimetry.transfer_characteristics);
    }

    if (BS_OK != decoder->auxinfo(decoder, 0, OVERRIDE_NATIVE_COLORIMETRY, &input_colorimetry, sizeof(colorimetry_t))) {
        printf("Can't override native colorimetry.");
        return false;
    }

    if (BS_OK != decoder->auxinfo(decoder, 0, SET_OUTPUT_COLORIMETRY, &output_colorimetry, sizeof(colorimetry_t))) {
        printf("Can't set output colorimetry.");
        return false;
    }
    return true;
}

void picOutputCallback(context_t context, const hevc_picture_t* pic)
{
    frame_colorspace_info_tt colorspace_info;
    if (get_frame_colorspace_info(&colorspace_info, pic->width, pic->height, pic->fourcc, 0))
        return;

    auto ctx = reinterpret_cast<Context*>(context.p);

    bool is_first_frame = !ctx->buffer;
    if (is_first_frame)
        // Can be called on each frame
        ctx->error |= !setColorimteryMode(ctx->decoder, ctx->mode);

    if (!isBufferSizeEnough(ctx, colorspace_info)) {
        delete[] ctx->buffer;
        ctx->buffer = new uint8_t[colorspace_info.frame_size];
        ctx->buffer_size = colorspace_info.frame_size;
    }

    if (!fill_frame_from_colorspace_info(&colorspace_info, ctx->buffer, &ctx->frame)) {
        if (BS_OK == ctx->decoder->auxinfo(ctx->decoder, 0, GET_PIC, &ctx->frame, sizeof(frame_tt))) {
            // save frame to the output file
            for (uint32_t plane = 0; plane < colorspace_info.planes; ++plane)
                fwrite(ctx->frame.plane[plane], sizeof(uint8_t), colorspace_info.stride[plane] * colorspace_info.plane_height[plane], ctx->output_file);
        }
    }
}

int main(int argc, char* argv[])
{
    int32_t mode = 0;
    if (argc == 1 || !(mode = atoi(argv[1])) || mode < 0 || mode > PQ_TO_SDR) {
        printf("Usage: %s <mode>\n", argv[0]);
        printf("Supported conversion by changing the transfer characteristics:\n"
               "\t%d - HLG to PQ conversion.\n"
               "\t%d - PQ to HLG conversion.\n"
               "\t%d - PQ to SDR(BT.2020, BT.709, BT.601) conversion.\n",
            HLG_TO_PQ, PQ_TO_HLG, PQ_TO_SDR);
        return -1;
    }

    FILE* input_file = fopen(INPUT_FILE_NAME, "rb");

    if (!input_file) {
        printf("Error opening input file: %s\n", INPUT_FILE_NAME);
        return -1;
    }

    Context context{};
    context.output_file = fopen(OUTPUT_FILE_NAME, "wb");
    if (!context.output_file) {
        printf("Error opening output file: %s\n", OUTPUT_FILE_NAME);
        return -1;
    }

    callbacks_t callbacks{};
    callbacks.context = context_t{ &context };
    callbacks.get_license_data = getLicenceData;
    callbacks_decoder_hevc_t hevc_callbacks{};
    hevc_callbacks.pic_output_callback = picOutputCallback;

    context.decoder = createDecoderHEVC(&callbacks, &hevc_callbacks, nullptr);
    context.mode = static_cast<ConversionModes>(mode);

    auto buffer = new uint8_t[BUFFER_SIZE];
    auto bytes_count = static_cast<uint32_t>(fread(buffer, sizeof(uint8_t), BUFFER_SIZE, input_file));

    // While we have file data, feed it to the decoder
    while (bytes_count && !context.error) {
        uint32_t consumed = 0;
        while ((bytes_count - consumed) != 0 && !context.error) {
            // Feed data to the decoder
            consumed += context.decoder->copybytes(context.decoder, buffer + consumed, bytes_count - consumed);
            context.error |= (context.decoder->auxinfo(context.decoder, 0, CLEAN_PARSE_STATE, NULL, 0) & PARSE_ERR_FLAG) != 0;
        }

        bytes_count = static_cast<uint32_t>(fread(buffer, sizeof(uint8_t), BUFFER_SIZE, input_file));
    }

    // Once all data is read from the file, decode any remaining data left in the decoder
    while (context.decoder->copybytes(context.decoder, nullptr, 0) && !context.error)
        context.error |= (context.decoder->auxinfo(context.decoder, 0, CLEAN_PARSE_STATE, NULL, 0) & PARSE_ERR_FLAG) != 0;

    delete[] buffer;

    if (context.buffer)
        delete[] context.buffer;

    close_bufstream(context.decoder, 0);

    fclose(input_file);
    fclose(context.output_file);

    return context.error;
}
