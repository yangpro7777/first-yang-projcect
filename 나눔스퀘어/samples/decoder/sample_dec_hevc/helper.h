/*****************************************************************************
 File name: dec_hevc_helper.h
 Purpose: Helper class that holds common decoding variables

 Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.
 Unauthorized reproduction or distribution of any portion is prohibited by law.
******************************************************************************/

#ifndef UUID_F42C0789_4CA2_47AD_9A9F_33EF44133922
#define UUID_F42C0789_4CA2_47AD_9A9F_33EF44133922

#include "dec_hevc.h"
#include "misc.h"
#include "sample_common_misc.h"
#include "command_line.h"
#include "configuration.h"
#include "mccolorspace.h"

/* Base HEVC decoder helper class */
class Helper
{
public:
    Helper()
      : input_file(NULL),
        output_file(NULL),
        log(stderr),
        license_file_name(NULL),
        dec_start(0),
        dec_stop(0),
        frame_start(0),
        display_start(0),
        frame_duration(0),
        decoder_options(INTERN_REORDERING_FLAG),
        license_file_size(0),
        transfer_characteristics(TRANSFER_CHARACTERISTICS_AUTO),
        width(0),
        height(0),
        pictures_decoded(0),
        decoding_toolset(HEVCVD_DECODING_TOOLSET_CPU),
        raw_hardware_output(false),
        frame_required(false),
        convert_frame(false),
        m_frame_md5_exist(false)
    {
    }

    ~Helper()
    {
        if (input_file)
            fclose(input_file);

        if (output_file) {
            fflush(output_file);

            if (output_file != stdout)
                fclose(output_file);
        }
    }

    bool initialize(CommandLine const& command_line, Configuration const& config);

    void printHeader();
    void start();
    void resetInput();
    void finish();
    void printStreamSummary();
    void printSummary();

    void updateMD5(uint8_t const* const planes[4], const int32_t width[4], const uint32_t height[4], const int32_t stride[4], uint8_t plane_count);
    void outputFrame(uint8_t const* const planes[4], const int32_t width[4], const uint32_t height[4], const int32_t stride[4], uint8_t plane_count);
    void checkFrameMd5(
        uint8_t const* const planes[3], const int32_t width[3], const uint32_t height[3], const int32_t stride[3], uint8_t plane_count, uint8_t* frame_md5[3]);

    void updateProgress();
    void updateVerbose(
        const int32_t poc, const int32_t picture_type, const uint32_t width, const uint32_t height, const bool error, const bool skipped, const bool surfaces);
    void printSeiInfo(hevc_sei_messages_t const* sei);
    void printError(const char* const msg);

    FILE* input_file;
    FILE* output_file;
    FILE* log;
    char* license_file_name;

    uint64_t dec_start;        // Time we start decoding the HEVC file
    uint64_t dec_stop;         // Time we stop decoding the HEVC file
    uint64_t frame_start;      // First decoded frame number in current time frame
    uint64_t frame_start_time; // Time we started decoding the current frame
    uint64_t display_start;    // Used in frame duration calculations
    uint64_t frame_duration;
    uint32_t decoder_options;
    int32_t license_file_size;
    int32_t width;             // Video width
    int32_t height;            // Video height
    int32_t config_width;      // Video width
    int32_t config_height;     // Video height
    uint32_t fourcc;           // Desired fourcc we should convert source decoder frames to
    uint32_t pictures_decoded; // Number of pictures sent to pic_output_callback()
    int32_t threads;           // Number of threads, used by decoder
    uint64_t nodeset;          // A bitmask of NUMA nodes the decoder should use
    int32_t skip_mode;         // Skip mode decoder should use
    int32_t hw_adapter;
    uint32_t max_temporal_layer;
    nalu_types_e latest_nalu_type;            // Last parsed NALU type
    hevc_smp_mode_t smp_mode;                 // Multithreading mode decoder should use
    hevc_decoding_toolset_t decoding_toolset; // Host hardware where to offload decoding
    hevc_deinterlacing_mode_t deinterlacing_mode;
    transfer_characteristics_t transfer_characteristics;
    uint8_t cc_pix_range; // Pixel range conversion
    uint8_t md5;          // Mode md5 should be calculated
    uint8_t preview_mode;
    bool verbose;
    bool progress;  // Should we show decoder progress in the output log?
    bool md5_frame; // Check md5 for frame, if available
    bool print_sei_types;
    bool use_callbacks;
    bool async_intput_output;
    bool parse_frames;
    bool hw_enumerate;
    bool raw_hardware_output;
    bool frame_required;
    bool convert_frame;
    uint32_t md5_digest[4];

private:
    bool openInputFile(char const* const filename);
    bool openOutputFile(char const* const filename);

    context_md5_t m_ctx_md5; // Stream md5 context
    const char* m_input_file_name;
    char m_input_file_path_buffer[250];
    bool m_frame_md5_result[3];
    bool m_frame_md5_exist;
};

#endif
