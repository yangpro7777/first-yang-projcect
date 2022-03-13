/********************************************************************
 Created: 2007/06/07
 File name: sample_demux_mp2_file.cpp
 Purpose: command-line sample for MPEG-2 demuxer in file mode

 Copyright (c) 2007-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#include "mctypes.h"
#include "buf_file.h"
#include "buf_wave.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "demux_mp2.h"

bool USE_MP2_API = true;

typedef struct app_vars_s
{
    char *in_file;
    char *out_file;
    int32_t tid;
    int32_t sid;
    int32_t parser;
    int32_t create_index;
    bufstream_tt *out_bs;
    bufstream_tt *idx_bs;
} app_vars_t;



static int32_t prompt_index_mode(context_t context)
{
    return 1;  // 0 = no, 1 = yes
}


int32_t scte35_callback_ex(void *PSI_app_ptr, mp2dmux_TS_packet_info_t *pInfo)
{
    app_vars_t *pVars = (app_vars_t*)PSI_app_ptr;

    // write the SCTE35 data to the output file
    pVars->out_bs->copybytes(pVars->out_bs, (uint8_t*)pInfo->buf, pInfo->buf_len);

    return 0; // return non-zero to abort demuxing
}


void print_usage() {

    printf("\n==== MainConcept MPEG-2 Demuxer file sample ====\n"
        "Usage:\nsample_demux_mp2_file -i <filename> [-tid <id> -sid <id>] -o <filename> [-idx] [-find_stream_limit <amount in MiB>] [-old]\n"
        " -i <filename>     input filename\n"
        " -o <filename>     output filename\n"
        " -tid <id>         title id\n"
        " -sid <id>         stream id\n"
        " -idx              create index file (only for new API)\n"
        " -find_stream_limit <amount> (only for new API)\n"
        "                   limit parsing to <amount> MiB of the input\n"
        "                   file. If insufficient data is found, only \n"
        "                   PMT will be used for stream information.\n"
        " -old              use the demuxer's old API\n"
        "Use -tid, -sid and -o to demux a particular stream.  Otherwise just\n"
        "print information about the streams in the input file\n"
        "\n"
    );
}

#define IDS_FIND_STREAM_LIMIT      (IDC_CUSTOM_START_ID + 1)

int main_new_api(int argc, char * argv[])
{
    mp2dmux_settings_t demuxer_set;
    mp2dmux_parser_settings_t parser_set;
    mp2dmux_stream_settings_t stream_set;
    mp2dmux_program_info_t program_info;
    mp2dmux_file_info_t file_info;
    mp2dmux_stream_format_t stream_info;
    mp2dmux_tt *demuxer;
    int32_t i, j, ret = 1;
    int64_t byte_cnt, prg_mod, prg_next;
    app_vars_t vars;
    int32_t find_stream_limit;
    char *custom_arg(NULL);

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE,    1,  &vars.in_file},
        { IDI_C_TITLE_ID,    0,  &vars.tid},
        { IDI_C_STREAM_ID,   0,  &vars.sid},
        { IDS_OUTPUT_FILE,   0,  &vars.out_file},
        { IDS_CUSTOM_ARG,    0,  &custom_arg},
        //custom args
        { IDS_FIND_STREAM_LIMIT,        0, &find_stream_limit}
    };

    static const arg_item_desc_t custom_args[] =
    {
        { IDS_FIND_STREAM_LIMIT, { "find_stream_limit", 0 }, ItemTypeInt, 1, "find stream limit" },
    };

    memset(&vars, 0, sizeof(app_vars_t));

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params, custom_args, sizeof(custom_args) / sizeof(custom_args[0])) < 0)
    {
        print_usage();
        return 0;
    }

    if (!vars.in_file)
    {
        printf("Please define input file (-i stream.in)\n");
        return 0;
    }

    enum { BUFFER_SIZE = 256 };
    char input_path_buf[BUFFER_SIZE];
    vars.in_file = path2real(vars.in_file, input_path_buf, BUFFER_SIZE);

    char output_path_buf[BUFFER_SIZE];
    vars.out_file = path2real(vars.out_file, output_path_buf, BUFFER_SIZE);

    if(custom_arg) {
        if (!strcmp(custom_arg,"-idx")) {
            vars.create_index = 1;
        }
        else {
            print_usage();
            return 0;
        }
    }

    // create a demuxer
    memset(&demuxer_set, 0, sizeof(mp2dmux_settings_t));

    if (find_stream_limit >= 0)
        demuxer_set.find_stream_limit = find_stream_limit;

    callbacks_t callbacks;
    init_callbacks(callbacks);
    callbacks_demux_mp2_t demux_callbacks = {0};
    demux_callbacks.prompt_index_mode = prompt_index_mode;

    demuxer = mp2DemuxNew(&callbacks, &demux_callbacks, &demuxer_set);
    if (demuxer == NULL)
    {
        printf("Unable to create a demuxer instance.\n");
        goto err_exit;
    }

    // open the input file
    if (mp2DemuxOpen(demuxer, NULL, vars.in_file))
        goto err_exit;

    // get info about the input file
    if (mp2DemuxGetFileInfo(demuxer, &file_info))
        goto err_exit;

    if (vars.tid == ITEM_NOT_INIT || vars.sid == ITEM_NOT_INIT || !vars.out_file)
    {
        // just display the stream info and exit
        printf("Number of titles: %d\n\n", file_info.program_count);

        for (i = 0; i < file_info.program_count; i++)
        {
            // get info about the program (title)
            if (mp2DemuxGetProgramInfo(demuxer, &program_info, i))
                goto err_exit;

            printf("  Title: %d\n", i);

            for (j = 0; j < program_info.stream_count; j++)
            {
                char str[256];

                // get info about the stream
                if (mp2DemuxGetStreamFormat(demuxer, &stream_info, i, j))
                    goto err_exit;

                if( file_info.system_stream_flag == STREAM_MPEG2_TRANSPORT)
                    sprintf(str, "    PID: %d, Stream: %d, ", stream_info.elementary_PID, j);
                else
                    sprintf(str, "    Stream: %d, ", j);
        
                switch(stream_info.format.stream_mediatype)
                {
                case mctMPEG1V:
                    strcat(str, "MPEG-1 video");
                    break;
                case mctMPEG2V:
                    strcat(str, "MPEG-2 video");
                    break;
                case mctMPEG4V:
                    strcat(str, "MPEG-4 video");
                    break;
                case mctH263:
                    strcat(str, "H263 video");
                    break;
                case mctH264:
                    strcat(str, "AVC video");
                    break;
                case mctMVCSub:
                    strcat(str, "MVC substream video");
                    break;
                case mctMVCMux:
                    strcat(str, "MVC video");
                    break;
                case mctHEVC:
                    strcat(str, "HEVC video");
                    break;
                case mctVC1:
                    strcat(str, "VC1 video");
                    break;
                case mctJ2K:
                    strcat(str, "J2K video");
                    break;
                case mctMPEG1A:
                case mctMPEG2A:
                    strcat(str, "MPEG audio");
                    break;
                case mctAES3_337M_MPEG1A:
                case mctAES3_337M_MPEG2A:
                    strcat(str, "AES3 SMPTE 337M wrapped MPEG audio");
                    break;
                case mctAAC_Generic:
                case mctAAC_RAW:
                case mctAAC_LATM:
                case mctAAC_ADTS:
                    strcat(str, "AAC audio");
                    break;
                case mctAC3:
                    strcat(str, "Dolby Digital audio");
                    break;
                case mctAES3_337M_AC3:
                    strcat(str, "AES3 SMPTE 337M wrapped Dolby Digital audio");
                    break;
                case mctAC3_DDPlus:
                    strcat(str, "Dolby Digital Plus audio");
                    break;
                case mctAES3_337M_DDPlus:
                    strcat(str, "AES3 SMPTE 337M wrapped Dolby Digital Plus audio");
                    break;
                case mctAES3_337M_DOLBYE:
                    strcat(str, "AES3 SMPTE 337M wrapped Dolby E audio");
                    break;
                case mctAES3_302M:
                    strcat(str, "AES3 SMPTE 302M LPCM audio");
                    break;
                case mctDVD_LPCM:
                    strcat(str, "DVD LPCM audio");
                    break;
                case mctDTS:
                    strcat(str, "DTS audio");
                    break;
                case mctSDDS:
                    strcat(str, "SDDS audio");
                    break;
                case mctDVD_Subtitles:
                    strcat(str, "DVD Subpicture");
                    break;
                case mctHDMV_LPCM:
                    strcat(str, "HDMV LPCM audio");
                    break;
                case mctSCTE35_Data:
                    strcat(str, "SCTE 35 Splice info");
                    break;
                default:
                    if (stream_info.stream_type > 0)
                    {
                        char str1[64];

                        sprintf(str1, "Other, PMT/PSM stream type %d (0x%02X)", stream_info.stream_type, stream_info.stream_type);
                        strcat(str, str1);
                    }
                    else
                        strcat(str, "Other");
                    break;
                }
                if (stream_info.PES_scrambled || stream_info.ts_scrambled)
                    strcat(str, ", scrambled");
                strcat(str, "\n");
                printf(str);
            }

            printf("\n");
        }

        printf("Please define output file (-o stream.out), title identifier (-tid <title_number>) and stream number (-sid <stream number>)\n");

        // free the demuxer and exit
        mp2DemuxFree(demuxer);
        return 0;
    }

    // get info about the stream to demux
    if (mp2DemuxGetStreamFormat(demuxer, &stream_info, vars.tid, vars.sid))
        goto err_exit;

    // setup the output file
    switch (stream_info.format.stream_mediatype)
    {
    case mctAES3_302M:
    case mctDVD_LPCM:
    case mctHDMV_LPCM:
        // wrap the output in wave format
        vars.out_bs = open_wave_buf_write(vars.out_file);
        break;
    default:
        // normal bufstream output
        vars.out_bs = open_file_buf_write(vars.out_file, 65536 + 1, NULL);
    }

    if (!vars.out_bs)
    {
        printf("Unable to create an output bufstream.\n");
        goto err_exit;
    }

    // create a parser
    memset(&parser_set, 0, sizeof(mp2dmux_parser_settings_t));
    vars.parser = mp2DemuxNewParser(demuxer, &parser_set);
    if (vars.parser == 0)
        goto err_exit;

    if (stream_info.format.stream_mediatype == mctSCTE35_Data)
    {
        // use a callback to get the SCTE35 data, the normal mp2DemuxAddStream could be used below as well
        mp2dmux_PSI_callback_settings_t cb_set = {0};
        cb_set.parser_num = vars.parser;
        cb_set.id = stream_info.elementary_PID;
        cb_set.PSI_callback_ex = scte35_callback_ex;
        cb_set.PSI_app_ptr = &vars;

        if (mp2DemuxAddPSICallback(demuxer, &cb_set))
            goto err_exit;
    }
    else
    {
        // add the stream to demux to the parser
        memset(&stream_set, 0, sizeof(mp2dmux_stream_settings_t));

        stream_set.program_idx = vars.tid;
        stream_set.stream_idx = vars.sid;
        stream_set.bs = vars.out_bs;

        // enable this to output an AVC base stream and all associated MVC substreams as a single stream
        //        stream_set.mvc_reassemble_flag = 1;

        // enable this to output the PES packetized stream instead of an elementary stream, mpeg2 TS and PS only
        //        stream_set.output_pes_flag = 1;

        if (mp2DemuxAddStream(demuxer, vars.parser, &stream_set))
            goto err_exit;
    }

    // seek to the beginning of the file
    if (mp2DemuxSeekPos(demuxer, vars.parser, 0))
        goto err_exit;

    byte_cnt = 0;
    prg_next = 0;
    prg_mod = file_info.file_size / 100;
    if (!prg_mod)
        prg_mod = 1;

    // main demuxing loop
    // push data through the demuxer until eof or error
    while (1)
    {
        i = mp2DemuxPush(demuxer, vars.parser);
        if (i < 0)
            goto err_exit;    // an error occurred
        else if (i == 0)
            break;            // end of file
        else
        {
            byte_cnt += i;
            if (byte_cnt >= prg_next)
            {
                printf("\rProcessed %u%% ...", (uint32_t)((double)byte_cnt / (double)file_info.file_size * 100.0));
                prg_next += prg_mod;
            }
        }
    }

    // create index file
    if(vars.create_index)
    {
        if(mp2DemuxCreateIndex(demuxer, 0)) {
            printf("Failed to create index\n");
            goto err_exit;
        }

        // open file bufstream to write index file into
        vars.idx_bs = open_file_buf_write(strcat(vars.in_file, ".inx"), 65536 + 1, NULL);

        if(vars.idx_bs == NULL) {
            printf("Failed to open output index file\n");
            goto err_exit;
        }

        if(mp2DemuxSaveIndex(demuxer, vars.idx_bs)) {
            printf("Failed to save index file\n");
            goto err_exit;
        }
    }

    ret = 0;

err_exit:

    // close everything
    if (demuxer)
        mp2DemuxFree(demuxer);

    if (vars.out_bs)
        close_file_buf(vars.out_bs, 0);

    if (vars.idx_bs)
        close_file_buf(vars.idx_bs, 0);

    return ret;
}

int main_old_api(int argc, char * argv[])
{
    mpgdmux_settings demuxer_set;
    mpgdmux_parser_settings parser_set;
    mpgdmux_stream_settings stream_set;
    mpgdmux_program_info program_info;
    mpgdmux_file_info file_info;
    mpgdmux_stream_info stream_info;
    int32_t i, j, ret = 1;
    int64_t byte_cnt, prg_mod, prg_next;
    app_vars_t vars;
    mpgdmux_tt *demuxer;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE,    1,  &vars.in_file},
        { IDI_C_TITLE_ID,    0,  &vars.tid},
        { IDI_C_STREAM_ID,   0,  &vars.sid},
        { IDS_OUTPUT_FILE,   0,  &vars.out_file}
    };

    memset(&vars, 0, sizeof(app_vars_t));

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params) < 0)
    {
        print_usage();
        return 0;
    }

    // create a demuxer
    memset(&demuxer_set, 0, sizeof(mpgdmux_settings));

    callbacks_t callbacks;
    init_callbacks(callbacks);
    callbacks_demux_mp2_t demux_callbacks = {0};
    demux_callbacks.prompt_index_mode = prompt_index_mode;

    demuxer = mpegDemuxNew(&callbacks, &demux_callbacks, &demuxer_set);
    if (demuxer == NULL)
    {
        printf("Unable to create a demuxer instance.\n");
        goto err_exit;
    }

    // open the input file
    if (mpegDemuxOpen(demuxer, NULL, vars.in_file))
        goto err_exit;

    // get info about the input file
    if (mpegDemuxGetFileInfo(demuxer, &file_info))
        goto err_exit;

    if (argc < 4)
    {
        // just display the stream info and exit
        printf("Number of titles: %d\n\n", file_info.program_count);

        for (i = 0; i < file_info.program_count; i++)
        {
            // get info about the program (title)
            if (mpegDemuxGetProgramInfo(demuxer, &program_info, i))
                goto err_exit;

            printf("  Title: %d\n", i);

            for (j = 0; j < program_info.stream_count; j++)
            {
                char str[256];

                // get info about the stream
                if (mpegDemuxGetStreamInfo(demuxer, &stream_info, i, j))
                    goto err_exit;

                if( file_info.system_stream_flag == STREAM_MPEG2_TRANSPORT)
                    sprintf(str, "    PID: %d, Stream: %d, ", stream_info.elementary_PID, j);
                else
                    sprintf(str, "    Stream: %d, ", j);

                switch(stream_info.sdk_stream_type)
                {
                case MPGDMUX_MPEG_VIDEO:
                    strcat(str, "MPEG/AVC video");
                    break;
                case MPGDMUX_HEVC_VIDEO:
                    strcat(str, "HEVC video");
                    break;
                case MPGDMUX_VC1_VIDEO:
                    strcat(str, "VC1 video");
                    break;
                case MPGDMUX_MPEG_AUDIO:
                    strcat(str, "MPEG audio");
                    break;
                case MPGDMUX_AC3_AUDIO:
                    strcat(str, "Dolby Digital audio");
                    break;
                case MPGDMUX_AES_302M_AUDIO:
                    strcat(str, "AES3 SMPTE 302M LPCM audio");
                    break;
                case MPGDMUX_DVD_LPCM_AUDIO:
                    strcat(str, "DVD LPCM audio");
                    break;
                case MPGDMUX_DTS_AUDIO:
                    strcat(str, "DTS audio");
                    break;
                case MPGDMUX_SDDS_AUDIO:
                    strcat(str, "SDDS audio");
                    break;
                case MPGDMUX_DVD_SUBPIC:
                    strcat(str, "DVD Subpicture");
                    break;
                case MPGDMUX_HDMV_LPCM_AUDIO:                
                    strcat(str, "HDMV LPCM audio");
                    break;
                case MPGDMUX_DVD_PCI:
                    strcat(str, "DVD PCI");
                    break;
                case MPGDMUX_DVD_DSI:
                    strcat(str, "DVD DSI");
                    break;
                case MPGDMUX_EAC3_AUDIO:            
                    strcat(str, "Dolby Digital Plus audio");
                    break;
                case MPGDMUX_AES3_337M_DATA:
                    strcat(str, "AES3 SMPTE 337M data");
                    break;
                default:
                    strcat(str, "Other");
                    break;
                }
                strcat(str, "\n");
                printf(str);
            }

            printf("\n");
        }

        // free the demuxer and exit
        mpegDemuxFree(demuxer);
        return 0;
    }

    if (!vars.out_file)
    {
        printf("Please define output file with    -o stream.out\n");
        goto err_exit;
    }

    // get info about the stream to demux
    if (mpegDemuxGetStreamInfo(demuxer, &stream_info, vars.tid, vars.sid))
        goto err_exit;

    // setup the output file
    switch (stream_info.sdk_stream_type)
    {
    case MPGDMUX_AES_302M_AUDIO:
    case MPGDMUX_DVD_LPCM_AUDIO:
    case MPGDMUX_HDMV_LPCM_AUDIO:
        // wrap the output in wave format
        vars.out_bs = open_wave_buf_write(vars.out_file);
        break;
    default:
        // normal bufstream output
        vars.out_bs = open_file_buf_write(vars.out_file, 65536 + 1, NULL);
    }

    if (!vars.out_bs)
    {
        printf("Unable to create an output bufstream.\n");
        goto err_exit;
    }

    // create a parser
    memset(&parser_set, 0, sizeof(mpgdmux_parser_settings));
    vars.parser = mpegDemuxNewParser(demuxer, &parser_set);
    if (vars.parser == 0)
        goto err_exit;

    // add the stream to demux to the parser
    memset(&stream_set, 0, sizeof(mpgdmux_stream_settings));

    stream_set.program_idx = vars.tid;
    stream_set.stream_idx = vars.sid;
    stream_set.bs = vars.out_bs;

    if (mpegDemuxAddStream(demuxer, vars.parser, &stream_set))
        goto err_exit;

    // seek to the beginning of the file
    if (mpegDemuxSeek(demuxer, vars.parser, 0))
        goto err_exit;

    byte_cnt = 0;
    prg_next = 0;
    prg_mod = file_info.file_size / 100;
    if (!prg_mod)
        prg_mod = 1;

    // main demuxing loop
    // push data through the demuxer until eof or error
    while (1)
    {
        i = mpegDemuxPush(demuxer, vars.parser);
        if (i < 0)
            goto err_exit;    // an error occurred
        else if (i == 0)
            break;            // end of file
        else
        {
            byte_cnt += i;
            if (byte_cnt >= prg_next)
            {
                printf("\rProcessed %u%% ...", (uint32_t)((double)byte_cnt / (double)file_info.file_size * 100.0));
                prg_next += prg_mod;
            }
        }
    }

    ret = 0;

err_exit:

    // close everything
    if (demuxer)
        mpegDemuxFree(demuxer);

    if (vars.out_bs)
        close_file_buf(vars.out_bs, 0);

    return ret;
}


int32_t main(int32_t argc, char * argv[])
{
    USE_MP2_API = true;

    int32_t iRet, iArgc = 0;
    char* ppArgv[128];

    for ( int32_t i = 0; i < argc; i++ ) {

        if ( strcmp(argv[i], "-old") == 0 ) {
            USE_MP2_API = false;
        } else {
            ppArgv[iArgc] = argv[i];
            iArgc++;
        }
    };

    if ( USE_MP2_API ) {
        iRet = main_new_api(iArgc, ppArgv);
    } else {
        iRet = main_old_api(iArgc, ppArgv);
    }

    return iRet;
}

