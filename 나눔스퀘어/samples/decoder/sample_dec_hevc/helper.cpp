/******************************************************************************
File name: dec_hevc_helper.cpp
Purpose: HEVC playback app helper functions

Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.
Unauthorized reproduction or distribution of any portion is prohibited by law.
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#if defined(_WIN32)
#include <io.h>
#endif
#include <fcntl.h>
#include "unicode_tools.h"
#include "company.h"
#include "helper.h"
#include "misc.h"
#include <versioninfo.h>

// Initializing DecHevcHelper class.
// Open input and output files and merge command line and config file parameters
bool Helper::initialize(CommandLine const& command_line, Configuration const& config)
{
    if (command_line.out_file_name != NULL) {
        if (!openOutputFile(command_line.out_file_name))
            return false;
    }
    else if (strlen(config.outputfile) != 0) {
        if (!openOutputFile(config.outputfile))
            return false;
    }

    if (command_line.fourcc == ITEM_NOT_INIT) {
        if (config.fourcc[0] != '\0') {
            fourcc = MAKEFOURCC(config.fourcc[0], config.fourcc[1], config.fourcc[2], config.fourcc[3]);
            int32_t stride, img_start;
            if (!get_video_frame_size(128, 128, fourcc, &stride, &img_start))
                fourcc = FOURCC_I420;
        }
        else {
            fourcc = 0;
        }
    }
    else {
        fourcc = command_line.fourcc;
    }

    const std::map<std::string, hevc_decoding_toolset_t>& toolsets = enumerateDecodingToolsets();

    const char* hw_acc_name = command_line.hw_acc_name ? command_line.hw_acc_name : config.hw_acc_name;

    if (hw_acc_name && hw_acc_name[0]) {
        std::map<std::string, hevc_decoding_toolset_t>::const_iterator i = toolsets.find(std::string(hw_acc_name));
        if (i != toolsets.end())
            decoding_toolset = i->second;
        else {
            fprintf(log, "%s unknown hardware acceleration mode\n", hw_acc_name);
            return false;
        }
    }

    config_width = command_line.width != ITEM_NOT_INIT ? command_line.width : config.width;
    config_height = command_line.height != ITEM_NOT_INIT ? command_line.height : config.height;
    if ((config_width != 0 && config_height == 0) || (config_width == 0 && config_height != 0)) {
        fprintf(log, "Width and Height should be both set or not set.\n");
        return false;
    }

    threads = command_line.threads != ITEM_NOT_INIT ? command_line.threads : config.threads;
    nodeset = command_line.nodeset != ITEM_NOT_INIT ? command_line.nodeset : config.nodeset;
    md5 = command_line.md5 != ITEM_NOT_INIT ? 1 : (command_line.md5l != ITEM_NOT_INIT ? 2 : config.md5);
    progress = command_line.progress != ITEM_NOT_INIT ? 1 : config.progress != 0;
    verbose = !progress && command_line.quiet == ITEM_NOT_INIT && config.quiet == 0;
    md5_frame = verbose && (command_line.md5_frame != ITEM_NOT_INIT || config.md5_frame != 0);
    smp_mode = static_cast<hevc_smp_mode_t>(command_line.smp_mode != ITEM_NOT_INIT ? command_line.smp_mode : config.smp_mode);

    hw_adapter = command_line.hw_adapter != ITEM_NOT_INIT ? command_line.hw_adapter : config.hw_adapter;
    preview_mode = command_line.preview_mode != ITEM_NOT_INIT ? command_line.preview_mode : config.preview_mode;
    use_callbacks = command_line.use_callbacks != ITEM_NOT_INIT ? 1 : config.use_callbacks != 0;
    async_intput_output = command_line.async_input_output != ITEM_NOT_INIT ? 1 : config.async_intput_output != 0;
    hw_enumerate = command_line.hw_enumerate != ITEM_NOT_INIT ? true : false;

    if (!use_callbacks && async_intput_output) {
        fprintf(log, "Option async_intput_output should be set only if use_callbacks option is set.\n");
        return false;
    }

    if (!hw_enumerate) {
        if (!openInputFile(command_line.in_file_name != NULL ? command_line.in_file_name : config.inputfile))
            return false;
    }

    if (command_line.transfer_characteristics != ITEM_NOT_INIT)
        transfer_characteristics = static_cast<transfer_characteristics_t>(command_line.transfer_characteristics);

    license_file_name = command_line.license_file_name;

    skip_mode = config.skip_mode;
    cc_pix_range = command_line.cc_pix_range != ITEM_NOT_INIT ? command_line.cc_pix_range : config.cc_pix_range;
    max_temporal_layer = config.max_temporal_layer;
    print_sei_types = config.print_sei_types != 0;
    parse_frames = config.parse_frames != 0;
    deinterlacing_mode =
        static_cast<hevc_deinterlacing_mode_t>(command_line.deinterlacing_mode != ITEM_NOT_INIT ? command_line.deinterlacing_mode : config.deinterlacing_mode);

    // initialize the MD5 sum if necessary
    if (md5)
        MD5Init(&m_ctx_md5);

    frame_required = md5 || md5_frame || output_file || fourcc;
    raw_hardware_output = decoding_toolset > HEVCVD_DECODING_TOOLSET_CPU && !fourcc;

    convert_frame = (config_height && config_width) || cc_pix_range != RANGE_FULL_TO_FULL || fourcc || (deinterlacing_mode > 0);
    convert_frame |= transfer_characteristics != TRANSFER_CHARACTERISTICS_AUTO;
    convert_frame |= raw_hardware_output ? frame_required : 0;

#ifdef DEMO_LOGO
    convert_frame |= frame_required;
#endif

    if (convert_frame) {
        if (raw_hardware_output)
            fprintf(log, "Warning: using copyback from GPU memory to CPU memory.\n");

        if (!fourcc)
            fprintf(log, "Warning: using colorspace conversion! Speed not the best.\n");
    }

    return true;
}

void Helper::printHeader()
{
    fprintf(log, "\nMainConcept HEVC Video Decoder sample.\nCopyright(C) %s %s. All rights reserved.\n\n", MC_VERSION_YEAR, COMPANY_NAME);
    fflush(log);
}

void Helper::start()
{
    UnicodeTools::printf(log, "\nDecoding %s\n", UnicodeTools::unicodeArgument(m_input_file_name).c_str());
    // initialize some decode progress variables
    dec_start = frame_start_time = time_get_count();
    frame_start = display_start = 0;
}

void Helper::resetInput()
{
    // Get file ready for decoding
    fseek(input_file, 0, SEEK_SET);
}

void Helper::finish()
{
    // Save decoding finished time
    dec_stop = time_get_count();

    if (md5)
        MD5Final((uint8_t*)md5_digest, &m_ctx_md5, 1);

    if (progress)
        fprintf(log, "                                                                  ");
}

void Helper::printStreamSummary()
{
    fprintf(log, "\nTotal Frames: --------------- %d", pictures_decoded);
    fprintf(log, "\nDimensions: ----------------- %dx%d", width, height);

    if (md5) {
        fprintf(log, md5 == 1 ? "%s %08X%08X%08X%08X" : "%s %08x%08x%08x%08x", "\nMD5 sum: --------------------", md5_digest[3], md5_digest[2], md5_digest[1],
            md5_digest[0]);
    }
}

void Helper::printSummary()
{
    double seconds_elapsed = double(dec_stop - dec_start) / time_get_freq();
    fprintf(log, "\nTotal Time (ms): ------------ %0.2f", seconds_elapsed * 1000.0);

    if (pictures_decoded) {
        double fps = pictures_decoded / seconds_elapsed;
        fprintf(log, "\nTime per Frame (ms): -------- %0.2f", 1000.0 / fps);
        fprintf(log, "\nFrames per Second: ---------- %0.2f", fps);
    }
}

void Helper::updateMD5(uint8_t const* const planes[4], const int32_t width[4], const uint32_t height[4], const int32_t stride[4], uint8_t plane_count)
{
    for (uint8_t plane = 0; plane < plane_count; plane++)
        for (uint32_t h = 0; h < height[plane]; h++)
            MD5Update(&m_ctx_md5, const_cast<uint8_t*>(planes[plane]) + stride[plane] * h, width[plane]);
}

void Helper::outputFrame(uint8_t const* const planes[4], const int32_t width[4], const uint32_t height[4], const int32_t stride[4], uint8_t plane_count)
{
    for (uint8_t plane = 0; plane < plane_count; plane++)
        for (uint32_t h = 0; h < height[plane]; h++)
            fwrite(planes[plane] + stride[plane] * h, 1, width[plane], output_file);
}

void Helper::checkFrameMd5(
    uint8_t const* const planes[3], const int32_t width[3], const uint32_t height[3], const int32_t stride[3], uint8_t plane_count, uint8_t* frame_md5[3])
{
    if (!frame_md5[0]) {
        m_frame_md5_exist = false;
        return;
    }

    m_frame_md5_exist = true;
    uint8_t md5[16];
    context_md5_t context_md5;
    for (uint8_t plane = 0; plane < plane_count; plane++) {
        MD5Init(&context_md5);
        for (uint32_t h = 0; h < height[plane]; h++)
            MD5Update(&context_md5, const_cast<uint8_t*>(planes[plane]) + stride[plane] * h, width[plane]);
        MD5Final(md5, &context_md5, 0);
        m_frame_md5_result[plane] = 0 == memcmp(frame_md5[plane], md5, 16);
    }

    for (uint8_t plane = plane_count; plane < 3; plane++)
        m_frame_md5_result[plane] = true;
}

bool Helper::openInputFile(char const* const filename)
{
    m_input_file_name = path2real(filename, m_input_file_path_buffer, 250);
    input_file = UnicodeTools::openFile(m_input_file_name, true);
    if (!input_file) {
        UnicodeTools::printf(log, "Error opening input file: %s\n", UnicodeTools::unicodeArgument(m_input_file_name).c_str());
        return false;
    }

    return true;
}

bool Helper::openOutputFile(char const* const filename)
{
    char output_filename_buffer[250];
    const char* output_filename = path2real(filename, output_filename_buffer, 250);
    if (strcmp(output_filename, "-")) {
        output_file = UnicodeTools::openFile(output_filename, false);
    }
    else {
#if defined(_WIN32)
        _setmode(_fileno(stdout), _O_BINARY);
#endif
        output_file = stdout;
    }
    if (!output_file) {
        UnicodeTools::printf(log, "Error opening output file: %s\n", UnicodeTools::unicodeArgument(output_filename).c_str());
        return false;
    }

    return true;
}

void Helper::updateProgress()
{
    uint64_t current_time = time_get_count();
    uint32_t elapsed = static_cast<uint32_t>(current_time - frame_start_time);
    if (elapsed > time_get_freq()) {
        fprintf(log, "%u frames decoded, %.2f fps    \r", pictures_decoded, (pictures_decoded - frame_start) * time_get_freq() / (double)elapsed);
        frame_start = pictures_decoded;
        frame_start_time = current_time;
    }
}

void Helper::updateVerbose(
    const int32_t poc, const int32_t picture_type, const uint32_t width, const uint32_t height, const bool error, const bool skipped, const bool surfaces)
{
    static const char ColorPlanes[3] = { 'Y', 'U', 'V' };
    static const char PictureTypes[3] = { 'B', 'P', 'I' };

    fprintf(log, "Frame: %6d POC: %5d, SLICE_TYPE: %c %dx%d ", pictures_decoded - 1, poc, PictureTypes[picture_type], width, height);

    if (error)
        fprintf(log, "ERROR ");

    if (skipped)
        fprintf(log, "SKIPPED ");

#ifndef DEMO_LOGO
    if (decoding_toolset > HEVCVD_DECODING_TOOLSET_CPU && !skipped && !fourcc && !surfaces)
        fprintf(log, "FALLBACK ");
#endif // !DEMO_LOGO

    if (md5_frame) {
        if (m_frame_md5_exist && m_frame_md5_result[0] && m_frame_md5_result[1] && m_frame_md5_result[2]) {
            fprintf(log, "MD5 [ OK ]");
        }
        else if (m_frame_md5_exist) {
            fprintf(log, "MD5 [ ");

            for (uint8_t plane = 0; plane < 3; plane++)
                if (!m_frame_md5_result[plane])
                    fprintf(log, "%c - failed ", ColorPlanes[plane]);

            fprintf(log, "]");
        }
        else {
            fprintf(log, "MD5 [ missing ]");
        }
    }

    fprintf(log, "\n");
}

void Helper::printSeiInfo(hevc_sei_messages_t const* sei)
{
    hevc_sei_payload_s const* payload = sei->sei_payload;
    for (uint16_t i = 0; i < sei->num_messages; ++i, ++payload)
        fprintf(log, "SEI:    Payload type: %5d, Payload size: %5d\n", payload->payload_type, payload->payload_size);
}

void Helper::printError(const char* const msg) { fprintf(log, "Frame: %6d, %s\n", pictures_decoded - 1, msg); }
