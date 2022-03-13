/* ----------------------------------------------------------------------------
 * File: sample_mfimport_stream.cpp
 *
 * Desc: command-line sample for MFImport
 *
 * Copyright (c) 2014 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <cinttypes>

#include "mfimport.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

// comment this out to use the old mpegin API at the bottom of this file
bool USE_MFIMPORT_API = true; // use new mfimport API

typedef struct app_vars_s
{
    char *in_file;
    char *out_file;
    int32_t tid;
    int32_t sid;
    mfimport_tt *pMFI;
    mfimport_stream_tt *pStream;
    mfi_open_settings_t open_set;
    mfi_stream_info_t stream_info;
    mfi_stream_settings_t stream_set;
    mfi_seek_settings_t seek_info;
    mfi_sample_settings_t sample_set;
    mfi_sample_info_t sample_info;
    uint32_t fourcc;
} app_vars_t;

static int32_t prompt_index_mode_ex(context_t context)
{
    return 1;  // 0 = no, 1 = yes
}


typedef struct decoder_callback_settings_s
{
    int32_t smp_mode;

} decoder_callback_settings_t;


static int32_t video_decoder_callback(void *p_app_ptr, bufstream_tt *p_decoder_instance, uint8_t creation_flag)
{
    decoder_callback_settings_t *pSet = (decoder_callback_settings_t*)p_app_ptr;

    // example using the callback to set the SMP decoder mode option
    // could also set SET_CPU_NUM, SET_LOOP_FILTER_MODE, etc.
    p_decoder_instance->auxinfo(p_decoder_instance, pSet->smp_mode, SET_SMP_MODE, NULL, 0);

    return 0;
}


static int32_t display_file_info(app_vars_t *pVars)
{
    int32_t i, j;
    mfi_container_info_t container_info;

    // get the container info
    if (mfimportGetContainerInfo(pVars->pMFI, &container_info))
    {
        printf("Unable to get container information.\n");
        return 1;
    }

    // display the container info
    switch (container_info.container_type)
    {
    case mcmjtElementary:
        printf("File is an elementary stream\n");
        break;
    case mcmjtPVA:
        printf("File is a PVA container\n");
    case mcmjtMPEG1System:
        printf("File is a MPEG-1 system container\n");
        break;
    case mcmjtMPEG2Program:
        printf("File is a MPEG-2 program container\n");
        break;
    case mcmjtMPEG2Transport:
        printf("File is a MPEG-2 transport container, %d titles\n", container_info.num_titles);
        break;
    case mcmjtMXF:
        printf("File is a MXF container, %d titles\n", container_info.num_titles);
        break;
    case mcmjtMP4:
        printf("File is a MP4 container, %d titles\n", container_info.num_titles);
        break;
    case mcmjtAVI:
        printf("File is an AVI container, %d titles\n", container_info.num_titles);
        break;
    case mcmjtMKV:
        printf("File is a MKV container, %d titles\n", container_info.num_titles);
        break;
    case mcmjtASF:
        printf("File is an ASF container, %d titles\n", container_info.num_titles);
        break;
    }

    // display the title and stream info
    for (i = 0; i < container_info.num_titles; i++)
    {
        mfi_title_info_t title_info;

        // get the title info
        if (mfimportGetTitleInfo(pVars->pMFI, i, &title_info))
        {
            printf("Unable to get title %d information.\n", i);
            return 1;
        }

        printf("  Title %d, ID %d\n", i, title_info.title_id);

        for (j = 0; j < title_info.num_streams; j++)
        {
            mfi_stream_info_t stream_info;

            // get the stream info
            if (mfimportGetStreamInfo(pVars->pMFI, i, j, &stream_info))
            {
                printf("Unable to get title %d stream %d information.\n", i, j);
                return 1;
            }

            if (!(stream_info.flags & MFI_STREAM_INFO_IS_DECODABLE))
                printf("    Stream %d, ID %d, not decodable, \n", j, stream_info.stream_id);
            else
                printf("    Stream %d, ID %d,\n", j, stream_info.stream_id);

            switch (stream_info.format.stream_mediatype)
            {
            case mctMPEG1V:
                printf("    MPEG-1 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctMPEG2V:
                printf("    MPEG-2 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctMPEG4V:
                printf("    MPEG-4 part 2 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctH263:
                printf("    H.263 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctH264:
                printf("    AVC video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctMVCMux:
                printf("    MVC video, number of views %d, samples %" PRId64 "\n", stream_info.view_count, stream_info.total_samples);
                break;
            case mctHEVC:
                printf("    HEVC video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctProRes:
                printf("    ProRes video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctVC1:
                printf("    VC-1 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctVC3:
                printf("    VC-3 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctDV:
                printf("    DV video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctJ2K:
                printf("    JPEG 2000 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctMotionJPEG:
                printf("    Motion JPEG video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctDIV3:
                printf("    DIV3 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctDIV4:
                printf("    DIV4 video, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctMPEG1A:
                printf("    MPEG-1 audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctMPEG2A:
                printf("    MPEG-2 audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAAC_RAW:
                printf("    RAW AAC audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAAC_ADTS:
                printf("    ADTS AAC audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAAC_LATM:
                printf("    LATM AAC audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAC3:
                printf("    Dolby Digital audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAC3_DDPlus:
                printf("    Dolby Digital Plus audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAMR:
                printf("    AMR audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctWMA:
                printf("    WMA audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctALAW:
                printf("    ALAW audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctPCM:
                printf("    PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAIFF:
                printf("    AIFF audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctDVD_LPCM:
                printf("    DVD PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctHDMV_LPCM:
                printf("    HDMV PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAES3_302M:
                printf("    AES-3 302M PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAES3_382M:
                printf("    AES-3 382M PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctAES3_331M:
                printf("    AES-3 331M PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctTWOS_LPCM:
                printf("    TWOS PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctQT_PCM:
                printf("    QuickTime PCM audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctDTS:
                printf("    DTS audio, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            case mctDVD_Subtitles:
                printf("    DVD subtitle, samples %" PRId64 "\n", stream_info.total_samples);
                break;
            default:
                printf("    unknown media type\n");
            }
        }
    }

    return 0;
}

#define MAX_SUPPORTED_VIEWS    3    // this sample supports 3 views max, there can be up to 1024

#define IDC_NO_EDIT_LISTS IDC_CUSTOM_START_ID + 1
#define IDC_SHOW_PREROLL IDC_NO_EDIT_LISTS + 1

int32_t main_new_api(int32_t argc, char * argv[], char * pCodecPath, uint8_t index_es_flag)
{
    char str[2048];
    int32_t ret = 1;
    uint64_t sample_num;
    FILE *fp_out[MAX_SUPPORTED_VIEWS] = {0};
    uint8_t *sample_bfr[MAX_SUPPORTED_VIEWS] = {0};
    int32_t sample_bfr_size = 0;
    uint64_t prg_mod, prg_next;
    uint32_t num_views = 1;  // default is 1 view
    uint32_t idx;
    app_vars_t vars = {0};
    char* pSN = NULL;
    decoder_callback_settings_t callback_settings = {0};
    int32_t noelst = 0;
    int32_t show_preroll = 0;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE,       1, &vars.in_file},
        { IDI_C_TITLE_ID,       0, &vars.tid},
        { IDI_C_STREAM_ID,      0, &vars.sid},
        { IDS_OUTPUT_FILE,      0, &vars.out_file},
        { IDS_SERIAL_NUMBER,    0, &pSN},
        { IDN_V_SMP,            0, &callback_settings.smp_mode},
        { IDN_V_FOURCC,         0, &vars.fourcc},
        { IDS_LIC_FILE,         0, &license_file_path },
        { IDC_NO_EDIT_LISTS,    0, &noelst },
        { IDC_SHOW_PREROLL,     0, &show_preroll }
    };

    const arg_item_desc_t custom_args[] =
    {
        { IDC_NO_EDIT_LISTS, "ignore_edit_lists",  "", ItemTypeInt, 0, "Enable to ignore mp4 edit lists" },
        { IDC_SHOW_PREROLL, "show_preroll_frames", "", ItemTypeInt, 0, "Enable to show preroll frames" },
    };

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params, custom_args, sizeof(custom_args) / sizeof(custom_args[0])) < 0)
    {
        printf("\n==== MainConcept MFImport stream sample ====\n"
            "Usage:\n"
            "  sample_mfimport_stream.exe -i <filename> -tid <id> -sid <id> [-<fourcc>] -o <filename>[-codec_path = path_to_codecs]\n\n"
            "  -i <filename>           input filename\n"
            "  -tid <id>               title id\n"
            "  -sid <id>               stream id\n"
            "  -smp <mode>             SMP mode for video stream decoders (MPEG-2, H264, HEVC, ProRes)\n"
            "                               0 = Use decoders default mode,\n"
            "                               1 = Serial decoding,\n"
            "                               2 = Parallel decoding by pictures,\n"
            "                               3 = Parallel decoding by slices\n"
            "  -codec_path             path to demuxers and decoders\n"
            "  -<fourcc>                for example: -I420. By default: BGR4\n"
            "  -ignore_edit_lists      flag to disable MPEG-4 edit lists\n"
            "                               0 = do not ignore MPEG-4 edit lists (default)\n"
            "                               1 = ignore MPEG-4 edit lists\n"
            "  -show_preroll_frames    flag to enable decoding of pre-roll frames\n"
            "                               0 = do not show pre-roll frames (default)\n"
            "                               1 = show pre-roll frames\n"
            "  -o <filename>           output filename\n"
            "  -lf <file_name>         path to license file\n"
            "  -sn                     decoder serial number\n"            
            " You can use -old to switch to old mfimport API\n"
            );
        return 1;
    }

    callbacks_t callbacks;
    init_callbacks(callbacks);
    callbacks_mfimport_t mfimport_callbacks = {0};
    mfimport_callbacks.prompt_index_mode = prompt_index_mode_ex;
 
    if (ITEM_NOT_INIT == vars.fourcc)
    {
        vars.fourcc = FOURCC_BGR4;
    }

    // create an mfimport instance
    vars.pMFI = mfimportCreate(&callbacks, &mfimport_callbacks);
    if (!vars.pMFI)
    {
        printf("Unable to create an mfimport instance.\n");
        goto err_exit;
    }

    if (pCodecPath && strlen(pCodecPath))
    {
        // this sets the path of all the demuxer and decoders, see mfimportSetComponentPath
        // in mfimport.h for info on setting the path of individual demuxers or decoders
        if (mfimportSetComponentPath(vars.pMFI, mcmjtElementary, mctUnknown, pCodecPath))
        {
            printf("Unable to set codec path.\n");
            goto err_exit;
        }
    }

    // open the file
    memset(&vars.open_set, 0, sizeof(mfi_open_settings_t));
    vars.open_set.index_mode_required_action = 2;    // call the prompt_index_mode_ex if index mode is required

    if (noelst == 1)
        vars.open_set.flags |= MFI_OPEN_MP4_IGNORE_EDIT_LIST;	// do not ignore mp4 edit lists by default

    if (show_preroll != 1)
        vars.open_set.flags |= MFI_OPEN_IGNORE_PREROLL;		// ignore pre-roll frames by default

    if (index_es_flag)
        vars.open_set.flags = MFI_OPEN_ES_INDEX_MODE;   // open all elementary streams in index mode

    if (mfimportFileOpen(vars.pMFI, vars.in_file, &vars.open_set))
    {
        printf("Unable to open the input file.\n");
        goto err_exit;
    }

    if (argc <= 5)
    {
        // just display the stream info and exit
        ret = display_file_info(&vars);
        mfimportFree(vars.pMFI);
        return ret;
    }

    // get the stream info
    if (mfimportGetStreamInfo(vars.pMFI, vars.tid, vars.sid, &vars.stream_info))
    {
        printf("Unable to get the stream info.\n");
        goto err_exit;
    }

    // Not supported in versions of mc_dec_ddp 11.0 or higher
    vars.stream_set.p_serial_number = pSN;

    if (!(vars.stream_info.flags & MFI_STREAM_INFO_IS_DECODABLE))
        vars.stream_set.flags |= MFI_STREAM_SET_CODED_ONLY;
    else if (vars.stream_info.format.stream_mediatype == mctMVCMux)
    {
        vars.stream_set.flags |= MFI_STREAM_SET_ENABLE_SUBVIEWS;
        num_views = vars.stream_info.view_count;
        if (num_views > MAX_SUPPORTED_VIEWS)
            num_views = MAX_SUPPORTED_VIEWS;
    }

    vars.stream_set.title_idx = vars.tid;
    vars.stream_set.stream_idx = vars.sid;

    if (callback_settings.smp_mode >= 0)
    {
        // setup a video decoder callback for any custom setup needed (SMP, loop filtering, etc.)
        vars.stream_set.p_decoder_callback = video_decoder_callback;
        vars.stream_set.p_app_ptr = &callback_settings;  // passed back to the callback as the p_app_ptr parameter
    }

    vars.pStream = mfimportStreamNew(vars.pMFI, &vars.stream_set, NULL);
    if (!vars.pStream)
    {
        printf("mfimportStreamNew failed.\n");
        goto err_exit;
    }

    // open the output file(s)
    for (idx = 0; idx < num_views; idx++)
    {
        if (idx == 0)
        {
            if (vars.out_file == NULL)
            {
                printf("Output file was not specified.\n");
                goto err_exit;
            }

            fp_out[idx] = fopen(vars.out_file, "wb");
        }
        else
        {
            sprintf(str, "%s.%d", vars.out_file, idx + 1);
            fp_out[idx] = fopen(str, "wb");
        }

        if (!fp_out[idx])
        {
            printf("Unable to open output file %s.\n",vars.out_file);
            goto err_exit;
        }
    }

    if (!(vars.stream_info.flags & MFI_STREAM_INFO_IS_DECODABLE))
    {
        // seek to the first sample
        vars.seek_info.flags = MFI_SEEK_BY_TIME;
        vars.seek_info.sample_start = 0;

        if (mfimportStreamSeek(vars.pStream, &vars.seek_info, NULL, NULL))
        {
            printf("mfimportStreamSeek failed.\n");
            goto err_exit;
        }

        sample_num = 0;
        prg_next = 0;
        prg_mod = vars.stream_info.duration / 100;
        if (!prg_mod)
            prg_mod = 1;

        // loop through all the samples
        while (1)
        {
            // get the next sample
            vars.sample_set.num_samples = 1;

            if (mfimportStreamGetNext(vars.pStream, &vars.sample_set, &vars.sample_info))
                break;

            if (vars.sample_info.preroll == 0) {
                // write the samples out
                if (fwrite(vars.sample_info.p_coded_buffer, 1, vars.sample_info.coded_buffer_len, fp_out[0]) != vars.sample_info.coded_buffer_len)
                {
                    printf("Unable to write to output file.\n");
                    goto err_exit;
                }
            }

            sample_num++;
            if (sample_num >= prg_next)
            {
                printf("\rProcessed sample %u ...", (uint32_t)sample_num);
                fflush(stdout);
                prg_next += prg_mod;
            }
        }

    }
    else if ((vars.stream_info.format.stream_mediatype >= mctMinVideoType) &&
            (vars.stream_info.format.stream_mediatype <= mctMaxVideoType))
    {
        frame_tt frame[MAX_SUPPORTED_VIEWS] = {0};
        mc_video_format_t *pVFormat = (mc_video_format_t*)vars.stream_info.format.pFormat;

        for (idx = 0; idx < num_views; idx++)
        {
            // create a buffer for the video frames
            // Use a helper function from mccolorspace.h to help calculate the buffer size
            frame_colorspace_info_tt cs_info = { 0 };
            get_frame_colorspace_info(&cs_info, pVFormat->width, pVFormat->height, vars.fourcc, 0);

            sample_bfr_size = cs_info.frame_size;
            sample_bfr[idx] = (uint8_t*)malloc(sample_bfr_size);
            if (!sample_bfr[idx])
            {
                printf("Unable to allocate memory for a sample buffer.\n");
                goto err_exit;
            }

            // Use a helper function mccolorspace.h to update the frame_tt struct with the sample buffer
            fill_frame_from_colorspace_info(&cs_info, sample_bfr[idx], &frame[idx]);
        }

        // seek to the first frame
        vars.seek_info.flags = MFI_SEEK_BY_SAMPLE_NUMBER;
        vars.seek_info.sample_start = 0;    // first frame

        vars.sample_set.p_frame = &frame[0];
        vars.sample_set.num_samples = 1;

        if (num_views > 1)
        {
            vars.sample_set.subview_count = num_views - 1;
            vars.sample_set.p_subviews = &frame[1];
        }

        if (mfimportStreamSeek(vars.pStream, &vars.seek_info, &vars.sample_set, &vars.sample_info))
        {
            printf("mfimportStreamSeek failed.\n");
            goto err_exit;
        }

        // write the frame and subviews out
        for (idx = 0; idx < num_views; idx++)
        {
            if (fwrite(sample_bfr[idx], 1, sample_bfr_size, fp_out[idx]) != sample_bfr_size)
            {
                printf("Unable to write to output file.\n");
                goto err_exit;
            }
        }

        sample_num = 1;
        prg_next = 0;
        prg_mod = vars.stream_info.total_samples / 100;
        if (!prg_mod)
            prg_mod = 1;

        // loop until EOS returned
        while (true)
        {
            // get the next frame
            ret = mfimportStreamGetNext(vars.pStream, &vars.sample_set, &vars.sample_info);
            if(ret == mfiErrorEndOfStream)
            {
                break;
            }
            else if (ret != mfiNoError)
            {
                printf("mfimportStreamGetNext failed.\n");
                goto err_exit;
            }

            // write the frame and subviews out
            for (idx = 0; idx < num_views; idx++)
            {
                if (fwrite(sample_bfr[idx], 1, sample_bfr_size, fp_out[idx]) != sample_bfr_size)
                {
                    printf("Unable to write to output file.\n");
                    goto err_exit;
                }
            }

            sample_num++;
            if (sample_num >= prg_next)
            {
                printf("\rProcessed %u%% ...", (uint32_t)((double)sample_num / (double)vars.stream_info.total_samples * 100.0));
                fflush(stdout);
                prg_next += prg_mod;
                if (prg_next >= vars.stream_info.total_samples)
                    prg_next = vars.stream_info.total_samples - 1;
            }
        }
    }
    else if ((vars.stream_info.format.stream_mediatype >= mctMinAudioType) &&
            (vars.stream_info.format.stream_mediatype <= mctMaxPCMType))
    {
        int32_t samples_returned;
        uint8_t bytes_per_sample;
        mc_audio_format_t *pAFormat = (mc_audio_format_t*)vars.stream_info.format.pFormat;

        // make a buffer for 1s of audio
        bytes_per_sample = pAFormat->bits_per_sample >> 3;
        if (pAFormat->bits_per_sample % 8)
            bytes_per_sample++;

        bytes_per_sample *= pAFormat->channels;
            
        sample_bfr_size = bytes_per_sample * pAFormat->samples_per_sec;
        sample_bfr[0] = (uint8_t*)malloc(sample_bfr_size);
        if (!sample_bfr[0])
        {
            printf("Unable to allocate memory for a sample buffer.\n");
            goto err_exit;
        }

        // seek to the first sample without getting any samples
        vars.seek_info.flags = MFI_SEEK_BY_SAMPLE_NUMBER;
        vars.seek_info.sample_start = 0;    // first frame

        if (mfimportStreamSeek(vars.pStream, &vars.seek_info, NULL, NULL))
        {
            printf("mfimportStreamSeek failed.\n");
            goto err_exit;
        }

        sample_num = 0;
        prg_next = 0;
        prg_mod = vars.stream_info.total_samples / 100;
        if (!prg_mod)
            prg_mod = 1;

        // loop until EOS returned
        while (true)
        {
            int32_t samples_to_get = pAFormat->samples_per_sec;
            if (sample_num + samples_to_get >= vars.stream_info.total_samples)
                 samples_to_get = (int32_t)(vars.stream_info.total_samples - sample_num);

            // get the next block of samples
            vars.sample_set.p_buffer = sample_bfr[0];
            vars.sample_set.num_samples = samples_to_get;

            if(vars.sample_set.num_samples <= 0)
                vars.sample_set.num_samples = 1;

            ret = mfimportStreamGetNext(vars.pStream, &vars.sample_set, &vars.sample_info);
            if (ret == mfiErrorEndOfStream) {
                break;
            }
            else if (ret != mfiNoError)
            {
                printf("mfimportStreamGetNext failed.\n");
                goto err_exit;
            }

            samples_returned = vars.sample_info.num_samples_returned;

            // write the samples out
            if (fwrite(sample_bfr[0], 1, samples_returned * bytes_per_sample, fp_out[0]) != samples_returned * bytes_per_sample)
            {
                printf("Unable to write to output file.\n");
                goto err_exit;
            }

            sample_num += samples_returned;
            if (sample_num >= prg_next)
            {
                printf("\rProcessed %u%% ...", (uint32_t)((double)sample_num / (double)vars.stream_info.total_samples * 100.0));
                fflush(stdout);
                prg_next += prg_mod;
                if (prg_next >= vars.stream_info.total_samples)
                    prg_next = vars.stream_info.total_samples - 1;
            }
        }
    }

    printf("\rProcessed 100%%.\n");

    ret = 0;

err_exit:

    printf("\n");
    // close everything
    if (vars.pStream)
        mfimportStreamFree(vars.pStream);

    if (vars.pMFI)
        mfimportFree(vars.pMFI);

    for (idx = 0; idx < MAX_SUPPORTED_VIEWS; idx++)
    {
        if (fp_out[idx])
            fclose(fp_out[idx]);

        if (sample_bfr[idx])
            free(sample_bfr[idx]);
    }

    return ret;
}

static void display_stream_info(int32_t stream_num,
                                int32_t program_stream_flag,
                                mpegin_stream_data *stream_info,
                                const char *title_str)
{
    switch (program_stream_flag)
    {
    case STREAM_ELEMENTARY:
        printf("  %d: %s\n", stream_num, title_str );
        break;
    case STREAM_PVA:
    case STREAM_MPEG1_PROGRAM:
        printf("  %d: %s, ID = 0x%02X (%d)\n",
                stream_num,
                title_str,
                stream_info->id,
                stream_info->id);
        break;
    case STREAM_MPEG2_PROGRAM:
        if ((stream_info->id == 0xBD) && stream_info->sub_id)
            printf("  %d: %s, ID = 0x%02X (%d), subID = 0x%02X (%d)\n",
                stream_num,
                title_str,
                stream_info->id,
                stream_info->id,
                stream_info->sub_id,
                stream_info->sub_id);
        else
            printf("  %d: %s, ID = 0x%02X (%d)\n",
                stream_num,
                title_str,
                stream_info->id,
                stream_info->id);
        break;
    case STREAM_MPEG2_TRANSPORT:
        printf("  %d: %s, PID = 0x%04X (%d), program num %d\n",
            stream_num,
            title_str,
            stream_info->pid,
            stream_info->pid,
            stream_info->program_number);
        break;

    case STREAM_MXF_XDCAM_IMX:
    case STREAM_MXF_XDCAM_HD:
    case STREAM_MXF_XDCAM_DV:
    case STREAM_MXF_DV:
    case STREAM_MXF_JPEG2000:
    case STREAM_MXF_P2_DVCPRO:
    case STREAM_MXF_P2_AVCI:
    case STREAM_MXF_GENERIC:
    case STREAM_MP4:
    case STREAM_QT:
        printf("  %d: %s, ID = 0x%02X (%d)\n",
            stream_num,
            title_str,
            stream_info->pid,
            stream_info->pid);
        break;

    case STREAM_DMF:
        printf("  %d: %s, ID = 0x%04X (%d), title num %d\n",
            stream_num,
            title_str,
            stream_info->pid,
            stream_info->pid,
            stream_info->program_number);
        break;

    case STREAM_ASF:
        printf("  %d: %s, ID = 0x%04X (%d), title num %d\n",
            stream_num,
            title_str,
            stream_info->pid,
            stream_info->pid,
            stream_info->program_number);
        break;
    }
}


static void display_file_info(mpegInInfo *pMFI)
{
    int32_t i, j;

    i = pMFI->video_stream_count +
        pMFI->audio_stream_count +
        pMFI->pcm_stream_count +
        pMFI->ac3_stream_count +
        pMFI->aes3_stream_count +
        pMFI->amr_stream_count +
        pMFI->wma_stream_count +
        pMFI->subpic_stream_count +
        pMFI->unknown_video_stream_count +
        pMFI->unknown_audio_stream_count;

    switch (pMFI->program_stream_flag)
    {
    case STREAM_ELEMENTARY:
        printf("File is an elementary stream \n\n");
        break;
    case STREAM_PVA:
        printf("File is a PVA container, total streams %u\n\n", i);
        break;
    case STREAM_MPEG1_PROGRAM:
        printf("File is a MPEG-1 system container, total streams %u\n\n", i);
        break;
    case STREAM_MPEG2_PROGRAM:
        printf("File is a MPEG-2 program container, total streams %u\n\n", i);
        break;
    case STREAM_MPEG2_TRANSPORT:
        printf("File is a MPEG-2 transport container, total streams %u\n\n", i);
        break;
    case STREAM_MXF_XDCAM_IMX:
        printf("File is a MXF XDCAM IMX container, total streams %u\n\n", i);
        break;
    case STREAM_MXF_XDCAM_HD:
    case STREAM_MXF_XDCAM_DV:
        printf("File is a MXF XDCAM HD container, total streams %u\n\n", i);
        break;
    case STREAM_MXF_DV:
    case STREAM_MXF_JPEG2000:
    case STREAM_MXF_P2_DVCPRO:
    case STREAM_MXF_P2_AVCI:
        printf("File is a MXF container, total streams %u\n\n", i);
        break;
    case STREAM_MP4:
    case STREAM_QT:
        printf("File is a MP4 container, total streams %u\n\n", i);
        break;
    case STREAM_DMF:
        printf("File is a DMF container, total streams %u\n\n", i);
        break;
    case STREAM_ASF:
        printf("File is a ASF container, total streams %u\n\n", i);
        break;
    }

    if (pMFI->unknown_video_stream_count > 0)
        printf("  Unknown video streams present: %d\n\n", pMFI->unknown_video_stream_count);

    if (pMFI->unknown_audio_stream_count > 0)
        printf("  Unknown audio streams present: %d\n\n", pMFI->unknown_audio_stream_count);

    j = 0;
    for (i = 0; i < pMFI->video_stream_count; i++)
    {
        if (i == 0)    // these flags only apply to the first video stream after a mpegInGetInfo
        {
            if (pMFI->MPEG2_video)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "MPEG-2 video stream");
            else if (pMFI->h264_video_flag)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "AVC video stream");
            else if (pMFI->hevc_video_flag)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "HEVC video stream");
            else if (pMFI->prores_video_flag)
                display_stream_info(j++,
                    pMFI->program_stream_flag,
                    &pMFI->video_streams[i],
                    "ProRes video stream");
            else if (pMFI->mpeg4_video_flag)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "MPEG-4 video stream");
            else if (pMFI->jpeg2k_video_flag)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "JPEG2000 video stream");
            else if (pMFI->dv_video_flag)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "DV video stream");
            else if (pMFI->vc1_video_flag)
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "VC1 video stream");
            else
                display_stream_info(j++,
                                    pMFI->program_stream_flag,
                                    &pMFI->video_streams[i],
                                    "MPEG-1 video stream");
        }
        else
           display_stream_info(j++,
                               pMFI->program_stream_flag,
                               &pMFI->video_streams[i],
                               "Video stream");
    }

    for (i = 0; i < pMFI->audio_stream_count; i++)
    {
        display_stream_info(j++,
                            pMFI->program_stream_flag,
                            &pMFI->audio_streams[i],
                            "MPEG/AAC audio stream");
    }

    for (i = 0; i < pMFI->pcm_stream_count; i++)
    {
        display_stream_info(j++,
                            pMFI->program_stream_flag,
                            &pMFI->pcm_streams[i],
                            "DVD LPCM audio stream");
    }

    for (i = 0; i < pMFI->ac3_stream_count; i++)
    {
        display_stream_info(j++,
                            pMFI->program_stream_flag,
                            &pMFI->ac3_streams[i],
                            "Dolby Digital audio stream");
    }

    for (i = 0; i < pMFI->dts_stream_count; i++)
    {
        display_stream_info(j++,
            pMFI->program_stream_flag,
            &pMFI->dts_streams[i],
            "DTS audio stream");
    }

    for (i = 0; i < pMFI->aes3_stream_count; i++)
    {
        display_stream_info(j++,
                            pMFI->program_stream_flag,
                            &pMFI->aes3_streams[i],
                            "AES3 PCM audio stream");
    }

    for (i = 0; i < pMFI->amr_stream_count; i++)
    {    
        display_stream_info(j++,
                            pMFI->program_stream_flag,
                            &pMFI->amr_streams[i],
                            "AMR audio stream");
    }

    for (i = 0; i < pMFI->wma_stream_count; i++)
    {
        display_stream_info(j++,
                            pMFI->program_stream_flag,
                            &pMFI->wma_streams[i],
                            "WMA audio stream");
    }

    for (i = 0; i < pMFI->subpic_stream_count; i++)
    {
        switch (pMFI->subpic_streams[i].stream_type)
        {
        case mctDVD_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "DVD subpic stream");
            break;
        case mctDXSB_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "XSUB subpic stream");
            break;
        case mctUTF8_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "UTF8 subpic stream");
            break;
        case mctSSA_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "SSA subpic stream");
            break;
        case mctASS_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "ASS subpic stream");
            break;
        case mctUSF_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "USF subpic stream");
            break;
        case mctBMP_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "BMP subpic stream");
            break;
        case mctVSUB_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "VSUB subpic stream");
            break;
        default:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "Subpic stream");
            break;
        }
    }

    // display selected stream info
    if( pMFI->videoAvail )
    {
        printf("  Selected video stream:\n");

        printf("      Stream info:\n");
        printf("        %dx%d ", pMFI->horizontal_size, pMFI->vertical_size);
        if( pMFI->hevc_video_flag || pMFI->h264_video_flag || pMFI->mpeg4_video_flag || pMFI->dv_video_flag || pMFI->jpeg2k_video_flag || pMFI->prores_video_flag)
            printf("(par %d:%d) ", pMFI->aspect_ratio_width, pMFI->aspect_ratio_height);
        else
            printf("(par %d) ", pMFI->aspect_ratio_information);  // mainly for mpeg-1 or mpeg-2 video
        printf("at %2.2f\n", pMFI->actual_frame_rate);

        printf("      ES info:\n");
        printf("        at %2.2f\n", pMFI->stream_frame_rate);

        printf("      Container info:\n");
        printf("        %dx%d (par %d:%d) at %2.2f\n", pMFI->container_width, pMFI->container_height, pMFI->container_aspect_x, pMFI->container_aspect_y, pMFI->container_frame_rate);
    }

    if( pMFI->audioAvail )
    {
        printf("  Selected audio stream:\n");

        printf("      Stream info:\n");
        printf("        %d channels with %d bits per channel and %d samples per sec\n", pMFI->channels, pMFI->sampleSize, pMFI->audioRate);
        printf("        %d bytes per second\n", pMFI->audioBitrate >> 3);
    }

    printf("\n");
}

int32_t main_old_api(int32_t argc, char * argv[], char * pCodecPath, uint8_t index_es_flag)
{
    int32_t ret = 1;
    int64_t sample_num;
    int32_t sid = 0;
    uint8_t is_video = 0;
    uint8_t is_subpic = 0;
    char *in_file = NULL;
    char *out_file = NULL;
    FILE *fp_out = NULL;
    mpegInInfo *pMFI = NULL;
    uint8_t *sample_bfr = NULL;
    int32_t sample_bfr_size = 0;
    int64_t prg_mod, prg_next;
    int32_t audio_stream_base;
    int32_t pcm_stream_base;
    int32_t ac3_stream_base;
    int32_t dts_stream_base;
    int32_t aes3_stream_base;
    int32_t amr_stream_base;
    int32_t wma_stream_base;
    int32_t subpic_stream_base;
    char* pSN = NULL;
    uint32_t fourcc = 0;
    decoder_callback_settings_t callback_settings = {0};
    int32_t noelst = 0;
    int32_t show_preroll = 0;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE,       1, &in_file},
        { IDI_C_STREAM_ID,      0, &sid},
        { IDS_OUTPUT_FILE,      0, &out_file},
        { IDS_SERIAL_NUMBER,    0, &pSN},
        { IDN_V_SMP,            0, &callback_settings.smp_mode},
        { IDS_LIC_FILE,         0, &license_file_path },
        { IDN_V_FOURCC,         0, &fourcc},
        { IDC_NO_EDIT_LISTS,    0, &noelst },
        { IDC_SHOW_PREROLL,     0, &show_preroll }
    };

    const arg_item_desc_t custom_args[] =
    {
        { IDC_NO_EDIT_LISTS, "ignore_edit_lists",  "", ItemTypeInt, 0, "Enable to ignore mp4 edit lists" },
        { IDC_SHOW_PREROLL, "show_preroll_frames", "", ItemTypeInt, 0, "Enable to show preroll frames" },
    };

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params, custom_args, sizeof(custom_args) / sizeof(custom_args[0])) < 0)
    {
        printf("\n==== MainConcept MFImport stream sample ====\n"
            "Usage:\n"
            "  sample_mfimport_stream.exe -i <filename> -old -sid <id> -o <filename> [-<fourcc>] [-codec_path = path_to_codecs]\n\n"
            "  -i <filename>           input filename\n"
            "  -old                    use old mfimport API\n"
            "  -sid <id>               stream id\n"
            "  -smp <mode>             SMP mode for video stream decoders (MPEG-2, H264, HEVC, ProRes)\n"
            "                               0 = Use decoders default mode,\n"
            "                               1 = Serial decoding,\n"
            "                               2 = Parallel decoding by pictures,\n"
            "                               3 = Parallel decoding by slices\n"
            "  -codec_path             path to demuxers and decoders\n"
            "  -<fourcc>                for example: -I420. By default: BGR4\n"
            "  -ignore_edit_lists      flag to disable MPEG-4 edit lists\n"
            "                               0 = do not ignore MPEG-4 edit lists (default)\n"
            "                               1 = ignore MPEG-4 edit lists\n"
            "  -show_preroll_frames    flag to enable decoding of pre-roll frames\n"
            "                               0 = do not show pre-roll frames (default)\n"
            "                               1 = show pre-roll frames\n"
            "  -o <filename>           output filename\n"
            "  -lf <file_name>         path to license file\n"
            "  -sn                     decoder serial number\n"            
            );
        return 1;
    }

    if (ITEM_NOT_INIT == fourcc)
    {
        fourcc = FOURCC_BGR4;
    }
    callbacks_t callbacks;
    init_callbacks(callbacks);
    // create an mfimport instance
    pMFI = mpegInCreate(&callbacks, NULL);
    if (!pMFI)
    {
        printf("Unable to create an mfimport instance.\n");
        goto err_exit;
    }

    if (pCodecPath && strlen(pCodecPath))
    {
        // this sets the path of all the demuxer and decoders, see mpegInSetComponentPath
        // in mfimport.h for info on setting the path of individual demuxers or decoders
        if (mpegInSetComponentPath(pMFI, mcmjtElementary, mctUnknown, pCodecPath))
        {
            printf("Unable to set codec path.\n");
            goto err_exit;
        }
    }

    // set this for the mpegInInfo function
    pMFI->inputFilename = in_file;
    pMFI->FastAccess = 1;    // set to 0 for index mode
    if (index_es_flag)
        pMFI->es_index_mode_flag = 1;   // open all elementary streams in index mode

    // get the number of streams in the file
    
    if (mpegInGetInfo(pMFI))
    {
        printf("Unable to get file info.\n");
        goto err_exit;
    }

    if (argc <= 5)
    {
        // just display the stream info and exit
        display_file_info(pMFI);
        mpegInFree(pMFI);
        return 0;
    }

    // open the output file
    if (out_file == NULL)
    {
        printf("Output file was not specified.\n");
        goto err_exit;
    }

    fp_out = fopen(out_file, "wb");
    if (!fp_out)
    {
        printf("Unable to open output file.\n");
        goto err_exit;
    }

    // set the proper 'stream_to_get'
    pMFI->video_stream_to_get = 0;
    pMFI->audio_stream_to_get = 0;
    pMFI->pcm_stream_to_get = 0;
    pMFI->ac3_stream_to_get = 0;
    pMFI->aes3_stream_to_get = 0;
    pMFI->dts_stream_to_get = 0;
    pMFI->amr_stream_to_get = 0;
    pMFI->wma_stream_to_get = 0;
    pMFI->subpic_stream_to_get = 0;
    pMFI->ignore_preroll_frames = 1;

    audio_stream_base = pMFI->video_stream_count;
    pcm_stream_base = audio_stream_base + pMFI->audio_stream_count;
    ac3_stream_base = pcm_stream_base + pMFI->pcm_stream_count;
    dts_stream_base = ac3_stream_base + pMFI->ac3_stream_count;
    aes3_stream_base = dts_stream_base + pMFI->dts_stream_count;
    amr_stream_base = aes3_stream_base + pMFI->aes3_stream_count;
    wma_stream_base = amr_stream_base + pMFI->amr_stream_count;
    subpic_stream_base = wma_stream_base + pMFI->wma_stream_count;

    if (sid < audio_stream_base)
    {
        pMFI->video_stream_to_get = sid + 1;
        pMFI->use_external_frame_buffer = 1;
        // disable audio
        int32_t i = ~(AUDIO_PCM);
        pMFI->outputTypes &= i;
        is_video = 1;
    }
    else if (sid < subpic_stream_base)
    {
        // disable video
        pMFI->fourCC = 0;
        pMFI->outputTypes = AUDIO_PCM;
        pMFI->use_external_frame_buffer = 0;

        if (sid < pcm_stream_base)
            pMFI->audio_stream_to_get = sid - audio_stream_base + 1;
        else if (sid < ac3_stream_base)
            pMFI->pcm_stream_to_get = sid - pcm_stream_base + 1;
        else if (sid < dts_stream_base)
            pMFI->ac3_stream_to_get = sid - ac3_stream_base + 1;
        else if (sid < aes3_stream_base)
            pMFI->dts_stream_to_get = sid - dts_stream_base + 1;
        else if (sid < amr_stream_base)
            pMFI->aes3_stream_to_get = sid - aes3_stream_base + 1;
        else if (sid < wma_stream_base)
            pMFI->amr_stream_to_get = sid - amr_stream_base + 1;
        else
            pMFI->wma_stream_to_get = sid - wma_stream_base + 1;
    }
    else
    {
        pMFI->subpic_stream_to_get = sid + 1;

        // need to say we are getting a video stream
        pMFI->video_stream_to_get = 1;
        pMFI->use_external_frame_buffer = 1;

        // disable audio
        int32_t i = ~(AUDIO_PCM);
        pMFI->outputTypes &= i;
        is_subpic = 1;
    }
    
    // Unused since mc_ddp_dec version 11.0. Please, use license file activation.
    pMFI->ac3_serial_number = pSN;

    if (callback_settings.smp_mode >= 0)
    {
        // setup a video decoder callback for any custom setup needed (SMP, loop filtering, etc.)
        pMFI->videoDecoderCallback = video_decoder_callback;
        pMFI->appData = &callback_settings;   // passed back to the callback as the p_app_ptr parameter
    }

    if (noelst == 1)
        pMFI->ignore_edit_list = 1;     // do not ignore mp4 edit lists by default

    if (show_preroll != 1)
        pMFI->ignore_preroll_frames = 1;     // ignore pre-roll frames by default

    pMFI->fourCC = fourcc;

    // now open the file
    if (mpegInOpen(pMFI, in_file))
    {
        printf("Unable to open the input file.\n");
        goto err_exit;
    }

    if (is_video)
    {
        // Create a buffer for the video frames
        frame_tt frame;
        memset(&frame, 0, sizeof(frame_tt));

        // Use a helper function from mccolorspace.h to help calculate the buffer size
        frame_colorspace_info_tt cs_info = { 0 };
        get_frame_colorspace_info(&cs_info, pMFI->horizontal_size, pMFI->vertical_size, fourcc, 0);

        sample_bfr_size = cs_info.frame_size;
        sample_bfr = (uint8_t*)malloc(sample_bfr_size);
        if (!sample_bfr)
        {
            printf("Unable to allocate memory for a sample buffer.\n");
            goto err_exit;
        }

        // Use a helper function mccolorspace.h to update the frame_tt struct with the sample buffer
        fill_frame_from_colorspace_info(&cs_info, sample_bfr, &frame);
        pMFI->frame_info = &frame;

        // seek to the first frame
        if (mpegInSeekFrame(pMFI, 0))
        {
            printf("mpegInSeekFrame failed.\n");
            goto err_exit;
        }

        // write the frame out
        if (fwrite(sample_bfr, 1, sample_bfr_size, fp_out) != sample_bfr_size)
        {
            printf("Unable to write to output file.\n");
            goto err_exit;
        }

        sample_num = 1;
        prg_next = 0;
        prg_mod = pMFI->totalFrames / 100;
        if (!prg_mod)
            prg_mod = 1;

        // loop through the remaining frames
        while (sample_num < pMFI->totalFrames)
        {
            // get the next frame
            if (mpegInGetNextFrame(pMFI))
            {
                printf("mpegInGetNextFrame failed.\n");
                goto err_exit;
            }

            // write the frame out
            if (fwrite(sample_bfr, 1, sample_bfr_size, fp_out) != sample_bfr_size)
            {
                printf("Unable to write to output file.\n");
                goto err_exit;
            }

            sample_num++;
            if (sample_num >= prg_next)
            {
                printf("\rProcessed %u%% ...", (uint32_t)((double)sample_num / (double)pMFI->totalFrames * 100.0));
                fflush(stdout);
                prg_next += prg_mod;
                if (prg_next >= pMFI->totalFrames)
                    prg_next = pMFI->totalFrames - 1;
            }
        }
    }
    else if (is_subpic)
    {
        mpegin_subpic_info_struct subpic_info;

        // get all the raw subpic data
        sample_num = 0;
        int32_t done = 0;
        uint8_t *pBfr = NULL;
        int32_t bfr_size = 0;
        int32_t ret_val;

        while (done == 0)
        {
            subpic_info.buffer = NULL;  // get the needed size
            ret_val = mpegInGetNextSubpicChunk(pMFI, &subpic_info);
            if (ret_val != 0)
            {
                if (pBfr)
                    free(pBfr);
                goto err_exit;
            }

            if (subpic_info.buffer_size <= 0)
            {
                done = 1;
            }
            else
            {
                if (subpic_info.buffer_size > bfr_size)
                {
                    if (pBfr)
                        free(pBfr);

                    pBfr = (uint8_t*)malloc(subpic_info.buffer_size);
                    if (!pBfr)
                    {
                        printf("Unable to allocate %d bytes for an output buffer.\n", subpic_info.buffer_size);
                        goto err_exit;
                    }
                    bfr_size = subpic_info.buffer_size;
                }

                subpic_info.buffer = pBfr;
                ret_val = mpegInGetNextSubpicChunk(pMFI, &subpic_info);
                if (ret_val != 0)
                {
                    if (pBfr)
                        free(pBfr);
                    goto err_exit;
                }

                // write the subpic out
                if (fwrite(subpic_info.buffer, 1, subpic_info.buffer_size, fp_out) != subpic_info.buffer_size)
                {
                    printf("Unable to write to output file.\n");
                    if (pBfr)
                        free(pBfr);
                    goto err_exit;
                }

                printf("\rProcessed subpic %d, PTS %ums ...", (uint32_t)sample_num, (uint32_t)(subpic_info.PTS / 27000.0));
                sample_num++;
            }
        }

        if (pBfr)
            free(pBfr);
    }
    else  // audio
    {
        int32_t samples_returned;

        // make a buffer for 1s of audio
        sample_bfr_size = pMFI->bytesPerSample * pMFI->audioRate;
        sample_bfr = (uint8_t*)malloc(sample_bfr_size);
        if (!sample_bfr)
        {
            printf("Unable to allocate memory for a sample buffer.\n");
            goto err_exit;
        }

        // seek to the first sample without getting any samples
        if (mpegInSeekSample64(pMFI, NULL, 0, 0))
        {
            printf("mpegInSeekSample64 failed.\n");
            goto err_exit;
        }

        sample_num = 0;
        prg_next = 0;
        prg_mod = pMFI->totalSamples64 / 100;
        if (!prg_mod)
            prg_mod = 1;

        // loop through all the samples getting 1s worth at a time
        while (sample_num < pMFI->totalSamples64)
        {
            int32_t samples_to_get = pMFI->audioRate;
            if (sample_num + samples_to_get >= pMFI->totalSamples64)
                samples_to_get = (int32_t)(pMFI->totalSamples64 - sample_num);

            // get the next block of samples
            samples_returned = mpegInGetNextSample(pMFI, sample_bfr, samples_to_get);
            if (samples_returned != samples_to_get)
            {
                printf("mpegInGetNextSample could not retrieve the next block of samples.\n");
                goto err_exit;
            }

            // write the samples out
            if (fwrite(sample_bfr, 1, samples_returned * pMFI->bytesPerSample, fp_out) != samples_returned * pMFI->bytesPerSample)
            {
                printf("Unable to write to output file.\n");
                goto err_exit;
            }

            sample_num += samples_returned;
            if (sample_num >= prg_next)
            {
                printf("\rProcessed %u%% ...", (uint32_t)((double)sample_num / (double)pMFI->totalSamples64 * 100.0));
                fflush(stdout);
                prg_next += prg_mod;
                if (prg_next >= pMFI->totalSamples64)
                    prg_next = pMFI->totalSamples64 - 1;
            }
        }
    }

    printf("\rProcessed 100%%.\n");

    ret = 0;

err_exit:

    printf("\n");
    // close everything
    if (pMFI)
        mpegInFree(pMFI);

    if (sample_bfr)
        free(sample_bfr);

    if (fp_out)
        fclose(fp_out);

    return ret;
}

int32_t main(int32_t argc, char * argv[])
{
    USE_MFIMPORT_API = true;
    char* pCodecPath = NULL;
    uint8_t index_es_flag = 0;
    int32_t iArgc = 0;
    char* ppArgv[128];
    int32_t iRet = 0;

    for ( int32_t i = 0; i < argc; i++ ) {
        if ( strcmp(argv[i], "-old") == 0 ) {
            USE_MFIMPORT_API = false;
        } else if ( !strncmp( argv[i], "-codec_path", strlen("-codec_path") ) ) {
            char *arg_value = strchr( argv[i], '=' );
            if (arg_value && strlen(arg_value) > 1) {
                pCodecPath = &arg_value[1];
            }
        } else if ( !strncmp( argv[i], "-index_es", strlen("-index_es") ) ) {
            index_es_flag = 1;
        } else {
            ppArgv[iArgc] = argv[i];
            iArgc++;
        }
    }

    if ( USE_MFIMPORT_API ) {
        iRet = main_new_api(iArgc, ppArgv, pCodecPath, index_es_flag);
    } else {
        iRet = main_old_api(iArgc, ppArgv, pCodecPath, index_es_flag);
    }

    return iRet;
}
