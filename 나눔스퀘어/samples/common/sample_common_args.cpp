/********************************************************************
created:    2009/07/20 - 12:00
file name:  sample_common_args.cpp
purpose:    Common command line parsing routines

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
*********************************************************************/

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#if !defined(__linux__) && !defined(__native_client__) && !defined(__APPLE__)
#include <conio.h> // _kbhit()
#endif

#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "mcfourcc.h"
#include "mcprofile.h"
#include "mccolorspace.h"

#if defined _MSC_VER
#define strcmp_lower_case _stricmp
#else
#include <strings.h>
#define strcmp_lower_case strcasecmp
#endif

// add new command line parameters to this table,
// make sure to keep 2nd and 3rd column commands unique and to not shift the index of IDS_CUSTOM_ARG!
static const arg_item_desc_t g_arg_items[] =
{
    // common args
    { IDS_VIDEO_FILE,       {"v",        "video"},        ItemTypeString, 0,              "raw input video file name"},
    { IDS_AUDIO_FILE,       {"a",        "audio"},        ItemTypeString, 0,              "raw input audio file name"},
    { IDS_INPUT_FILE,       {"i",        "input"},        ItemTypeString, 0,              "input file name"},
    { IDS_OUTPUT_FILE,      {"o",        "output"},       ItemTypeString, 0,              "output file name"},
    { IDI_BITRATE,          {"b",        "bitrate"},      ItemTypeInt,    0,              "target bit rate (kbps)"},
    { IDI_BITS_PER_SAMPLE,  {"bps",      ""},             ItemTypeInt,    0,              "bits per sample"},
    { IDS_CONFIG_FILE,      {"c",        "config"},       ItemTypeString, 0,              "config file name"},
    { IDS_CUSTOM_ARG,       {"",         ""},             ItemTypeString, 0,              "custom argument"},             // this one is for check_params()
    { IDS_OUTPUT_FILE_VID,  {"ov",       "outputvideo"},  ItemTypeString, 0,              "output file name video"},
    { IDS_OUTPUT_FILE_AUD,  {"oa",       "outputaudio"},  ItemTypeString, 0,              "output file name audio"},
    { IDI_OUTPUT_BITS_PER_SAMPLE, {"obps", ""},           ItemTypeInt,    0,              "output bits per sample"},
    { IDS_REC_FILE,         {"preview",  ""},             ItemTypeString, 0,              "output file name for reconstructed stream"},
    { IDS_ORG_FILE,         {"original",   ""},           ItemTypeString, 0,              "output file name for source stream"},
    { IDS_OPS_FILE,         {"opsfile",  ""},             ItemTypeString, 0,              "name of operations list file"},
    { IDS_LOG_FILE,         {"logfile",  ""},             ItemTypeString, 0,              "name of output logging file"},
    { IDS_SERIAL_NUMBER,    {"sn",       "serialnumber"}, ItemTypeString, 0,              "serial number"},
    { IDI_NUM_THREADS,      {"nth",      "numthreads"},   ItemTypeInt,    0,              "number of threads"},
    { IDN_QUIET,            {"q",        "quiet"},        ItemTypeNoArg,  1,              "quiet mode"},
    { IDN_HEADERS,          {"",         "headers"},      ItemTypeNoArg,  1,              "print header information"},
    { IDS_STAT_FILE,        {"st",       "statistics"},   ItemTypeString, 0,              "statistics file name"},
    { IDI_TEMPORAL_ID,      {"t",        "temporalid"},   ItemTypeInt,    0,              "max HEVC temporal ID to extract"},

    //{ IDS_EXT_MEM_ALLOC,    {"",         ""},             ItemTypeNoArg,  0,              "no external memory allocator"},
    { IDS_EXT_MEM_ALLOC,    {"malloc",   "memalloc"},     ItemTypeNoArg,  1,              "external memory allocator"},
    
    // MC preset
    { IDI_PRESET,           {"preset",   ""},             ItemTypeInt,    0,              "MC preset"},

    // video args
    { IDI_V_WIDTH,          {"w",        "width"},        ItemTypeInt,    0,              "picture width"},
    { IDI_V_HEIGHT,         {"h",        "height"},       ItemTypeInt,    0,              "picture height"},
    { IDI_V_LINE_SIZE,      {"l",        "linesize"},     ItemTypeInt,    0,              "picture stride"},
    { IDD_V_FRAMERATE,      {"f",        "framerate"},    ItemTypeDouble, 0,              "frame rate"},
    { IDI_V_NUM_LAYERS,     {"nl",       "numlayers"},    ItemTypeInt,    0,              "number of scalability layers"},
    { IDI_V_SR_MODE,        {"srm",      "sr_mode"},      ItemTypeInt,    0,              "smart render mode"},
    { IDI_V_MULTIPASS_MODE, {"mpass",    "multipass"},    ItemTypeInt,    1,              "pass to run for multi-pass encoding"},
    { IDI_V_PAL,            {"pal",      ""},             ItemTypeInt,    0,              "PAL video"},
    { IDI_V_START_FRAME,    {"tstart",   ""},             ItemTypeInt,    0,              "Start frame for processing"},
    { IDI_V_DURATION,       {"tdurat",   ""},             ItemTypeInt,    0,              "A number of frames for processing"},
    { IDN_V_SWITCH_I,       {"switchi",  ""},             ItemTypeNoArg,  1,              "SR <-> non SR switching at I frame"},
    { IDN_V_PROGRESS,       {"progress", ""},             ItemTypeNoArg,  1,              "Show progress during decoding"},
    { IDN_V_MD5,            {"md5",      ""},             ItemTypeNoArg,  1,              "calculate MD5 of decoded frames"},
    { IDN_V_MD5L,           {"md5l",     ""},             ItemTypeNoArg,  1,              "calculate MD5 of decoded frames (lowercase)"},
    { IDN_V_MD5_FRAME,      {"md5frame", ""},             ItemTypeNoArg,  1,              "calculate MD5 of each frame and check against SEI hash if available" },
    { IDN_V_SMP,            {"smp",      ""},             ItemTypeInt,   -1,              "SMP mode"},
    { IDI_V_FRAMECHANGE,    {"nframe",   "" },            ItemTypeInt,   -1,              "Number of change settings frame" },

    { IDN_V_FOURCC,         {"",         ""},             ItemTypeNoArg,  0,              "input fourCC"},                // this one is for check_params()
    { IDN_V_FOURCC,         {"cs",      "colorspace"},    ItemTypeInt,    0,              "Colorspace"},
    { IDN_V_FOURCC,         {"cs0",     ""},                ItemTypeNoArg,  0,              "Native colorspace"},

    { IDI_V_PERFORMANCE,    {"perf",     ""},             ItemTypeInt,    0,              "performance preset"},
    { IDN_V_COLOR_MATRIX,   {"bt601",    ""},             ItemTypeNoArg,  601,            "ITU-R BT.601"},
    { IDN_V_COLOR_MATRIX,   {"bt709",    ""},             ItemTypeNoArg,  709,            "ITU-R BT.709"},

    { IDN_V_INTERLACED,     {"",         ""},             ItemTypeNoArg,  0,              "interlaced / progressive"},    // this one is for check_params()
    { IDN_V_INTERLACED,     {"p",        "progressive"},  ItemTypeNoArg,  0,              "progressive"},
    { IDN_V_INTERLACED,     {"i",        "interlaced"},   ItemTypeNoArg,  1,              "interlaced (field pictures)"},
    { IDN_V_INTERLACED,     {"mbaff",    ""},             ItemTypeNoArg,  2,              "interlaced (macroblock-adaptive frame-field)"},

    { IDN_V_SNR_SCALABILITY,{"",         ""},             ItemTypeNoArg,  0,              "SNR scalability type"},    // this one is for check_params()
    { IDN_V_SNR_SCALABILITY,{"cgs",      ""},             ItemTypeNoArg,  0,              "CGS"},
    { IDN_V_SNR_SCALABILITY,{"mgs",      ""},             ItemTypeNoArg,  1,              "MGS"},

    // audio args
    { ID_A_SAMPLERATE,      {"s",        "samplerate"},   ItemTypeInt,    0,              "audio sample rate"},
    { ID_A_CHANNELS,        {"ch",       "channels"},     ItemTypeInt,    0,              "number of channels"},
    { ID_A_OUTPUT_SAMPLERATE, {"os",     "outsamplerate"}, ItemTypeInt,   0,              "output audio sample rate"},
    { ID_A_OUTPUT_CHANNELS, {"och",      "outchannels"},  ItemTypeInt,    0,              "output number of channels"},
    { ID_A_PRIMING_CUT,     {"priming",  ""},             ItemTypeInt,    0,              "the encoder delay duration of priming samples in PTS"},
    { ID_A_ENCODER_IDX,     {"encoder_idx",  ""},         ItemTypeInt,    0,              "the audio encoder idx"},
    { ID_A_PLAYBACK_DUR,    {"playback", ""},             ItemTypeInt64,  0,              "the playback time duration of audio stream in PTS"},
    { ID_A_RESTART_INTERVAL,{"restart_interval", ""},     ItemTypeInt,    0,              "audio restart interval in PTS"},
    { ID_A_OBJECT_TYPE,     { "aot",       "" },          ItemTypeString, 0,              "audio object type" },

    // container args
    { IDI_C_TITLE_ID,       {"tid",         "titleid"},       ItemTypeInt,    0,              "title id"},
    { IDI_C_STREAM_ID,      {"sid",         "streamid"},      ItemTypeInt,    0,              "stream id"},
    { IDI_C_MPD_PROFILE,    {"mpd_profile", ""},              ItemTypeString, 0,              "MPD profile"},
    { IDS_C_DMF_PROFILE,    {"",           "dmf_profile"},    ItemTypeString, 0,              "dmf mux profile name"},
    { IDS_C_MP2_PCR_OFFSET, {"",           "clock_offset"},   ItemTypeInt64,  0,              "mp2 mux SCR/PCR offset in msec"},
    { IDS_C_MP2_VBR_MUX,    {"",           "vbr_multiplex"},  ItemTypeNoArg,  1,              "VBR multiplex" },

    // network args
    { IDS_NET_IP,           {"ip",       "ipaddress"},    ItemTypeString, 0,              "network IP address"},
    { IDI_NET_PORT,         {"port",     "portnumber"},   ItemTypeInt,    0,              "network port number"},
    { IDS_NET_URI,          {"uri",      "uri"},          ItemTypeString, 0,              "network URI"},

    // mode args
    { IDI_MODE,             {"m",        "mode"},         ItemTypeInt,    0,              "mode"},
    { IDI_GUI_MODE,         {"gui",      ""},             ItemTypeInt,    0,              "gui mode"},
    { IDI_NOMUX,            {"nm",       "nomux"},        ItemTypeNoArg,  1,              "don't mux encoder output"},

    // 3D
    { IDI_3D_NUM_VIEWS,     {"nv",       "numviews"},     ItemTypeInt,    0,              "number of views"},
    { IDI_3D_SPLIT_VIEWS,   {"",         ""},             ItemTypeNoArg,  0,              "do not split output views"},
    { IDI_3D_SPLIT_VIEWS,   {"split",    "splitviews"},   ItemTypeNoArg,  1,              "split output views"},
    { IDI_TRANSMISSION_FORMAT,   {"",    ""},             ItemTypeNoArg,  0,              "sequential video"},
    { IDI_TRANSMISSION_FORMAT,   {"side","sidebyside"},   ItemTypeNoArg,  1,              "side-by-side video"},
    { IDI_TRANSMISSION_FORMAT,   {"top", "topandbottom"}, ItemTypeNoArg,  2,              "top-and-bottom video"},

    //hardware acceleration
    { IDI_HWACC_MODE,       {"",         ""},             ItemTypeNoArg,  0,              "hardware acceleration type" },
    { IDI_HWACC_MODE,       {"dxva2",    ""},             ItemTypeNoArg,  2,              "use dxva2 hardware acceleration mode" },


    //decoders supported hardware acceleration
    { IDI_MC_HWACC_DECODER, {"",         ""},             ItemTypeNoArg,  0,              "decoder supported hardware acceleration type" },
    { IDI_MC_HWACC_DECODER, {"mpeg2",    ""},             ItemTypeNoArg,  1,              "use mpeg2 decoder" },
    { IDI_MC_HWACC_DECODER, {"vc1",      ""},             ItemTypeNoArg,  2,              "use vc1 decoder" },
    { IDI_MC_HWACC_DECODER, {"avc",      ""},             ItemTypeNoArg,  3,              "use avc decoder" },


    { IDN_LOOPPLAY,         {"loop",     ""},             ItemTypeNoArg,  1,              "loop play" },
    { IDI_SILENT_MODE,      {"silent",   ""},             ItemTypeNoArg,  1,              "silent mode" },
    { IDI_ERRCONCEAL_MODE,  {"err",      ""},             ItemTypeNoArg,  1,              "err conceal mode" },
    { IDS_BINDIR_PATH,      {"bd",       "bindir"},       ItemTypeString, 1,              "path to SDK dir" },
    { IDS_LIC_KEYGEN,       {"kf",       "keyfile"},      ItemTypeString, 1,              "keys file" },
    { IDS_LIC_FILE,         {"lf",       "licfile"},      ItemTypeString, 1,              "license file" },
    { IDS_LIC_DBGLEVEL,     {"dbg",      "debug"},        ItemTypeInt,    1,              "DBG level" },
    { IDN_LIC_MAKEALL,      {"makeall",  ""},             ItemTypeNoArg,  1,              "" },
    { IDN_LIC_KEYSIZE,      { "ks",  "keysize" },         ItemTypeInt,    1,              "Size of RSA keys" },

    // MainConcept DRM in DASH (MPD)
    { IDN_PROTECTION_ENABLE, {"protection-enable", "" }, ItemTypeNoArg, 1, "MainConcept DRM in DASH (MPD) : Enable Content Protection" },
    { IDS_PROTECTION_SCHEME, {"protection-scheme", "" }, ItemTypeString, 0, "MainConcept DRM in DASH (MPD) : Content Protection Scheme ID" },
    { IDS_PROTECTION_LICENSE, {"protection-license", "" }, ItemTypeString, 0, "MainConcept DRM in DASH (MPD) : Content Protection License URI" },
    { IDS_PROTECTION_SESSION, {"protection-session", "" }, ItemTypeString, 0, "MainConcept DRM in DASH (MPD) : Content Protection Session URI" },
    { IDS_PROTECTION_UPDATE, {"protection-update", "" }, ItemTypeString, 0, "MainConcept DRM in DASH (MPD) : Content Protection Update URI" },
    { IDS_PROTECTION_CONFIG, {"protection-config", "" }, ItemTypeString, 0, "MainConcept DRM in DASH (MPD) : Content Protection Config URI" },
    { IDS_PROTECTION_NAMESPACE, {"protection-namespace", ""}, ItemTypeString, 0, "MainConcept DRM in DASH (MPD) : Content Protection Namespace" },
    { IDS_PROTECTION_BASE_LOCATION, {"protection-baseloc", ""}, ItemTypeString, 0, "MainConcept DRM in DASH (MP4) : Content Protection Base Location URI" },
    { IDS_PROTECTION_PURCHASE_LOCATION, {"protection-purchaseloc", ""}, ItemTypeString, 0, "MainConcept DRM in DASH (MP4) : Content Protection Purchase Location URI" },
    { IDS_PROTECTION_IV, {"protection-iv", ""}, ItemTypeString, 0, "MainConcept DRM in DASH (MP4) : Content Protection Initialization Vector, 8 or 16 bytes" },
    { IDS_PROTECTION_KID, {"protection-kid", ""}, ItemTypeString, 0, "MainConcept DRM in DASH (MP4) : Content Protection Key ID, 16 bytes" },
    { IDS_PROTECTION_KEY, {"protection-key", ""}, ItemTypeString, 0, "MainConcept DRM in DASH (MP4) : Content Protection Key, 16 bytes" },

    { IDI_SINGLE_THREAD_MODE, {"single-thread", ""}, ItemTypeNoArg, 1, "MPEG-2 muxer : single-thread mode (no locks)" }
};

