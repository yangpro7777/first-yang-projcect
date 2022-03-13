/********************************************************************
 Created: 2007/06/07
 File name: sample_mfimport_stream_info.cpp
 Purpose: command-line sample for MFImport

 Copyright (c) 2007-2011 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "mfimport.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "private_binary_data.h"

// comment this out to use the old mpegin API at the bottom of this file
bool USE_MFIMPORT_API = true;    // use new mfimport API

typedef struct app_vars_s
{
    char *in_file;
    mfimport_tt *pMFI;
    mfi_open_settings_t open_set;
    mfi_container_info_t container_info;

} app_vars_t;


static int32_t prompt_index_mode_ex(context_t context)
{
    return 1;  // 0 = no, 1 = yes
}

int32_t main_new_api(int32_t argc, char * argv[], char * pCodecPath)
{
    int32_t i, j, ret = 1;
    app_vars_t vars = {0};

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE, 1, &vars.in_file },
        { IDS_LIC_FILE, 0, &license_file_path },
    };

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params) < 0)
    {
        printf("\n==== MainConcept MFImport stream info sample ====\n"
            "Usage:\n"
            "sample_mfimport_stream_info.exe -i file.in [-codec_path=path_to_codecs]\n"
            "- Use -codec_path to specify a path where the demuxers and decoders are located\n"
            "      -lf <file_name>  path to license file\n"
            "You can use -old to switch sample to old mfimport API\n"
            "\n"
            );
        return 1;
    }

    callbacks_t callbacks;
    init_callbacks(callbacks);
    callbacks_mfimport_t mfimport_callbacks = {0};
    mfimport_callbacks.prompt_index_mode = prompt_index_mode_ex;
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
    vars.open_set.flags = MFI_OPEN_GET_INFO_ONLY;

    if (mfimportFileOpen(vars.pMFI, vars.in_file, &vars.open_set))
    {
        printf("Unable to open file.\n");
        goto err_exit;
    }

    // get the container info
    if (mfimportGetContainerInfo(vars.pMFI, &vars.container_info))
    {
        printf("Unable to get container information.\n");
        goto err_exit;
    }

    // display the container info
    switch (vars.container_info.container_type)
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
        printf("File is a MPEG-2 transport container, %d titles\n", vars.container_info.num_titles);
        break;
    case mcmjtMXF:
        printf("File is a MXF container, %d titles\n", vars.container_info.num_titles);
        break;
    case mcmjtMP4:
        printf("File is a MP4 container, %d titles\n", vars.container_info.num_titles);
        break;
    case mcmjtAVI:
        printf("File is an AVI container, %d titles\n", vars.container_info.num_titles);
        break;
    case mcmjtMKV:
        printf("File is a MKV container, %d titles\n", vars.container_info.num_titles);
        break;
    case mcmjtASF:
        printf("File is an ASF container, %d titles\n", vars.container_info.num_titles);
        break;
    }

    // display the title and stream info
    for (i = 0; i < vars.container_info.num_titles; i++)
    {
        mfi_title_info_t title_info;

        // get the title info
        if (mfimportGetTitleInfo(vars.pMFI, i, &title_info))
        {
            printf("Unable to get title %d information.\n", i);
            goto err_exit;
        }

        printf("  Title %d, ID %d\n", i, title_info.title_id);

        for (j = 0; j < title_info.num_streams; j++)
        {
            mfi_stream_info_t stream_info;

            // get the stream info
            if (mfimportGetStreamInfo(vars.pMFI, i, j, &stream_info))
            {
                printf("Unable to get title %d stream %d information.\n", i, j);
                goto err_exit;
            }

            if (!(stream_info.flags & MFI_STREAM_INFO_IS_DECODABLE))
                printf("    Stream %d, ID %d, not decodable, ", j, stream_info.stream_id);
            else
                printf("    Stream %d, ID %d, ", j, stream_info.stream_id);

            switch (stream_info.format.stream_mediatype)
            {
            case mctMPEG1V:
                printf("MPEG-1 video\n");
                break;
            case mctMPEG2V:
                printf("MPEG-2 video\n");
                break;
            case mctMPEG4V:
                printf("MPEG-4 part 2 video\n");
                break;
            case mctH263:
                printf("H.263 video\n");
                break;
            case mctH264:
                printf("AVC video\n");
                break;
            case mctMVCMux:
                printf("MVC video, number of views %d\n", stream_info.view_count);
                break;
            case mctHEVC:
                printf("HEVC video\n");
                break;
            case mctProRes:
                printf("ProRes video\n");
                break;
            case mctVC1:
                printf("VC-1 video\n");
                break;
            case mctVC3:
                printf("VC-3 video\n");
                break;
            case mctDV:
                printf("DV video\n");
                break;
            case mctJ2K:
                printf("JPEG 2000 video\n");
                break;
            case mctDIV3:
                printf("DIV3 video\n");
                break;
            case mctDIV4:
                printf("DIV4 video\n");
                break;
            case mctMPEG1A:
                printf("MPEG-1 audio\n");
                break;
            case mctMPEG2A:
                printf("MPEG-2 audio\n");
                break;
            case mctAAC_RAW:
                printf("RAW AAC audio\n");
                break;
            case mctAAC_ADTS:
                printf("ADTS AAC audio\n");
                break;
            case mctAAC_LATM:
                printf("LATM AAC audio\n");
                break;
            case mctAC3:
                printf("Dolby Digital audio\n");
                break;
            case mctAC3_DDPlus:
                printf("Dolby Digital Plus audio\n");
                break;
            case mctDTS:
                printf("DTS audio\n");
                break;
            case mctAMR:
                printf("AMR audio\n");
                break;
            case mctWMA:
                printf("WMA audio\n");
                break;
            case mctALAW:
                printf("ALAW audio\n");
                break;
            case mctPCM:
                printf("PCM audio\n");
                break;
            case mctAIFF:
                printf("AIFF audio\n");
                break;
            case mctDVD_LPCM:
                printf("DVD PCM audio\n");
                break;
            case mctHDMV_LPCM:
                printf("HDMV PCM audio\n");
                break;
            case mctAES3_302M:
                printf("AES-3 302M PCM audio\n");
                break;
            case mctAES3_382M:
                printf("AES-3 382M PCM audio\n");
                break;
            case mctAES3_331M:
                printf("AES-3 331M PCM audio\n");
                break;
            case mctTWOS_LPCM:
                printf("TWOS PCM audio\n");
                break;
            case mctQT_PCM:
                printf("QuickTime PCM audio\n");
                break;
            case mctDVB_Teletext:
                printf("DVB teletext\n");
                break;
            case mctDVD_Subtitles:
                printf("DVD subtitle\n");
                break;
            case mctDXSB_Subtitles:
                printf("DXSB subtitle\n");
                break;
            case mctUTF8_Subtitles:
                printf("UTF8 subtitle\n");
                break;
            case mctSSA_Subtitles:
                printf("SSA subtitle\n");
                break;
            case mctASS_Subtitles:
                printf("ASS subtitle\n");
                break;
            case mctUSF_Subtitles:
                printf("USF subtitle\n");
                break;
            case mctBMP_Subtitles:
                printf("BMP subtitle\n");
                break;
            case mctVSUB_Subtitles:
                printf("VSUB subtitle\n");
                break;
            case mctDVB_Subtitles:
                printf("DVB subtitle\n");
                break;
            case mctCFF_ImgSubtitles:
                printf("CFF image subtitle\n");
                break;
            case mctCFF_TxtSubtitles:
                printf("CFF text subtitle\n");
                break;
            case mctDXSA_Subtitles:
                printf("DXSA subtitle\n");
                break;
            case mctPrivateBinary:
                printf("Private binary\n");
                break;
            case mctMotionJPEG:
                printf("Motion JPEG video\n");
                break;
            default:
                printf("unknown\n");
            }

            // Determine type (IE: what we are decoding the stream to)
            if (mctMinVideoType <= stream_info.format.stream_mediatype && stream_info.format.stream_mediatype <= mctMaxVideoType)
            {
                // this can be a mixture of the ES and container info
                const mc_video_format_t& streamFormat = *reinterpret_cast<const mc_video_format_t*>(stream_info.format.pFormat);
                // this is info parsed from the elementary stream headers if any
                const mc_video_format_t& ESFormat = *reinterpret_cast<const mc_video_format_t*>(stream_info.stream_format.pFormat);
                // this is info from the container if any
                const mc_video_format_t& containerFormat = *reinterpret_cast<const mc_video_format_t*>(stream_info.container_format.pFormat);
                
                double streamFrameRate(0.0), ESFrameRate(0.0), containerFrameRate(0.0);
                if(streamFormat.avg_time_per_frame) 
                    streamFrameRate = 10000000.0 / static_cast<double>(streamFormat.avg_time_per_frame);
                if(ESFormat.avg_time_per_frame) 
                    ESFrameRate = 10000000.0 / static_cast<double>(ESFormat.avg_time_per_frame);
                if(containerFormat.avg_time_per_frame) 
                    containerFrameRate = 10000000.0 / static_cast<double>(containerFormat.avg_time_per_frame);

                printf("      Stream info:\n");
                printf("        %dx%d (par %d:%d) at %2.2f\n", streamFormat.width, streamFormat.height, streamFormat.pictAspRatioX, streamFormat.pictAspRatioY, streamFrameRate);
                if ((streamFormat.flags & MC_VIDEO_FORMAT_FLAG_INTERLACED) != 0)
                    printf("        interlaced\n");
                if ((streamFormat.flags & MC_VIDEO_FORMAT_FLAG_PROGRESSIVE) != 0)
                    printf("        progressive\n");
                if ((streamFormat.flags & MC_VIDEO_FORMAT_FLAG_TFF) != 0)
                    printf("        tff\n");
                if ((streamFormat.flags & MC_VIDEO_FORMAT_FLAG_BFF) != 0)
                    printf("        bff\n");
                #ifdef _WIN32
                    printf("        first timestamp = %I64u\n", stream_info.first_timestamp);
                #else
                    printf("        first timestamp = %llu\n", stream_info.first_timestamp);
                #endif

                printf("      ES info:\n");
                printf("        %dx%d (par %d:%d) at %2.2f\n", ESFormat.width, ESFormat.height, ESFormat.pictAspRatioX, ESFormat.pictAspRatioY, ESFrameRate);
                if ((ESFormat.flags & MC_VIDEO_FORMAT_FLAG_INTERLACED) != 0)
                    printf("        interlaced\n");
                if ((ESFormat.flags & MC_VIDEO_FORMAT_FLAG_PROGRESSIVE) != 0)
                    printf("        progressive\n");
                if ((ESFormat.flags & MC_VIDEO_FORMAT_FLAG_TFF) != 0)
                    printf("        tff\n");
                if ((ESFormat.flags & MC_VIDEO_FORMAT_FLAG_BFF) != 0)
                    printf("        bff\n");

                printf("      Container info:\n");
                printf("        %dx%d (par %d:%d) at %2.2f\n", containerFormat.width, containerFormat.height, containerFormat.pictAspRatioX, containerFormat.pictAspRatioY, containerFrameRate);
                if ((containerFormat.flags & MC_VIDEO_FORMAT_FLAG_INTERLACED) != 0)
                    printf("        interlaced\n");
                if ((containerFormat.flags & MC_VIDEO_FORMAT_FLAG_PROGRESSIVE) != 0)
                    printf("        progressive\n");
                if ((containerFormat.flags & MC_VIDEO_FORMAT_FLAG_TFF) != 0)
                    printf("        tff\n");
                if ((containerFormat.flags & MC_VIDEO_FORMAT_FLAG_BFF) != 0)
                    printf("        bff\n");
                printf("        bit_rate: %d\n", containerFormat.avg_bitrate);
            }
            else if ((mctMinAudioType <= stream_info.format.stream_mediatype && stream_info.format.stream_mediatype <= mctMaxAudioType)
                || (mctMinPCMType <= stream_info.format.stream_mediatype && stream_info.format.stream_mediatype <= mctMaxPCMType))
            {
                // this can be a mixture of the ES and container info
                const mc_audio_format_t& streamFormat = *reinterpret_cast<const mc_audio_format_t*>(stream_info.format.pFormat);
                // this is info parsed from the elementary stream headers if any
                const mc_audio_format_t& ESFormat = *reinterpret_cast<const mc_audio_format_t*>(stream_info.stream_format.pFormat);
                // this is info from the container if any
                const mc_audio_format_t& containerFormat = *reinterpret_cast<const mc_audio_format_t*>(stream_info.container_format.pFormat);
                    
                printf("      Stream info:\n");
                printf("        %d channels with %d bits per channel and %d samples per sec\n", streamFormat.channels, streamFormat.bits_per_sample, streamFormat.samples_per_sec);
                printf("        %d bytes per second\n", streamFormat.avg_bytes_per_sec);

                printf("      ES info:\n");
                printf("        %d channels with %d bits per channel and %d samples per sec\n", ESFormat.channels, ESFormat.bits_per_sample, ESFormat.samples_per_sec);
                printf("        %d bytes per second\n", ESFormat.avg_bytes_per_sec);

                printf("      Container info:\n");
                printf("        %d channels with %d bits per channel and %d samples per sec\n", containerFormat.channels, containerFormat.bits_per_sample, containerFormat.samples_per_sec);
                printf("        %d bytes per second\n", containerFormat.avg_bytes_per_sec);
            }
            else if( stream_info.format.stream_mediatype == mctPrivateBinary )
            {
                if( PrivateBinaryData::QTMP4Timecode::Recognize(stream_info) )
                {
                    PrivateBinaryData::QTMP4Timecode timecode(stream_info);
                    printf("      Timecode track:\n");
                    printf("        start timecode = %c%02d:%02d:%02d.%02d\n", timecode.negative?'-':' ', timecode.hour, timecode.minute, timecode.second, timecode.frame);
                }
            }
        }
    }

    printf("\n");

    ret = 0;

err_exit:

    // close everything
    if (vars.pMFI)
        mfimportFree(vars.pMFI);

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
        printf("  %d: %s\n", stream_num, title_str);
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

int32_t main_old_api(int32_t argc, char * argv[], char * pCodecPath)
{
    int32_t i, j, ret = 1;
    char *in_file = NULL;
    mpegInInfo *pMFI = NULL;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE, 1, &in_file},
        { IDS_LIC_FILE, 0, &license_file_path },
    };

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params) < 0)
    {
        printf("\n==== MainConcept MFImport stream info sample ====\n"
            "Usage:\n"
            "sample_mfimport_stream_info.exe -old -i file.in [-codec_path=path_to_codecs]\n"
            "- Use -codec_path to specify a path where the demuxers and decoders are located\n"
            "      -lf <file_name>  path to license file\n"
            "\n"
            );
        return 1;
    }
    callbacks_t callbacks;
    init_callbacks(callbacks);
    // create an mfimport instance
    pMFI = mpegInCreate(&callbacks, NULL);
    if (!pMFI)
    {
        printf("Unable to create a mfimport instance.\n");
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

    // get the number of streams in the file
    if (mpegInGetInfo(pMFI))
    {
        printf("Unable to get file info.\n");
        goto err_exit;
    }

    // count up all the streams
    i = pMFI->video_stream_count +
        pMFI->audio_stream_count +
        pMFI->pcm_stream_count +
        pMFI->ac3_stream_count +
        pMFI->aes3_stream_count +
        pMFI->amr_stream_count +
        pMFI->dts_stream_count +
        pMFI->wma_stream_count +
        pMFI->subpic_stream_count +
        pMFI->unknown_video_stream_count +
        pMFI->unknown_audio_stream_count;

    // display the file info
    switch (pMFI->program_stream_flag)
    {
    case STREAM_ELEMENTARY:
        printf("File is an elementary stream\n\n");
        break;
    case STREAM_PVA:
        printf("File is a PVA container, total streams %u\n\n", i);
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
    case STREAM_MXF_GENERIC:
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
    // display the stream info
    for (i = 0; i < pMFI->video_stream_count; i++)
    {
        if (i == pMFI->video_stream_to_get - 1)    // this info only applies to the selected video stream
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
            else if (pMFI->mjpeg_video_flag)
                display_stream_info(j++,
                pMFI->program_stream_flag,
                &pMFI->video_streams[i],
                "Motion JPEG video stream");
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
        case mctDXSA_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "DXSA subtitle stream");
            break;
        case mctDXSB_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "DXSB subtitle stream");
            break;
        case mctUTF8_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "UTF8 subtitle stream");
            break;
        case mctSSA_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "SSA subtitle stream");
            break;
        case mctASS_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "ASS subtitle stream");
            break;
        case mctUSF_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "USF subtitle stream");
            break;
        case mctBMP_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "BMP subtitle stream");
            break;
        case mctVSUB_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "VSUB subtitle stream");
            break;
        case mctDVD_Subtitles:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "DVD subpicture stream");
            break;
        default:
            display_stream_info(j++,
                                pMFI->program_stream_flag,
                                &pMFI->subpic_streams[i],
                                "Subpicture stream");
        }
    }

    // display selected stream info
    if (pMFI->videoAvail)
    {
        printf("  Selected video stream:\n");

        printf("      Stream info:\n");
        printf("        %dx%d ", pMFI->horizontal_size, pMFI->vertical_size);
        if (pMFI->hevc_video_flag || pMFI->h264_video_flag || pMFI->mpeg4_video_flag || pMFI->dv_video_flag || pMFI->jpeg2k_video_flag || pMFI->prores_video_flag)
            printf("(par %d:%d) ", pMFI->aspect_ratio_width, pMFI->aspect_ratio_height);
        else
            printf("(par %d) ", pMFI->aspect_ratio_information);  // mainly for mpeg-1 or mpeg-2 video
        printf("at %2.2f\n", pMFI->actual_frame_rate);

        if ((pMFI->progressive_frame) != 0)
            printf("        progressive\n");
        else
            printf("        interlaced\n");

        printf("      ES info:\n");
        printf("        at %2.2f\n", pMFI->stream_frame_rate);

        printf("      Container info:\n");
        printf("        %dx%d (par %d:%d) at %2.2f\n", pMFI->container_width, pMFI->container_height, pMFI->container_aspect_x, pMFI->container_aspect_y, pMFI->container_frame_rate);
        printf("        bit_rate: %.1f\n", pMFI->bit_rate);
    }

    if (pMFI->audioAvail)
    {
        printf("  Selected audio stream:\n");

        printf("      Stream info:\n");
        printf("        %d channels with %d bits per channel and %d samples per sec\n", pMFI->channels, pMFI->sampleSize, pMFI->audioRate);
        printf("        %d bytes per second\n", pMFI->audioBitrate >> 3);
    }


    ret = 0;

err_exit:

    // close everything
    if (pMFI)
        mpegInFree(pMFI);

    return ret;
}

int32_t main(int32_t argc, char * argv[])
{
    USE_MFIMPORT_API = true;
    char * pCodecPath = NULL;

    int32_t iRet, iArgc = 0;
    char* ppArgv[128];

    for ( int32_t i = 0; i < argc; i++ ) {

        if ( strcmp(argv[i], "-old") == 0 ) {
            USE_MFIMPORT_API = false;
        } else if ( !strncmp( argv[i], "-codec_path", strlen("-codec_path") ) ) {
            char *arg_value = strchr( argv[i], '=' );
            if (arg_value && strlen(arg_value) > 1) {
                pCodecPath = &arg_value[1];
            }
        } else {
            ppArgv[iArgc] = argv[i];
            iArgc++;
        }
    }

    if ( USE_MFIMPORT_API ) {
        iRet = main_new_api(iArgc, ppArgv, pCodecPath);
    } else {
        iRet = main_old_api(iArgc, ppArgv, pCodecPath);
    }

    return iRet;
}
