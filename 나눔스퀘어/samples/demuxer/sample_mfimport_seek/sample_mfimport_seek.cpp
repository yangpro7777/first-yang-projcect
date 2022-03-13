/********************************************************************
 Created: 2007/06/07
 File name: sample_mfimport_seek.cpp
 Purpose: command-line sample for MFImport

 Copyright (c) 2007-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include "mctypes.h"
#include "bufstrm.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "mfimport.h"
#include "mcfourcc.h"
#include "mccolorspace.h"

#include <time.h>
#if !defined(__linux__) && !defined(__APPLE__)
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

// comment this out to use the old mpegin API
bool USE_MFIMPORT_API = true;// use new mfimport API

#define    MAKE_ALIGN(x,align)    ((((x)+((align)-1))/(align))*(align))

int64_t GetCurrentTimeInMilliseconds()
{
    int64_t i64CurrentTime;

#if defined(__linux__) || defined(__APPLE__)
    timeval tv;
    gettimeofday(&tv, NULL);

    i64CurrentTime = (tv.tv_sec * 1000 + tv.tv_usec/1000);
#elif !defined (UNDER_CE)
    struct _timeb tstruct;
    _ftime( &tstruct );
    i64CurrentTime = tstruct.time * 1000 + tstruct.millitm;
#else
    i64CurrentTime = timeGetTime();
#endif

    return i64CurrentTime;
}


// parse_seek_string
// parses through the seek sequence string
// return -1 on parse error or string position
// on pass
int parse_seek_string(const char* str,
                      int maxlen,
                      int pos,
                      bool& absolute,
                      int& startPos,
                      int& length)
{
    if (!str)
        return -1;
    if ( pos >= maxlen)
        return -2;

    bool isNeg = false;
    absolute = false;
    startPos = length = 0;

    char c;
    int state=0;
    while ((c = str[pos++]) != 0)
        switch (state)
        {
            case 0:
                if (c == '+')
                {
                    absolute = false;
                    startPos = 0;
                    state = 1;
                }
                else if (c == '-')
                {
                    absolute = false;
                    isNeg = true;
                    startPos = -1;
                    state = 2;
                }
                else if ( c >= '0' && c <= '9')
                {
                    absolute = true;
                    startPos = + (c - '0'); // absolute starting frame
                    state = 3;
                }
                else if (c == ';')  // ignore semicolon
                    break;
                else
                    return -1;   // parse error!
                break;
            case 1:
            case 2:
                if ( c >= '0' && c <= '9')
                {
                    if (isNeg)
                        startPos = - (c - '0');
                    else
                        startPos = + (c - '0');
                    state = 3;
                }
                else
                    return -1; // parse error!
                break;
            case 3:
                if ( c >= '0' && c <= '9')
                {
                    if (isNeg)
                        startPos = startPos * 10 - (c - '0');
                    else
                        startPos = startPos * 10 + (c - '0');
                }
                else if (c == ':')
                    state = 4;
                else
                    return -1; // parse error!
                break;
            case 4:
                if ( c >= '0' && c <= '9')
                {
                    length = (c - '0');
                    state = 5;
                }
                else if (c == '-')
                {
                    // until eos
                    length = -1;
                    return pos; // finished!
                }
                else
                    return -1; // parse error!
                break;
            case 5:
                if (c >= '0' && c <= '9')
                {
                    length = length * 10 + (c - '0');
                }
                else if (c != ';')
                    return -1; // parse error
                else
                    return pos;   // finished!
                break;
        }
    return pos;
}

typedef struct app_vars_s
{
    char *in_file;
    char *out_file;
    uint32_t fourcc;
    int32_t tid;
    int32_t sid;
    int32_t smp_mode; 
    mfimport_tt *pMFI;
    mfimport_stream_tt *pStream;
    mfi_open_settings_t open_set;
    mfi_stream_info_t stream_info;
    mfi_stream_settings_t stream_set;
    mfi_seek_settings_t seek_info;
    mfi_sample_settings_t sample_set;
    mfi_sample_info_t sample_info;
} app_vars_t;

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
    default:
        printf("File is an unknown container, %d titles\n", container_info.num_titles);
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
            case mctMotionJPEG:
                printf("Motion JPEG video\n");
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
            case mctDVD_Subtitles:
                printf("DVD subtitle\n");
                break;
            default:
                printf("unknown\n");
            }
        }
    }

    return 0;
}

void print_frame_info(char* chFrameInfo, mfi_sample_info_t *SampleInfo)
{
  if( !chFrameInfo || !SampleInfo)
    return;

  if(SampleInfo->flags & MFI_SAMPLE_IS_MBAFF || SampleInfo->flags & MFI_SAMPLE_IS_PROGRESSIVE )
  {
    if(SampleInfo->flags & MFI_SAMPLE_IS_MBAFF)
      sprintf(chFrameInfo, "MBAFF Frame: ");
    else
      sprintf(chFrameInfo, "Progressive Frame: ");

    if(SampleInfo->flags & MFI_SAMPLE_IS_I_TYPE)
      strcat(chFrameInfo, "type I\n");
    else if(SampleInfo->flags & MFI_SAMPLE_IS_P_TYPE)
      strcat(chFrameInfo, "type P\n");
    else if(SampleInfo->flags & MFI_SAMPLE_IS_B_TYPE)
      strcat(chFrameInfo, "type B\n");
    else
      strcat(chFrameInfo,"Picture type Unknown\n");
  }
  else if(SampleInfo->flags & MFI_SAMPLE_IS_INTERLACED)
  {
    sprintf(chFrameInfo, "Interlaced Frame: ");

    if(SampleInfo->flags & MFI_SAMPLE_IS_I_TYPE)
      strcat(chFrameInfo, "Field 1 type I, ");
    else if(SampleInfo->flags & MFI_SAMPLE_IS_P_TYPE)
      strcat(chFrameInfo, "Field 1 type P, ");
    else if(SampleInfo->flags & MFI_SAMPLE_IS_B_TYPE)
      strcat(chFrameInfo, "Field 1 type B, ");
    else
      strcat(chFrameInfo, "Field 1 type Unknown,");

    if(SampleInfo->flags & MFI_SAMPLE_IS_I_TYPE_FIELD2)
      strcat(chFrameInfo, " Field 2 type I\n");
    else if(SampleInfo->flags & MFI_SAMPLE_IS_P_TYPE_FIELD2)
      strcat(chFrameInfo, " Field 2 type P\n");
    else if(SampleInfo->flags & MFI_SAMPLE_IS_B_TYPE_FIELD2)
      strcat(chFrameInfo, " Field 2 type B\n");
    else
      strcat(chFrameInfo, " Field 2 type Unknown\n");
  }

  if(SampleInfo->flags & MFI_SAMPLE_IS_BROKEN)
    strcat(chFrameInfo, " - Broken frame");

//  strcat(chFrameInfo, "\n");
}

#define IDC_NO_EDIT_LISTS (IDC_CUSTOM_START_ID + 1)
#define IDC_SHOW_PREROLL (IDC_CUSTOM_START_ID + 2)
#define IDC_DISABLE_SYNC (IDC_CUSTOM_START_ID + 3)
#define IDC_INDEX_ES (IDC_CUSTOM_START_ID + 4)

int32_t main_new_api(int32_t argc, char * argv[])
{
    int32_t ret = 1;
    uint8_t is_video = 0;
    uint8_t is_subpic = 0;
    char *seekStr = NULL;
    FILE *fp_out = NULL;
    uint8_t *sample_bfr = NULL;
    int32_t sample_bfr_size;
    app_vars_t vars = {0};

    int32_t pos;
    bool absolute;
    int32_t startPos;
    int32_t length;
    int32_t maxLen;
    int32_t currentFrame;
    int32_t numArgs;
    int32_t result;
    uint8_t bOutputFlag = 1;
    int32_t noelst = 0;
    int32_t show_preroll = 0;
    int32_t index_es = 0;
    int32_t write_wav_header = 0;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE,       1, &vars.in_file},
        { IDI_C_TITLE_ID,       0, &vars.tid},
        { IDI_C_STREAM_ID,      0, &vars.sid},
        { IDN_V_FOURCC,         0, &vars.fourcc},
        { IDS_OUTPUT_FILE,      0, &vars.out_file},
        { IDN_V_SMP,            0, &vars.smp_mode},
        { IDS_LIC_FILE,         0, &license_file_path },
        { IDC_NO_EDIT_LISTS,    0, &noelst },
        { IDC_SHOW_PREROLL,     0, &show_preroll },
        { IDC_INDEX_ES,         0, &index_es },
    };

    const arg_item_desc_t custom_args[] =
    {
        { IDC_NO_EDIT_LISTS, {"ignore_edit_lists",  ""}, ItemTypeInt, 0, "Enable to ignore mp4 edit lists" },
        { IDC_SHOW_PREROLL, {"show_preroll_frames", ""}, ItemTypeInt, 0, "Enable to show preroll frames" },
        { IDC_INDEX_ES, {"index_es", ""}, ItemTypeInt, 0, "Enable indexing of elementary streams" },
    };

    if (argc <= 5)
        numArgs = argc-1;
    else
        numArgs = argc-2;

    result = parse_args(numArgs, argv + 1, sizeof(params) / sizeof(params[0]), params, custom_args, sizeof(custom_args) / sizeof(custom_args[0]));

    if ((result < 0) || ((numArgs == argc) && (strcmp(argv[argc - 2], "-seek") != 0)))
    {
        printf("\n==== MainConcept MFImport stream sample ====\n"
            "Usage:\n"
            "  sample_mfimport_seek.exe -i <filename> -tid <id> -sid <id> -o <filename> [-<fourcc>] <seek-seq>[;<seek-seq>]+\n\n"
            "  -i <filename>           input filename\n"
            "  -tid <id>               title id\n"
            "  -sid <id>               stream id\n"
            "  -smp <mode>             SMP mode for video stream decoders (MPEG-2, H264, HEVC, ProRes)\n"
            "                            0 = Use decoders default mode,\n"
            "                            1 = Serial decoding,\n"
            "                            2 = Parallel decoding by pictures,\n"
            "                            3 = Parallel decoding by slices\n"
            "  -<fourcc>               output fourcc (=BGR4)\n"
            "  -ignore_edit_lists      flag to disable MPEG-4 edit lists\n"
            "                               0 = do not ignore MPEG-4 edit lists (default)\n"
            "                               1 = ignore MPEG-4 edit lists\n"
            "  -show_preroll_frames    flag to enable decoding of pre-roll frames\n"
            "                               0 = do not show pre-roll frames (default)\n"
            "                               1 = show pre-roll frames\n"
            "  -o <filename>           output filename\n"
            "  -lf <file_name>         path to license file\n"
            "  <seek-seq>              seek sequence, examples:\n"
            "                           +29:20   seek forward 29 frames, read 20 frames\n"
            "                           -14:40   seek backward 14 frame, read 40 frames\n"
            "                            12:-    seek to absolute frame #12, read until EOF\n"
            " You can use -old to switch to old mfimport API\n"
            " Supported fourcc values: \n"
            "    ARGB, AYUV, BGR3, BGR4, BGRA, I420, P010, P016, P216, P210, R24C, R32C,\n"
            "    R555, R565, UYVY, UyVy, V210, VUYA, YV12, YV16, YUY2, Y216, Y210, YUYV,\n"
            "    YuYv\n"
            );
        return 0;
    }

    seekStr = argv[argc-1];
    callbacks_t callbacks;
    init_callbacks(callbacks);
    // create a mfimport instance
    vars.pMFI = mfimportCreate(&callbacks, NULL);
    if (!vars.pMFI)
    {
        printf("Unable to create a mfimport instance.\n");
        goto err_exit;
    }

    // open the file
    memset(&vars.open_set, 0, sizeof(mfi_open_settings_t));
    vars.open_set.index_mode_required_action = 2;    // call the prompt_index_mode_ex if index mode is required

    if (noelst == 1)
        vars.open_set.flags |= MFI_OPEN_MP4_IGNORE_EDIT_LIST;	// do not ignore mp4 edit lists by default

    if (show_preroll != 1)
        vars.open_set.flags |= MFI_OPEN_IGNORE_PREROLL;		// ignore pre-roll frames by default

    if (index_es == 1)
        vars.open_set.flags = MFI_OPEN_ES_INDEX_MODE;   // open all elementary streams in index mode

    if (mfimportFileOpen(vars.pMFI, vars.in_file, &vars.open_set))
        goto err_exit;

    if (argc <= 5)
    {
        // just display the stream info and exit
        ret = display_file_info(&vars);
        mfimportFree(vars.pMFI);
        return ret;
    }

    // get the stream info
    if (mfimportGetStreamInfo(vars.pMFI, vars.tid, vars.sid, &vars.stream_info))
        goto err_exit;

    if (vars.fourcc == ITEM_NOT_INIT)
        vars.fourcc = FOURCC_BGR4;

    pos=0;
    absolute=false;
    startPos=0;
    length=0;
    maxLen = seekStr ? (int)strlen(seekStr) : 0;
    currentFrame=0;

    while ((pos = parse_seek_string(seekStr,
                                    maxLen,
                                    pos,
                                    absolute,
                                    startPos,
                                    length)) > 0);
    if (pos == -1)
    {
        printf("Invalid seek sequence.\n");
        goto err_exit;
    }

    vars.stream_set.title_idx = vars.tid;
    vars.stream_set.stream_idx = vars.sid;

    mfi_stream_instance_info_t stream_instance_info;
    vars.pStream = mfimportStreamNew(vars.pMFI, &vars.stream_set, &stream_instance_info);
    if (!vars.pStream)
        goto err_exit;

    // set SMP mode for video decoder
    if((vars.smp_mode > 0) &&
        (vars.stream_info.format.stream_mediatype == mctH264 ||
         vars.stream_info.format.stream_mediatype == mctHEVC ||
         vars.stream_info.format.stream_mediatype == mctMPEG2V))
      stream_instance_info.p_decoder_instance->auxinfo(stream_instance_info.p_decoder_instance, vars.smp_mode -1, SET_SMP_MODE, NULL, 0);

    if (vars.out_file == 0)
    {
        printf("Output file was not specified.\n");
        goto err_exit;
    }

    if (!strcmp(vars.out_file, "NULL") || !strcmp(vars.out_file, "null"))
        bOutputFlag = 0;
    else
    {
        // open the output file
        fp_out = fopen(vars.out_file, "wb");
        if (!fp_out)
        {
            printf("Unable to open output file.\n");
            goto err_exit;
        }
        char* s;
        if (((s = strstr(vars.out_file, ".wav")) ||
            (s = strstr(vars.out_file, ".WAV"))) && strlen(s) == 4)
        {
            write_wav_header = 1;
        }
    }

    if ((vars.stream_info.format.stream_mediatype >= mctMinVideoType) &&
        (vars.stream_info.format.stream_mediatype <= mctMaxVideoType))
    {
        mc_video_format_t *pVideoFormat = (mc_video_format_t*)vars.stream_info.format.pFormat;
        char chFrameInfo[256] = "";

        // create a buffer for the video frames
        frame_tt frame;
        memset(&frame, 0, sizeof(frame_tt));

        // Use a helper function from mccolorspace.h to help calculate the buffer size
        frame_colorspace_info_tt cs_info = { 0 };
        get_frame_colorspace_info(&cs_info, pVideoFormat->width, pVideoFormat->height, vars.fourcc, 0);

        sample_bfr_size = cs_info.frame_size;
        sample_bfr = (uint8_t*)malloc(sample_bfr_size);
        if (!sample_bfr)
        {
            printf("Unable to allocate memory for a sample buffer.\n");
            goto err_exit;
        }

        // Use a helper function mccolorspace.h to update the frame_tt struct with the sample buffer
        fill_frame_from_colorspace_info(&cs_info, sample_bfr, &frame);

        vars.sample_set.p_frame = &frame;

        printf("\nTotal frames = %llu, FOURCC=\'%c%c%c%c\'\n",
            vars.stream_info.total_samples, frame.four_cc&255, (frame.four_cc>>8)&255,
            (frame.four_cc>>16)&255, frame.four_cc>>24);

        seekStr = argv[argc-1];

        currentFrame=0;
        // reset the parse settings
        pos=0;
        absolute=false;
        startPos=0;
        length=0;
        while ((pos = parse_seek_string(seekStr,
                                        maxLen,
                                        pos,
                                        absolute,
                                        startPos,
                                        length)) > 0)
        {
            if (absolute)
            {
                if (startPos >= vars.stream_info.total_samples)
                {
                    printf("\nSeeking position is out of range.\n");
                    goto err_exit;
                }
                if ((length >= 0) && (startPos + length-1) >= vars.stream_info.total_samples)
                {
                    printf("\nSeeking length is out of range, corrected.\n");
                    length = (int32_t)(vars.stream_info.total_samples - startPos - 1);
                }
                currentFrame = startPos;
            }
            else
            {
              // relative position seeking
              if ((currentFrame + startPos) < 0 ||
                  (currentFrame + startPos) >= vars.stream_info.total_samples)
              {
                  printf("\nSeeking position frame %d is out of range.\n", currentFrame+startPos);
                  goto err_exit;
              }
              currentFrame += startPos;

              if ((length >= 0) && (currentFrame + length-1) >= vars.stream_info.total_samples)
              {
                  printf("\nSeeking length is out of range, corrected.\n");
                  length = (int32_t)(vars.stream_info.total_samples - currentFrame - 1);
              }
            }
            int count=length;

            if (length == 0)
            {
                printf("length is zero, skipping.\n");
                continue;
            }

            // seek to the first frame
            vars.seek_info.flags = MFI_SEEK_BY_SAMPLE_NUMBER;
            vars.seek_info.sample_start = currentFrame;
            vars.sample_set.num_samples = 1;

            if (mfimportStreamSeek(vars.pStream, &vars.seek_info, &vars.sample_set, &vars.sample_info))
            {
                printf("Seeking error.\n");
                goto err_exit;
            }

            printf("Flags: 0x%x\n", vars.sample_info.flags);

            print_frame_info(chFrameInfo, &vars.sample_info);
            printf(chFrameInfo);
            

            // output sequence
            printf("\nSeeking ");
            if (absolute)
                printf("to frame %d, ", startPos);
            else
                printf("%+d frame(s), ", startPos);

            if(count > 0)
            {
                printf("reading %d frame(s)\n", count);
                --count;
                ++currentFrame;
            }
            else {
                printf("reading until EOF\n");
            }

            if (bOutputFlag)
            {
                // write the frame out
                if (fwrite(sample_bfr, 1, sample_bfr_size, fp_out) != sample_bfr_size)
                {
                    printf("Unable to write to output file.\n");
                    goto err_exit;
                }
            }

            // loop through the remaining frames
            bool isInfinite = count == -1;
            bool end = count <= 0;
            bool isEOS = false;
            while ( !isEOS && ( isInfinite || !end ) )
            {
              // get the next frame
              vars.sample_set.num_samples = 1;

              ret = mfimportStreamGetNext(vars.pStream, &vars.sample_set, &vars.sample_info);
              if(ret == mfiErrorEndOfStream)
              {
                  end = true;
                  break;
              }
              else if(ret)
              {
                  printf("mfimportStreamGetNext() failed.\n");
                  goto err_exit;
              }


              print_frame_info(chFrameInfo, &vars.sample_info);
              
              if (bOutputFlag)
              {
                  // write the frame out
                  if (fwrite(sample_bfr, 1, sample_bfr_size, fp_out) != sample_bfr_size)
                  {
                      printf("Unable to write to output file.\n");
                      goto err_exit;
                  }
              }

              currentFrame++;

              // if we asked for a fixed number of frames
              if(count > 0)
              {
                  --count;
                  if(count <= 0)
                      isEOS = true;
                  fprintf( stderr, "\rProcessed %u%% ...", ((length-1-count) * 100) / length);
              }
              else {
                  fprintf( stderr, "\rProcessed %u%% ...", (int32_t)((currentFrame - startPos) * 100 / (vars.stream_info.total_samples - startPos - 1)));
              }
              
              printf(chFrameInfo);
            }
            printf("\rProcessed 100%%          ");
        }
    }
    else
    {
        //////////////////    seek by AUDIO
        mc_audio_format_t*  audio_format = (mc_audio_format_t*)vars.stream_info.format.pFormat;
        uint32_t            bytesPerSample = audio_format->bits_per_sample / 8 * audio_format->channels;
        uint32_t total_audio_data_size = 0;

        int64_t stepSample = audio_format->samples_per_sec;

        sample_bfr_size = (int32_t)(bytesPerSample * stepSample);
        sample_bfr = (uint8_t*)malloc(sample_bfr_size);
        if (!sample_bfr)
        {
            printf("Unable to allocate memory for a sample buffer.\n");
            goto err_exit;
        }
        vars.sample_set.p_buffer = sample_bfr;

        // reset the parse settings
        int64_t CurrentAbsolutPosition = 0;
        int currentSeekStringPosition = 0;
        absolute = false;
        startPos = 0;
        length = 0;
        while ((currentSeekStringPosition = parse_seek_string(seekStr,
            maxLen,
            currentSeekStringPosition,
            absolute,
            startPos,
            length)) > 0 && length != 0)
        {
            if (length < 0) {
                length = (int32_t)vars.stream_info.total_samples;
            }
            if (absolute) {
                if (startPos + length > vars.stream_info.total_samples) {
                    if (vars.stream_info.total_samples < startPos)
                    {
                        printf("Seek interval (absolute) %d:%d exeeds the maximum audio samples (%d). It is rejected.\n", startPos, length, (int32_t)vars.stream_info.total_samples);
                        continue;
                    }
                    length = (int32_t)(vars.stream_info.total_samples - startPos);
                }
                CurrentAbsolutPosition = startPos;

                vars.seek_info.flags = MFI_SEEK_BY_SAMPLE_NUMBER;
                vars.seek_info.sample_start = CurrentAbsolutPosition;
                vars.sample_set.num_samples = 0;

                if (mfimportStreamSeek(vars.pStream, &vars.seek_info, 0, 0))
                {
                    printf("Seeking error.\n");
                    goto err_exit;
                }
            }
            else {
                if (CurrentAbsolutPosition + startPos + length > (int32_t)vars.stream_info.total_samples) {
                    if ((int64_t)vars.stream_info.total_samples < CurrentAbsolutPosition + startPos) 
                    {
                        printf("Seek interval (relative): %d:%d exeeds the maximum audio samples (%d). Current position: %d. It is rejected.\n", startPos, length, (int32_t)vars.stream_info.total_samples, (int32_t)CurrentAbsolutPosition);
                        continue;
                    }
                    length = (int32_t)(vars.stream_info.total_samples - (CurrentAbsolutPosition + startPos));
                }
                CurrentAbsolutPosition += startPos;

                vars.seek_info.flags = MFI_SEEK_BY_SAMPLE_NUMBER;
                vars.seek_info.sample_start = CurrentAbsolutPosition;
                vars.sample_set.num_samples = 0;

                if (mfimportStreamSeek(vars.pStream, &vars.seek_info, 0, 0))
                {
                    printf("Seeking error.\n");
                    goto err_exit;
                }
            }
            if (CurrentAbsolutPosition + length > (int64_t)vars.stream_info.total_samples)
            {
                printf("Adjust seek interval length: %d->%d.\n", length, (int32_t)(vars.stream_info.total_samples - CurrentAbsolutPosition));
                length = (int32_t)(vars.stream_info.total_samples - CurrentAbsolutPosition);
            }
            printf("Seek interval %d:%d\n", (int32_t)CurrentAbsolutPosition, length);

            for (int32_t TotalSamplesPerSeek = 0; TotalSamplesPerSeek < length; ) {

                int32_t Samples2Get = (int32_t)stepSample;
                if (TotalSamplesPerSeek + Samples2Get > length) {
                    Samples2Get = length - TotalSamplesPerSeek;
                }

                // get the next block of samples
                vars.sample_set.num_samples = Samples2Get;
                int32_t ret = mfimportStreamGetNext(vars.pStream, &vars.sample_set, &vars.sample_info);
                if (ret == mfiErrorEndOfStream)
                    break;
                else if (ret != mfiNoError)
                {
                    printf("Get sample error.\n");
                    goto err_exit;
                }

                int32_t SamplesReceived = vars.sample_info.num_samples_returned;

                CurrentAbsolutPosition += SamplesReceived;
                TotalSamplesPerSeek += SamplesReceived;

                //write the samples out
                if (write_wav_header == 1)
                {
                    write_wav_header = -1;
                    wav_hdr_param wav_hdr = {0};
                    wav_header_write(fp_out, &wav_hdr);
                }

                // write the samples out
                if (fwrite(sample_bfr, 1, SamplesReceived * bytesPerSample, fp_out) != SamplesReceived * bytesPerSample)
                {
                    printf("Unable to write to output file.\n");
                    goto err_exit;
                }
                total_audio_data_size += SamplesReceived * bytesPerSample;
            }
        }
        if (write_wav_header == -1)
        {
            wav_hdr_param wav_hdr = {0};
        
            wav_hdr.bits_per_sample = 16;
            wav_hdr.sample_rate = audio_format->samples_per_sec;
            wav_hdr.num_channels = audio_format->channels;
            wav_hdr.block_align = (16 >> 3) * wav_hdr.num_channels;
            wav_hdr.bytes_per_sec = wav_hdr.sample_rate * wav_hdr.block_align;
            wav_hdr.data_size = total_audio_data_size;
            wav_header_write(fp_out, &wav_hdr);
        }
    }

    printf("\n");

    ret = 0;

err_exit:

    // close everything
    if (vars.pStream)
        mfimportStreamFree(vars.pStream);

    if (vars.pMFI)
        mfimportFree(vars.pMFI);

    if (sample_bfr)
        free(sample_bfr);

    if (fp_out)
        fclose(fp_out);

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
        pMFI->subpic_stream_count;

    switch (pMFI->program_stream_flag)
    {
    case STREAM_ELEMENTARY:
        printf("File is an elementary stream\n\n");
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

    printf("\n");
}


int32_t main_old_api(int32_t argc, char * argv[])
{
    int32_t ret = 1;
    int32_t sid = 0;
    int32_t smp_mode = 0;
    uint32_t fourcc = 0;
    uint8_t is_video = 0;
    uint8_t is_subpic = 0;
    char *in_file = NULL;
    char *out_file = NULL;
    char *seekStr = NULL;
    char *dummy_str = NULL;
    FILE *fp_out = NULL;
    mpegInInfo *pMFI = NULL;
    uint8_t *sample_bfr = NULL;
    int32_t sample_bfr_size;
    int32_t audio_stream_base;
    int32_t pcm_stream_base;
    int32_t ac3_stream_base;
    int32_t aes3_stream_base;
    int32_t amr_stream_base;
    int32_t wma_stream_base;

    int pos;
    bool absolute;
    int startPos;
    int length;
    int maxLen;
    int currentFrame;
    int numArgs;
    int32_t result;
    uint8_t bOutputFlag = 1;
    int32_t noelst = 0;
    int32_t show_preroll = 0;
    int32_t disable_sync = 0;
    int32_t index_es = 0;
    int32_t write_wav_header = 0;
    uint32_t total_audio_data_size = 0;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE,       1, &in_file },
        { IDI_C_STREAM_ID,      0, &sid },
        { IDN_V_FOURCC,         0, &fourcc },
        { IDS_OUTPUT_FILE,      0, &out_file },
        { IDN_V_SMP,            0, &smp_mode },
        { IDS_LIC_FILE,         0, &license_file_path },
        { IDC_NO_EDIT_LISTS,    0, &noelst },
        { IDC_SHOW_PREROLL,     0, &show_preroll },
        { IDC_DISABLE_SYNC,     0, &disable_sync },
        { IDC_INDEX_ES,         0, &index_es},
    };

    const arg_item_desc_t custom_args[] =
    {
        { IDC_NO_EDIT_LISTS, {"ignore_edit_lists",  ""}, ItemTypeInt, 0, "Enable to ignore mp4 edit lists" },
        { IDC_SHOW_PREROLL, {"show_preroll_frames", ""}, ItemTypeInt, 0, "Enable to show preroll frames" },
        { IDC_DISABLE_SYNC, {"disable_sync", ""}, ItemTypeInt, 0, "Disable synchronization" },
        { IDC_INDEX_ES, {"index_es", ""}, ItemTypeInt, 0, "Enable indexing of elementary streams" },
    };

    if (argc <= 5)
        numArgs = argc-1;
    else
        numArgs = argc-2;

    result = parse_args(numArgs, argv + 1, sizeof(params) / sizeof(params[0]), params, custom_args, sizeof(custom_args) / sizeof(custom_args[0]));

    if ((result < 0) || ((numArgs == argc) && (strcmp(argv[argc - 2], "-seek") != 0)))
    {
        printf("\n==== MainConcept MFImport stream sample ====\n"
            "Usage:\n"
            "  sample_mfimport_seek.exe -old -i <filename> -sid <id> -o <filename> <seek-seq>[;<seek-seq>]+\n\n"
            "  -old                    use old mfimport API\n"
            "  -i <filename>           input filename\n"
            "  -sid <id>               stream id\n"
            "  -smp <mode>             SMP mode for video stream decoders (MPEG-2, H264, HEVC, ProRes)\n"
            "                            0 = Use decoders default mode,\n"
            "                            1 = Serial decoding,\n"
            "                            2 = Parallel decoding by pictures,\n"
            "                            3 = Parallel decoding by slices\n"
            "  -<fourcc>               output fourcc (=BGR4)\n"
            "  -ignore_edit_lists      flag to disable MPEG-4 edit lists\n"
            "                               0 = do not ignore MPEG-4 edit lists (default)\n"
            "                               1 = ignore MPEG-4 edit lists\n"
            "  -disable_sync           flag to disable audio and video synchronization\n"
            "                               0 = synchronization works (default)\n"
            "                               1 = disable synchronization\n"
            "  -show_preroll_frames    flag to enable decoding of pre-roll frames\n"
            "                               0 = do not show pre-roll frames (default)\n"
            "                               1 = show pre-roll frames\n"
            "  -o <filename>           output filename\n"
            "  -lf <file_name>         path to license file\n"
            "  <seek-seq>              seek sequence, examples:\n"
            "                           +29:20   seek forward 29 frames, read 20 frames\n"
            "                           -14:40   seek backward 14 frame, read 40 frames\n"
            "                            12:-    seek to absolute frame #12, read until EOF\n"
            " Supported fourcc values: \n"
            "    ARGB, AYUV, BGR3, BGR4, BGRA, I420, P010, P016, P216, P210, R24C, R32C,\n"
            "    R555, R565, UYVY, UyVy, V210, VUYA, YV12, YV16, YUY2, Y216, Y210, YUYV,\n"
            "    YuYv\n"
            );
        return 0;
    }

    seekStr = argv[argc-1];

    callbacks_t callbacks;
    init_callbacks(callbacks);
    // create a mfimport instance
    pMFI = mpegInCreate(&callbacks, NULL);
    if (!pMFI)
    {
        printf("Unable to create a mfimport instance.\n");
        goto err_exit;
    }

    // set this for the mpegInInfo function
    pMFI->inputFilename = in_file;

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
        goto err_exit;
    }

    if (fourcc == ITEM_NOT_INIT)
        fourcc = FOURCC_BGR4;

    pos=0;
    absolute=false;
    startPos=0;
    length=0;
    maxLen = seekStr ? (int)strlen(seekStr) : 0;
    currentFrame=0;

    while ((pos = parse_seek_string(seekStr,
                                    maxLen,
                                    pos,
                                    absolute,
                                    startPos,
                                    length)) > 0);
    if (pos == -1)
    {
        printf("Invalid seek sequence.\n");
        goto err_exit;
    }

    if (out_file == 0)
    {
        printf("Output file was not specified.\n");
        goto err_exit;
    }

    if (!strcmp(out_file, "NULL") || !strcmp(out_file, "null"))
        bOutputFlag = 0;
    else
    {
        // open the output file
        fp_out = fopen(out_file, "wb");
        if (!fp_out)
        {
            printf("Unable to open output file.\n");
            goto err_exit;
        }
        char* s;
        if (((s = strstr(out_file, ".wav")) ||
            (s = strstr(out_file, ".WAV"))) && strlen(s) == 4)
        {
            write_wav_header = 1;
        }

    }

    // set the proper 'stream_to_get'
    pMFI->video_stream_to_get = 0;
    pMFI->audio_stream_to_get = 0;
    pMFI->pcm_stream_to_get = 0;
    pMFI->ac3_stream_to_get = 0;
    pMFI->aes3_stream_to_get = 0;
    pMFI->amr_stream_to_get = 0;
    pMFI->wma_stream_to_get = 0;
    pMFI->subpic_stream_to_get = 0;

    audio_stream_base = pMFI->video_stream_count;
    pcm_stream_base = audio_stream_base + pMFI->audio_stream_count;
    ac3_stream_base = pcm_stream_base + pMFI->pcm_stream_count;
    aes3_stream_base = ac3_stream_base + pMFI->ac3_stream_count;
    amr_stream_base = aes3_stream_base + pMFI->aes3_stream_count;
    wma_stream_base = amr_stream_base + pMFI->amr_stream_count;

    if (sid < audio_stream_base)
    {
        pMFI->video_stream_to_get = sid + 1;
        pMFI->use_external_frame_buffer = 1;
        is_video = 1;
    }
    else if (sid < pcm_stream_base)
        pMFI->audio_stream_to_get = sid - audio_stream_base + 1;
    else if (sid < ac3_stream_base)
        pMFI->pcm_stream_to_get = sid - pcm_stream_base + 1;
    else if (sid < aes3_stream_base)
        pMFI->ac3_stream_to_get = sid - ac3_stream_base + 1;
    else if (sid < amr_stream_base)
        pMFI->aes3_stream_to_get = sid - aes3_stream_base + 1;
    else if (sid < wma_stream_base)
        pMFI->amr_stream_to_get = sid - amr_stream_base + 1;
    else
        pMFI->wma_stream_to_get = sid - wma_stream_base + 1;

    if (noelst == 1)
        pMFI->ignore_edit_list = 1;     // do not ignore mp4 edit lists by default

    if (show_preroll != 1)
        pMFI->ignore_preroll_frames = 1;     // ignore pre-roll frames by default

    if (disable_sync == 1)
        pMFI->disable_sync = 1;

    if (index_es == 1)
        pMFI->es_index_mode_flag = 1;

    pMFI->fourCC = fourcc;

    // now open the file
    if (mpegInOpen(pMFI, in_file))
        goto err_exit;

    if (is_video)
    {
        if(smp_mode > 0)
        {
          mpegInVideoAuxinfo(pMFI, smp_mode -1, SET_SMP_MODE, NULL, 0);
        }

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

        printf("\nTotal frames = %d, FOURCC=\'%c%c%c%c\'\n",
            pMFI->totalFrames, fourcc&255, (fourcc>>8)&255,
            (fourcc>>16)&255, fourcc>>24);

        seekStr = argv[argc-1];

        currentFrame=0;
        // reset the parse settings
        pos=0;
        absolute=false;
        startPos=0;
        length=0;
        while ((pos = parse_seek_string(seekStr,
                                        maxLen,
                                        pos,
                                        absolute,
                                        startPos,
                                        length)) > 0)
        {
            if (absolute)
            {
                if (startPos >= pMFI->totalFrames)
                {
                    printf("\nSeeking position is out of range.\n");
                    goto err_exit;
                }
                if ((startPos + length-1) >= pMFI->totalFrames)
                {
                    printf("\nSeeking length is out of range, corrected.\n");
                    length = pMFI->totalFrames - startPos - 1;
                }
                currentFrame = startPos;
            }
            else
            {
                // relative position seeking
                if ((currentFrame + startPos) < 0 ||
                    (currentFrame + startPos) >= pMFI->totalFrames)
                {
                    printf("\nSeeking position frame %d is out of range.\n", currentFrame+startPos);
                    goto err_exit;
                }
                currentFrame += startPos;

                if ((currentFrame + length-1) >= pMFI->totalFrames)
                {
                    printf("\nSeeking length is out of range, corrected.\n");
                    length = pMFI->totalFrames - currentFrame - 1;
                }
            }
            int count=length;

            if (length == 0)
            {
                printf("length is zero, skipping.\n");
                continue;
            }

            // seek to the first frame
            if (mpegInSeekFrame(pMFI, currentFrame))
            {
                printf("Seeking error.\n");
                goto err_exit;
            }
            // output sequence
            printf("\nSeeking ");
            if (absolute)
                printf("to frame %d, ", startPos);
            else
                printf("%+d frame(s), ", startPos);

            if(count > 0)
            {
                printf("reading %d frame(s)\n", count);
                --count;
                ++currentFrame;
            }
            else {
                printf("reading until EOF\n");
            }

            if (pMFI->broken_frame)
                printf("Broken frame\n");

            if (bOutputFlag)
            {
                // write the frame out
                if (fwrite(sample_bfr, 1, sample_bfr_size, fp_out) != sample_bfr_size)
                {
                    printf("Unable to write to output file.\n");
                    goto err_exit;
                }
            }

            // loop through the remaining frames
            bool isInfinite = count == -1;
            bool end = count <= 0;
            bool isEOS = false;
            while ( !isEOS && ( isInfinite || !end ) )
            {
                // get the next frame
                ret = mpegInGetNextFrame(pMFI);
                if (ret == mpegInEOF)
                {
                    end = true;
                    break;
                }
                else if (ret)
                {
                    goto err_exit;
                }

                if (bOutputFlag)
                {
                    // write the frame out
                    if (fwrite(sample_bfr, 1, sample_bfr_size, fp_out) != sample_bfr_size)
                    {
                        printf("Unable to write to output file.\n");
                        goto err_exit;
                    }
                }

                currentFrame++;
                if(count >= 0)
                {
                    --count;
                    // one frame has been already processed while calling mfimportStreamSeek
                    if(count <= 0)
                        end = true;
                    printf("\rProcessed %u%% .. ", ((length-count) * 100) / length);
                }
                else 
                {
                    printf("\rProcessed %u%% .. ", (currentFrame - startPos) * 100 / (pMFI->totalFrames - startPos - 1));
                }
            }
            printf("\rProcessed 100%%          ");
        }
    }
    else
    {
        //////////////////    seek by AUDIO


        int64_t stepSample = pMFI->audioRate;

        printf("\nTotal samples = %d (%0.3f sec), samples per seccond = %d, bits per sample = %d, channels = %d\n",
            (int32_t)pMFI->totalSamples64, (double)pMFI->totalSamples64 / (double)pMFI->audioRate, pMFI->audioRate, pMFI->bytesPerSample / pMFI->channels * 8, pMFI->channels);

        sample_bfr_size = (int32_t)(pMFI->bytesPerSample * stepSample);
        sample_bfr = (uint8_t*)malloc(sample_bfr_size);
        if (!sample_bfr)
        {
            printf("Unable to allocate memory for a sample buffer.\n");
            goto err_exit;
        }

        // reset the parse settings
        int64_t CurrentAbsolutPosition = 0;
        int currentSeekStringPosition = 0;
        absolute = false;
        startPos = 0;
        length = 0;
        while ((currentSeekStringPosition = parse_seek_string(seekStr,
            maxLen,
            currentSeekStringPosition,
            absolute,
            startPos,
            length)) > 0)
        {
            if (length < 0) {
                length = (int32_t)pMFI->totalSamples64;
            }
            if (absolute) {
                if (startPos + length > pMFI->totalSamples64) {
                    if (pMFI->totalSamples64 < startPos) {
                        printf("Seek interval (absolute) %d:%d exeeds the maximum audio samples (%d). It is rejected.\n", startPos, length, (int32_t)pMFI->totalSamples64);
                        continue;
                    }
                    length = (int32_t)(pMFI->totalSamples64 - startPos);
                }
                CurrentAbsolutPosition = startPos;
                if (mpegInSeekSample64(pMFI, 0, CurrentAbsolutPosition, 0))
                {
                    printf("mpegInSeekSample64 failed.\n");
                    goto err_exit;
                }
            }
            else {
                if (CurrentAbsolutPosition + startPos + length > pMFI->totalSamples64) {
                    if (pMFI->totalSamples64 < CurrentAbsolutPosition + startPos) {
                        printf("Seek interval (relative): %d:%d exeeds the maximum audio samples (%d). Current position: %d. It is rejected.\n", startPos, length, (int32_t)pMFI->totalSamples64, (int32_t)CurrentAbsolutPosition);
                        continue;
                    }
                    length = (int32_t)(pMFI->totalSamples64 - (CurrentAbsolutPosition + startPos));
                }
                CurrentAbsolutPosition += startPos;
                if (mpegInSeekSample64(pMFI, 0, CurrentAbsolutPosition, 0)) {
                    printf("mpegInSeekSample64 failed.\n");
                    goto err_exit;
                }
            }
            if (CurrentAbsolutPosition + length > pMFI->totalSamples64) {
                printf("Adjust seek interval length: %d->%d.\n", length, (int32_t)(pMFI->totalSamples64 - CurrentAbsolutPosition));
                length = (int32_t)(pMFI->totalSamples64 - CurrentAbsolutPosition);
            }
            printf("Seek interval %d:%d\n", (int32_t)CurrentAbsolutPosition, length);

            for (int32_t TotalSamplesPerSeek = 0; TotalSamplesPerSeek < length; ) {

                int32_t Samples2Get = (int32_t)stepSample;
                if (TotalSamplesPerSeek + Samples2Get > length) {
                    Samples2Get = length - TotalSamplesPerSeek;
                }

                // get the next block of samples
                int32_t SamplesReceived = mpegInGetNextSample(pMFI, sample_bfr, Samples2Get);
                if (SamplesReceived != Samples2Get)
                {
                    printf("mpegInGetNextSample could not retrieve the next block of samples.\n");
                    goto err_exit;
                }

                TotalSamplesPerSeek += SamplesReceived;
                CurrentAbsolutPosition += SamplesReceived;
                printf("\rProcessed time %0.3f sec, dumped %d samples", (double)CurrentAbsolutPosition / (double)pMFI->audioRate, TotalSamplesPerSeek);
                fflush(stdout);

                // write the samples out
                if (write_wav_header == 1) {
                    write_wav_header = -1;
                    wav_hdr_param wav_hdr = { 0 };
                    wav_header_write(fp_out, &wav_hdr);
                }
                if (fwrite(sample_bfr, 1, SamplesReceived * pMFI->bytesPerSample, fp_out) != SamplesReceived * pMFI->bytesPerSample)
                {
                    printf("Unable to write to output file.\n");
                    goto err_exit;
                }
                total_audio_data_size += SamplesReceived * pMFI->bytesPerSample;


            }
            printf("\n");
        }
    }

    if (write_wav_header == -1) {
        wav_hdr_param wav_hdr = { 0 };

        wav_hdr.bits_per_sample = 16;
        wav_hdr.sample_rate = pMFI->audioRate;
        wav_hdr.num_channels = pMFI->channels;
        wav_hdr.block_align = (16 >> 3) * pMFI->channels;
        wav_hdr.bytes_per_sec = pMFI->audioRate * wav_hdr.block_align;
        wav_hdr.data_size = total_audio_data_size;
        wav_header_write(fp_out, &wav_hdr);
    }

    printf("\n");

    ret = 0;

err_exit:

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

    int32_t iRet, iArgc = 0;
    char* ppArgv[128];

    for ( int32_t i = 0; i < argc; i++ ) {

        if ( strcmp(argv[i], "-old") == 0 ) {
            USE_MFIMPORT_API = false;
        } else {
            ppArgv[iArgc] = argv[i];
            iArgc++;
        }
    };
    int64_t iTime = GetCurrentTimeInMilliseconds();
    if ( USE_MFIMPORT_API ) {
        iRet = main_new_api(iArgc, ppArgv);
    } else {
        iRet = main_old_api(iArgc, ppArgv);
    }
    printf("Execution time: %d ms\n", (uint32_t)(GetCurrentTimeInMilliseconds() - iTime));
    return iRet;
}