static const int32_t g_arg_items_cnt = sizeof(g_arg_items) / sizeof(g_arg_items[0]);


static int32_t invalidate_params(int32_t dstc, const arg_item_t * dst, const arg_item_desc_t *custom_args /* = NULL */, int32_t custom_args_cnt /* = 0 */)
{
    for (int32_t i = 0; i < dstc; i++)
    {
        const arg_item_desc_t * item_desc = NULL;

        // first of all search in custom the custom component-defined argument list if it's available
        if (custom_args != NULL)
        {
            for (int32_t d = 0; d < custom_args_cnt; d++)
            {
                if (dst[i].id == custom_args[d].id)
                {
                    item_desc = custom_args + d;
                    break;
                }
            }
        }

        if (item_desc == NULL)
        {
            for (int32_t d = 0; d < g_arg_items_cnt; d++)
            {
                if (dst[i].id == g_arg_items[d].id)
                {
                    item_desc = g_arg_items + d;
                    break;
                }
            }
        }

        if (item_desc == NULL)
        {
            printf("Error: application expects unknown argument (id = %d)\n", dst[i].id);
            return -1;
        }
        else
        {
            switch (item_desc->type)
            {
            case ItemTypeNoArg:
            case ItemTypeInt:
                *(int32_t *)dst[i].dst = ITEM_NOT_INIT;
                break;
            case ItemTypeInt64:
                *(int64_t *)dst[i].dst = ITEM_NOT_INIT;
                break;

            case ItemTypeDouble:
                *(double *)dst[i].dst = (double)ITEM_NOT_INIT;
                break;

            case ItemTypeString:
                *(char **)dst[i].dst = NULL;
                break;
            }
        }
    }

    return 0;
}


int32_t check_params(int32_t dstc, const arg_item_t * dst, const arg_item_desc_t *custom_args /* = NULL */, int32_t custom_args_cnt /* = 0 */)
{
    int32_t i, d = -1;
    int32_t uninit_params = 0;

    for (i = 0; i < dstc; i++)
    {
        if (dst[i].mandatory)
        {
            const arg_item_desc_t * item_desc = NULL;

            // first of all search in custom the custom component-defined argument list if it's available
            if (custom_args != NULL)
            {
                for (int32_t d = 0; d < custom_args_cnt; d++)
                {
                    if (dst[i].id == custom_args[d].id)
                    {
                        item_desc = custom_args + d;
                        break;
                    }
                }
            }

            if (item_desc == NULL)
            {
                for (d = 0; d < g_arg_items_cnt; d++)
                {
                    if (dst[i].id == g_arg_items[d].id)
                    {
                        item_desc = g_arg_items + d;
                        break;
                    }
                }
            }

            if (item_desc == NULL)
            {
                printf("Error: application expects unknown argument (id = %d)\n", dst[i].id);
                return -2;
            }
            else
            {
                int32_t err = 0;

                switch (item_desc->type)
                {
                case ItemTypeNoArg:
                case ItemTypeInt:
                    err = (*(int32_t *)dst[i].dst == ITEM_NOT_INIT);
                    break;

                case ItemTypeInt64:
                    err = (*(int64_t *)dst[i].dst == ITEM_NOT_INIT);
                    break;

                case ItemTypeDouble:
                    err = ((int32_t)*(double *)dst[i].dst == ITEM_NOT_INIT);
                    break;

                case ItemTypeString:
                    err = (*(char **)dst[i].dst == NULL);
                    break;
                }

                if (err)
                {
                    uninit_params++;

                    if (d != -1 && item_desc->type == ItemTypeNoArg)
                    {
                        uninit_params++;

                        printf("Error: %s not set. Available values:\n", item_desc->desc);

                        while (++d < g_arg_items_cnt && item_desc->id == g_arg_items[d].id)
                        {
                            printf("%s%s\t%s\n", ITEM_PREFIX, g_arg_items[d].name[0], g_arg_items[d].desc);
                        }

                        if (IDN_V_FOURCC == item_desc->id) {
                            for (int32_t i = 0; i < g_ColorSpaceBaseInfoMapSize; i++) {
                                printf("%s%s\t\n", ITEM_PREFIX, g_ColorSpaceBaseInfoMap[i].name);
                                if (g_ColorSpaceBaseInfoMap[i].alternative_name[0] != 0)
                                    printf("%s%s\t\n", ITEM_PREFIX, g_ColorSpaceBaseInfoMap[i].alternative_name);
                            }
                        }
                    }
                    else
                    {
                        printf("Error: %s not set (%s%s)\n", item_desc->desc, ITEM_PREFIX, item_desc->name[0]);
                    }
                }
            }
        }
    }

    return (uninit_params > 0) ? -2 : 0;
}

const char* get_type_name(item_type_e type)
{
    switch (type) {
        case ItemTypeInt:
            return "integer";
        case ItemTypeInt64:
            return "integer 64 bit";
        case ItemTypeDouble:
            return "double";
        default:
            assert(0);
            break;
    }

    return "";
}

int32_t parse_args(int32_t argc, char * argv[], int32_t dstc, const arg_item_t * dst, const arg_item_desc_t *custom_args /* = NULL */, int32_t custom_args_cnt /* = 0 */)
{
    int32_t i = 0;
    bool parser_status = true;

    if (invalidate_params(dstc, dst, custom_args, custom_args_cnt) < 0)
        return -1;

    for(int32_t arg_idx = 0; arg_idx < argc; arg_idx++) {
        if (strstr(argv[arg_idx], ITEM_PREFIX) != argv[arg_idx]) {
            printf("Wrong argument (prefix \"%s\" expected): %s\n", ITEM_PREFIX, argv[arg_idx]);
            continue;
        }

        const char * arg_name = argv[arg_idx] + strlen(ITEM_PREFIX);
        const arg_item_desc_t * item_desc = NULL;

        // first of all search in custom the custom component-defined argument list if it's available
        if (custom_args != NULL)
        {
            for (int32_t i = 0; i < custom_args_cnt; i++)
            {
                if (strcmp_lower_case(custom_args[i].name[0], arg_name) == 0 ||
                    strcmp_lower_case(custom_args[i].name[1], arg_name) == 0)
                {
                    item_desc = custom_args + i;
                    break;
                }
            }
        }

        if (item_desc == NULL)
        {
            for (i = 0; i < g_arg_items_cnt; i++)
            {
                if (strcmp_lower_case(g_arg_items[i].name[0], arg_name) == 0 ||
                    strcmp_lower_case(g_arg_items[i].name[1], arg_name) == 0)
                {
                    item_desc = g_arg_items + i;
                    break;
                }
            }
        }

        arg_item_desc_t fourcc_desc = { IDN_V_FOURCC, {"", ""}, ItemTypeNoArg,  0, {""} };

        if (item_desc == NULL) {
            for (i = 0; i < g_ColorSpaceBaseInfoMapSize; i++) {
                if (strcmp(reinterpret_cast<const char*>(g_ColorSpaceBaseInfoMap[i].name), arg_name) == 0 ||
                    strcmp(reinterpret_cast<const char*>(g_ColorSpaceBaseInfoMap[i].alternative_name), arg_name) == 0) 
                {
                    fourcc_desc.value = g_ColorSpaceBaseInfoMap[i].fourcc;
                    item_desc = &fourcc_desc;
                    break;
                }
            }
        }

        if (item_desc == NULL)
        {
            for (i = 0; i < dstc; i++)
            {
                if (dst[i].id == IDS_CUSTOM_ARG)
                    item_desc = g_arg_items + IDS_CUSTOM_ARG;
                // this allows to handle unknown arguments privately in the application
            }
        }

        if (item_desc == NULL)
        {
            printf("Unknown argument: %s\n", argv[arg_idx]);
            parser_status = false;
            continue;
        }

        const arg_item_t * item = NULL;
        int32_t excessive_arg = 0;

        for (i = 0; i < dstc; i++)
        {
            if (dst[i].id == item_desc->id)
            {
                int32_t empty = 1;

                switch (item_desc->type)
                {
                case ItemTypeNoArg:
                case ItemTypeInt:
                    empty = (*(int32_t *)dst[i].dst == ITEM_NOT_INIT);
                    break;

                case ItemTypeInt64:
                    empty = (*(int64_t *)dst[i].dst == ITEM_NOT_INIT);
                    break;

                case ItemTypeDouble:
                    empty = ((int32_t)*(double *)dst[i].dst == ITEM_NOT_INIT);
                    break;

                case ItemTypeString:
                    empty = (*(char **)dst[i].dst == NULL);
                    break;
                }

                if (empty)
                {
                    item = dst + i;
                    break;
                }
                else
                {
                    excessive_arg = 1;
                }
            }
        }

        if (item == NULL && !excessive_arg)
        {
            printf("Unexpected argument: %s (%s)\n", argv[arg_idx], item_desc->desc);
            parser_status = false;
            continue;
        }

        
        if (item != NULL)
        {
            try {
                switch (item_desc->type)
                {
                case ItemTypeNoArg:
                    *(int32_t *)item->dst = item_desc->value;
                    break;

                case ItemTypeInt:
                    if (++arg_idx < argc)
                    {
                        if (item->id == IDN_V_FOURCC) {
                            int32_t tmpfourcc = get_fourcc((const uint8_t*)argv[arg_idx]);
                            *reinterpret_cast<int32_t*>(item->dst) = tmpfourcc ? tmpfourcc : (!strcmp(argv[arg_idx], "0") ? 0 : ITEM_NOT_INIT);
                        }
                        else {
#if defined(__ANDROID__)
                            *reinterpret_cast<int32_t*>(item->dst) = atoi(argv[arg_idx]);
#else
                            *reinterpret_cast<int32_t*>(item->dst) = std::stoi(argv[arg_idx]);
#endif
                        }
                    }
                    break;

                case ItemTypeInt64:
                    if (++arg_idx < argc)
#if defined(__ANDROID__)
                        *reinterpret_cast<long long*>(item->dst) = atoll(argv[arg_idx]);
#else
                        *reinterpret_cast<long long*>(item->dst) = std::stoll(argv[arg_idx]);
#endif
                    break;

                case ItemTypeDouble:
                    if (++arg_idx < argc)
#if defined(__ANDROID__)
                        *reinterpret_cast<double*>(item->dst) = strtod(argv[arg_idx], NULL);
#else
                        *reinterpret_cast<double*>(item->dst) = std::stod(argv[arg_idx]);
#endif
                    break;

                case ItemTypeString:
                    if( item->id != IDS_CUSTOM_ARG )
                        ++arg_idx;
                    if (arg_idx < argc)
                        *(const char **)item->dst = argv[arg_idx];
                    break;
                }
            }
            catch (const std::invalid_argument&) {
                printf("Wrong argument: \"%s\" (%s) value: \"%s\", %s was expected.\n", argv[arg_idx - 1], item_desc->desc, argv[arg_idx],
                    get_type_name(item_desc->type));
                return -2;
            }
            catch (const std::out_of_range&) {
                printf("Wrong argument: \"%s\" (%s) value: \"%s\", value is out of range.\n", argv[arg_idx - 1], item_desc->desc, argv[arg_idx]);
                return -2;
            }

            if (arg_idx >= argc)
            {
                printf("Incomplete argument: \"%s\" (%s) requires a value\n", argv[argc - 1], item_desc->desc);
                return -2;
            }
        }
        else
        {
            switch (item_desc->type)
            {
            case ItemTypeNoArg:
                printf("Warning: excessive argument \"%s\" will be ignored\n", argv[arg_idx]);
                break;

            case ItemTypeInt:
            case ItemTypeInt64:
            case ItemTypeDouble:
            case ItemTypeString:
                if (++arg_idx >= argc)
                    printf("Warning: incomplete excessive argument \"%s\" (%s) will be ignored\n", argv[argc - 1], item_desc->desc);
                else
                    printf("Warning: excessive argument \"%s %s\" will be ignored\n", argv[arg_idx - 1], argv[arg_idx]);
                break;
            }
        }
    }

    return parser_status ? check_params(dstc, dst, custom_args, custom_args_cnt) : -1;
}

int32_t parse_args(int32_t argc, char* argv[], const std::vector<arg_item_t>& params, const std::vector<arg_item_desc_t>& custom_params)
{
    return parse_args(argc, argv, (uint32_t)params.size(), &params[0], custom_params.empty() ? nullptr : &custom_params[0], (uint32_t)custom_params.size());
}

int32_t parse_program_options(int32_t argc, char* argv[], const std::vector<arg_item_t>& params, const std::vector<arg_item_desc_t> &custom_params) {
    return parse_args(
        argc - 1, argv + 1, (uint32_t)params.size(), &params[0], custom_params.empty() ? nullptr : &custom_params[0], (uint32_t)custom_params.size());
}


void waitForAnyKey( const char* text )
{
#if !defined(__linux__) && !defined(__native_client__) && !defined(__APPLE__)
    printf_s( text ? text : "\nPress any key to continue ...\n" );
    while( 1 )
    {
        if( _kbhit() )
        {
            char ch = _getch();
            if ( ch == 0 || ch == -32 ) {
                _getch(); // read second part of functional keys
            }
            break;
        }
    }
#endif
}

int map_param_name_to_index(char *s, mapparam_t* param_map, size_t param_num) 
{
    size_t i = 0;
    while (i < param_num && param_map[i].name != NULL)
        if (0==strcmp(param_map[i].name, s))
            return (int)i;
        else
            i++;
    return -1;
}

#define MAX_PARAM_NUM 100
int parse_config_params(char *buf, int len, mapparam_t* param_map, size_t param_num) 
{
    size_t i;
    int  init_flag[MAX_PARAM_NUM*3];
    char *param[MAX_PARAM_NUM*3];
    size_t params = 0;
    char *bufend = &buf[len];
    char *p = buf;
    int inparam = 0;
    int inquotes = 0;
    
    memset(init_flag, 0, sizeof(init_flag));
    
    while (p < bufend)
    {
        switch (*p)
        {
        case '#':                 // Found comment
            if (inquotes) {       // Ignore # in quotes
                p++;
                break;
            }
            *p = '\0';              // Replace '#' with '\0' in case of comment immediately following integer or string
            while (*p != '\n' && p < bufend)  // Skip till EOL or EOF, whichever comes first
                p++;
            inparam = 0;
            break;
        case '\n':
        case '\r':
            inparam = 0;
            inquotes = 0;
            *p++='\0';
            break;
        case '"':               // Begin/End of String
            inquotes = ~inquotes;
            *p++ = '\0';
            break;
        case ' ':
        case '\t':              // Skip whitespace, leave state unchanged
            if (!inquotes)
            {
                *p++ = '\0';
                inparam = 0;
                break;
            }
        default:
            if (!inparam)
            {
                param[params++] = p;
                inparam = ~inparam;
            }
            p++;
        }
    }

    for( i = 0; (i+2)<params; i++ )
    {
        int index = map_param_name_to_index(param[i], param_map, param_num);
        if( index >= 0 ) 
        {
            if( param_map[index].type == ItemTypeString ) 
            {
                if( !strcmp((char* )param_map[index].name,"FourCC") ) {
                    int32_t stride, img_start;
                    const uint32_t fourcc = get_fourcc(reinterpret_cast<uint8_t*>(param[i + 2]));

                    if(!fourcc || !get_video_frame_size(128, 128, fourcc, &stride, &img_start)) {
                        printf("\nunrecognized fourcc of %s.\n", param[i+2]);
                        *(int* )param_map[index].place = (int)param_map[index].def_val;
                    }else
                        static_cast<uint32_t*>(param_map[index].place)[0] = fourcc;
                }else
                {
                    strncpy((char* )param_map[index].place,param[i+2], CONFIG_STRING_TYPE_MAX_LEN);
                }
            }
            else if( param_map[index].type == ItemTypeInt ||
                      param_map[index].type == ItemTypeInt64)
            {
                int v = atoi(param[i+2]);
                if( param_map[index].param_limit ) 
                {
                    if( v > param_map[index].max_limit ||  v < param_map[index].min_limit ) 
                    {
                        printf("\nout of range value of %s. set default %d", param[i], (int)param_map[index].def_val );
                        v = (int)param_map[index].def_val;
                    }
                }
                *(int* )param_map[index].place = v;
            } 
            else 
            { // double
                double v = atof(param[i+2]);
                if( param_map[index].param_limit ) 
                {
                    if( v > param_map[index].max_limit ||  v < param_map[index].min_limit ) 
                    {
                        printf("\nout of range value of %s. set default %f",param[i],param_map[index].def_val );
                        v = param_map[index].def_val;
                    }
                }
                *(double* )param_map[index].place = v;
            }
            init_flag[index] = 1;
            i+=2;
        }
        else 
        {
            printf("\nunknow parameter: %s",param[i]);
        }
    }

    // init unseted params
    for( i=0; i<param_num; i++ ) 
    {
        if( !init_flag[i] ) 
        {
            if( param_map[i].type == ItemTypeString ) 
            {
                *(char*)param_map[i].place = '\0';
            }
            else if (param_map[i].type == ItemTypeInt)
            {
                *(int* )param_map[i].place = (int)(param_map[i].def_val);
            }
            else if (param_map[i].type == ItemTypeInt64)
            {
                *(int64_t*)param_map[i].place = (int64_t)(param_map[i].def_val);
            }
            else  //double
            {
                *(double* )param_map[i].place = param_map[i].def_val;
            }
        }
    }
    return 1;
}

void print_arg_item(const arg_item_desc_t& item, const bool mandatory)
{
    char buf[255];
    if (item.name[1][0])
        sprintf(buf, "-%s, -%s", item.name[0], item.name[1]);
    else
        sprintf(buf, "-%s", item.name[0]);

    printf("\t%-40s  %s %s\n",
        buf, item.desc, mandatory ? "(mandatory)" : "");
}

void print_help(const arg_item_s *params, int32_t params_cnt,
    const arg_item_desc_t *custom_args /* = NULL */, int32_t custom_args_cnt /* = 0 */)
{
    for (int32_t i = 0; i < params_cnt; i++) {
        bool printed = false;

        for (int32_t j = 0; j < custom_args_cnt; j++)
            if (params[i].id == custom_args[j].id) {
                print_arg_item(custom_args[j], params[i].mandatory == 1);
                printed = true;
                break;
            }

        if (printed)
            continue;

        for (int32_t j = 0; j < g_arg_items_cnt; j++)
            if (params[i].id == g_arg_items[j].id && g_arg_items[j].name[0][0]) {
                print_arg_item(g_arg_items[j], params[i].mandatory == 1);
                break;
            }
    }
}

void print_help(const std::vector<arg_item_s> &params,
    const std::vector<arg_item_desc_t>& custom_params)
{
    print_help(params.data(), static_cast<int32_t>(params.size()), custom_params.data(),
        static_cast<int32_t>(custom_params.size()));
}
