
/* ----------------------------------------------------------------------------
 * File: sample_demux_mp2_growing_file.cpp
 * Desc: command-line sample for demultiplexing with the MP2 demuxer
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

#include "demux_mp2.h"
#include "buf_file.h"
#include "auxinfo.h"
#include "sample_common_misc.h"

#if defined(_WIN32)

#include <Windows.h>
#include <process.h>

#define mc_beginthread(_func_, _args_) {_beginthread(_func_,0,_args_);}
#define mc_sleep(_args_){Sleep(_args_);}

#else 

#include <unistd.h>
#include <pthread.h>

#define mc_beginthread(_func_, _args_) { pthread_t t; pthread_create(&t, NULL, _func_, _args_);}
#define mc_sleep(_args_){usleep(_args_ * 1000);}

#endif


#define MAX_PROGRAMS        32
#define MAX_STREAMS         32    // the max streams we will handle


struct stream_info_struct
{
    mp2dmux_stream_settings_t set;
    mp2dmux_stream_format_t stream_format;
  
    char stream_ext[5];
    int32_t program_idx;
    int32_t stream_idx;

    int64_t total_demuxed;
};


struct program_info_struct
{
    mp2dmux_program_info_t prog_info;
    int32_t program_idx;
    stream_info_struct streams[MAX_STREAMS];
};


struct demuxer_vars
{
    bool detect_only;

    char *input_filename;
    uint64_t input_file_size;
    char output_base_name[2048];

    mp2dmux_tt *demuxer;
    mp2dmux_settings_t dmux_set;
    mp2dmux_file_info_t file_info;
    mp2dmux_program_info_t program_info;
    mp2dmux_parser_settings_t parser_set;
    int32_t parser;

    uint64_t seek_pos; 

    int32_t program_count;
    program_info_struct programs[MAX_STREAMS];

    int64_t curr_processed;

    bool demuxing_error;
    bool demuxing_done;

    bool index_mode;

    // copy
    char growing_file_name[2048];
    FILE *org_file;
    FILE *copy_file;
    uint8_t *copy_buf;
    int32_t copy_chunk_size; 
    int32_t copy_delay_ms;
    bool filecopy_done;

    bool copy_only; // only copy source file, don't attempt to demux target
};



#define MUX_START_DELAY 2000

#ifdef _WIN32
void FileCopyThread(void * pParams)
#else
void* FileCopyThread(void * pParams)
#endif
{
    demuxer_vars *demux_vars = (demuxer_vars*)pParams;

    bool bComplete = false;

    int32_t count_pause = 1;

    uint64_t data_size = demux_vars->input_file_size;
    int32_t to_copy = demux_vars->copy_chunk_size;

    //wait a bit before copying any data to stress the demuxer which wants to open the file immediately 
    printf ("\nFile Copy - wait %d ms before starting to copy data. \n\n", MUX_START_DELAY);
    mc_sleep(MUX_START_DELAY);

    printf ("\nStarting file copy.\n\n");

    while ( data_size ) 
    {
        to_copy = demux_vars->copy_chunk_size;
        if(to_copy > data_size)
            to_copy = (int32_t)data_size;

        if(!fread(demux_vars->copy_buf, 1, to_copy, demux_vars->org_file))
        {
            printf ("File Copy - Failed to read %d bytes\n", to_copy);
            break;
        }

        if(!fwrite(demux_vars->copy_buf, 1, to_copy, demux_vars->copy_file))
        {
            printf ("File Copy - Failed to write %d bytes\n", to_copy);
            break;
        }

        fflush(demux_vars->copy_file);

        data_size -= to_copy;

        if (!demux_vars->copy_only && (demux_vars->demuxing_error || demux_vars->demuxing_done))
            break;

        mc_sleep(demux_vars->copy_delay_ms);
    };

    printf ("File Copy - Thread finished\n");

    demux_vars->filecopy_done = true;

#ifdef _WIN32
#else
    return 0;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Demuxing
//

static void free_demuxer_data(demuxer_vars *demux_vars)
{
    if (demux_vars->parser)
    {
        mp2DemuxFreeParser(demux_vars->demuxer, demux_vars->parser);
        demux_vars->parser = 0;
    }

    if(demux_vars->demuxer)
    {
        mp2DemuxFree(demux_vars->demuxer);
        demux_vars->demuxer = NULL;
    }

    for(int32_t i = 0; i < demux_vars->program_count; i++)
    {
        for (int j = 0; j < demux_vars->programs[i].prog_info.stream_count; j++)
        {
            if (demux_vars->programs[i].streams[j].set.bs)
            {
                int64_t bytes_written = 0;
                demux_vars->programs[i].streams[j].set.bs->auxinfo(demux_vars->programs[i].streams[j].set.bs, 0, BYTECOUNT_INFO, &bytes_written, sizeof(bytes_written));
                printf("  demuxer Program %d Stream ID %d demuxed %d bytes\n", j, demux_vars->programs[i].streams[j].stream_format.ID, (int32_t)bytes_written);
                close_file_buf(demux_vars->programs[i].streams[j].set.bs, 0);
                demux_vars->programs[i].streams[j].set.bs = NULL;
            }
        }
    }
}


static int32_t init_demuxer_data(demuxer_vars *demux_vars)
{
    demux_vars->dmux_set.is_growing_file = 1; // tell the MP2 demuxer to treat file as a growing file.
    if (demux_vars->index_mode)
        demux_vars->dmux_set.index_mode_flag = 1;

    callbacks_t callbacks;
    init_callbacks(callbacks);
    demux_vars->demuxer = mp2DemuxNew(&callbacks, NULL, &demux_vars->dmux_set);
    if (!demux_vars->demuxer)
    {
        printf("  demuxer - Unable create demuxer instance\n");
        return 1;
    }

    char *ptr;
    ptr = strrchr(demux_vars->input_filename, '.');
    if(ptr)
        strncpy(demux_vars->output_base_name, demux_vars->input_filename, ptr - demux_vars->input_filename);
    else
        strncpy(demux_vars->output_base_name, demux_vars->input_filename, strlen(demux_vars->output_base_name));

    return 0;
}


static int32_t demux_detect_contents(demuxer_vars *vars)
{
    mc_time_code_t  time_code;
    memset(&time_code, 0, sizeof(mc_time_code_t));

    printf("\nContainer contents:\n");
    
    if(mp2DemuxGetFileInfo(vars->demuxer, &vars->file_info))
    {
        printf("Unable to get file info\n");
        vars->demuxing_error = true;
        return 1;
    }

    vars->program_count = vars->file_info.program_count;
    if (vars->program_count > MAX_PROGRAMS)
        vars->program_count = MAX_PROGRAMS;

    printf("\n  Programs: %d \n\n", vars->file_info.program_count);

    for(int32_t i = 0; i < vars->file_info.program_count; i++)
    {
        printf("  Program %d:\n", i);
    
        if(mp2DemuxGetProgramInfo(vars->demuxer, &vars->programs[i].prog_info, i))
        {
            printf("Unable to get info for program %d\n", i);
            vars->demuxing_error = true;
            return 1;
        }

        vars->programs[i].program_idx = i;

        for (int32_t j = 0; j < vars->programs[i].prog_info.stream_count; j++)
        {
            if (mp2DemuxGetStreamFormat(vars->demuxer, &vars->programs[i].streams[j].stream_format, i, j))
            {
                printf("Unable to get stream format for stream %d of program %d\n", j, i);
                vars->demuxing_error = true;
                return 1;
            }

            vars->programs[i].streams[j].program_idx = vars->programs[i].program_idx;
            vars->programs[i].streams[j].stream_idx = j;
    
            char stream_type[100] = "";

            switch(vars->programs[i].streams[j].stream_format.format.stream_mediatype)
            {
            case mctMPEG2V:       sprintf(stream_type, "MPEG-2 video stream");    sprintf(vars->programs[i].streams[j].stream_ext, "m2v"); break;
            case mctMPEG4V:       sprintf(stream_type, "MPEG-4 video stream");    sprintf(vars->programs[i].streams[j].stream_ext, "m4v"); break;
            case mctVC1:          sprintf(stream_type, "VC1 video stream");       sprintf(vars->programs[i].streams[j].stream_ext, "vc1"); break;
            case mctH264:         sprintf(stream_type, "AVC video stream");       sprintf(vars->programs[i].streams[j].stream_ext, "avc"); break;
            case mctHEVC:         sprintf(stream_type, "HEVC video stream");      sprintf(vars->programs[i].streams[j].stream_ext, "hevc"); break;
            case mctHDMV_LPCM:    sprintf(stream_type, "HDMV LPCM audio");        sprintf(vars->programs[i].streams[j].stream_ext, "pcm"); break;
            case mctDVD_LPCM:     sprintf(stream_type, "AES3 302M LPCM audio");   sprintf(vars->programs[i].streams[j].stream_ext, "pcm"); break;
            case mctAES3_302M:    sprintf(stream_type, "PCM audio stream");       sprintf(vars->programs[i].streams[j].stream_ext, "pcm"); break;
            case mctMPEG1A:
            case mctMPEG2A:
                sprintf(stream_type, "MPEG-1 audio stream");    sprintf(vars->programs[i].streams[j].stream_ext, "m1a"); break;
            case mctAAC_Generic:
            case mctAAC_RAW:
            case mctAAC_LATM:
            case mctAAC_ADTS:
                sprintf(stream_type, "AAC audio stream");       sprintf(vars->programs[i].streams[j].stream_ext, "aac"); break;
            case mctAC3: 
            case mctAC3_DDPlus:
                sprintf(stream_type, "AC3 audio stream");       sprintf(vars->programs[i].streams[j].stream_ext, "ac3"); break;
            default:  
                sprintf(stream_type, "Unknown stream type %d", vars->programs[i].streams[j].stream_format.format.stream_mediatype); 
                sprintf(vars->programs[i].streams[j].stream_ext, "dat");
            }

            if(vars->file_info.system_stream_flag == STREAM_MPEG2_TRANSPORT)
                printf("    Stream PID %u : %s, duration %dms \n", vars->programs[i].streams[j].stream_format.elementary_PID, stream_type, (int32_t)(vars->programs[i].streams[j].stream_format.duration / 27000.0));
            else
                printf("    Stream ID %u : %s, duration %dms \n", vars->programs[i].streams[j].stream_format.ID, stream_type, (int32_t)(vars->programs[i].streams[j].stream_format.duration / 27000.0));
        }
    }

    printf("\n");

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
void DemuxThread(void * pParams)
#else
void* DemuxThread(void * pParams)
#endif
{
#define WAIT_FILE_MS   200     // Milliseconds to wait until the attempt to open the file is repeated.
#define MAX_TRIES      300      // Number of tries to open the growing file, 60s

#define EOS_MAX_WAIT   10000   // 10 sec

    int64_t prg_next = 0;
    int64_t prg_mod = 65536; //1024 * 1024; //* 5;

    int32_t wait_count = 0;
    int32_t ret = 0;

    int32_t eos_wait_count = 0;
    uint8_t eos_sent = 0;

    demuxer_vars *demux_vars = (demuxer_vars*)pParams;

    printf("Initialize demuxer\n\n");
    // create demuxer and set a timeout for growing files opening and parsing
    if(init_demuxer_data(demux_vars))
        goto exit;

    printf(" demuxer thread - Try to open the file.\n\n");

    // wait for the file to become available and the demuxer having detected the streams.
    while (!demux_vars->filecopy_done && (wait_count < MAX_TRIES))
    {
        ret = mp2DemuxOpen(demux_vars->demuxer, NULL, demux_vars->growing_file_name);
        if (ret == 2) // end of file streams could not be detected.
        {
            printf("  demuxer thread - mp2DemuxOpen - not enough data, attempt %d, waiting %d ms...\n", wait_count, WAIT_FILE_MS);
            mc_sleep(WAIT_FILE_MS);
            wait_count++;
        }
        else
        {
            if (ret == 1)
                printf("  demuxer - demuxer returned an error opening the file\n");
            break;
        }
    }

    if(ret != 0 )
    {
        printf("  demuxer - Unable to open input file after %d attempts over %.2f seconds\n", wait_count, (double)(wait_count * WAIT_FILE_MS) / 1000.0);
        goto exit;
    }

    printf("\n  demuxer - The file is opened :-)\n");

    // see what's in the file
    demux_detect_contents(demux_vars);

    //
    // prepare demuxing 
    //

    demux_vars->parser = mp2DemuxNewParser(demux_vars->demuxer, &demux_vars->parser_set);
    if (!demux_vars->parser)
    {
        printf(" demuxer - Unable to create new parser\n");
        demux_vars->demuxing_error = true;
        goto exit;
    }

    printf("\n  demuxer - creating elementary stream outputs.\n");
    // create outputs 
    for(int32_t i = 0; i < demux_vars->program_count; i++)
    {
        for (int32_t j = 0; j < demux_vars->programs[i].prog_info.stream_count; j++)
        {
            int32_t sid = (demux_vars->file_info.system_stream_flag == STREAM_MPEG2_TRANSPORT)? demux_vars->programs[i].streams[j].stream_format.elementary_PID:demux_vars->programs[i].streams[j].stream_format.ID; 
            char stream_filename[2014];
            sprintf(stream_filename,"%s_tid%02d_sid-%02d.%s", demux_vars->output_base_name, 
                                                            demux_vars->programs[i].program_idx,
                                                            sid,
                                                            demux_vars->programs[i].streams[j].stream_ext);
    
            demux_vars->programs[i].streams[j].set.bs = open_file_buf_write_flushable(stream_filename, 65536, NULL);
            if(!demux_vars->programs[i].streams[j].set.bs)
            {
                printf(" demuxer - Unable to create output bufstream for program %d stream %d\n", demux_vars->programs[i].program_idx, sid);
                demux_vars->demuxing_error = true;
                goto exit;
            }

            demux_vars->programs[i].streams[j].set.stream_idx = demux_vars->programs[i].streams[j].stream_idx;
            demux_vars->programs[i].streams[j].set.program_idx = demux_vars->programs[i].program_idx;

            if (mp2DemuxAddStream(demux_vars->demuxer, demux_vars->parser, &demux_vars->programs[i].streams[j].set))
            {
                printf(" demuxer - Unable to add output stream for program %d stream %d\n", demux_vars->programs[i].program_idx, sid);
                demux_vars->demuxing_error = true;
                goto exit;
            }
        }
    }

    
    // seek to file begin
    mp2DemuxSeekPos(demux_vars->demuxer, demux_vars->parser, 0);

    printf("  Start demuxing\n");

    while (true)
    {
        // demux some data
        ret = mp2DemuxPush(demux_vars->demuxer, demux_vars->parser);
        if(ret == 0)// Reached the real end of the file
        {
            printf("  demuxer - mp2DemuxPush returned: EOF\n");
            break;
        }
        else if (ret > 0) // got some data
        {
            eos_wait_count = 0;

            //printf("  demuxer - mp2DemuxPush returned: %d\n", ret );

            demux_vars->curr_processed += ret;
     
            if(demux_vars->curr_processed > prg_next)
            {
                printf(" Current demuxed  %d \n", (int32_t)demux_vars->curr_processed); 
                for (int32_t i = 0; i < demux_vars->program_count; i++)
                {
                    for (int32_t j = 0; j < demux_vars->programs[i].prog_info.stream_count; j++)
                    {
                        int64_t old_duration = demux_vars->programs[i].streams[j].stream_format.duration;
                        // get updated duration 
                        if(0 == mp2DemuxGetStreamFormat(demux_vars->demuxer, &demux_vars->programs[i].streams[j].stream_format, i, j))
                        {
                            int32_t sid = (demux_vars->file_info.system_stream_flag == STREAM_MPEG2_TRANSPORT)? demux_vars->programs[i].streams[j].stream_format.elementary_PID:demux_vars->programs[i].streams[j].stream_format.ID;
                            if (old_duration != demux_vars->programs[i].streams[j].stream_format.duration)
                                printf("  Program %d Stream ID %d :  current duration %dms \n", i, sid, (int32_t)(demux_vars->programs[i].streams[j].stream_format.duration / 27000.0));
                        }
                    }
                }
                prg_next += prg_mod;
            }
        }
        else if (ret == -2) // Not enough data in file to continue parsing
        {
            if (eos_sent)
                break;    // all done

            printf("  demuxer - mp2DemuxPush returned: Need more Data\n");
            if (demux_vars->filecopy_done)
            {
                printf("  demuxer - sending EOS as filecopy is done\n");
                mp2DemuxUpdateFilesize(demux_vars->demuxer, demux_vars->parser, -1);
                eos_sent = 1;
                continue;
            }
            else if(eos_wait_count * 100  >= EOS_MAX_WAIT)
            {
                printf("  demuxer - No file update for %d seconds - give up\n", eos_wait_count * 100 / 1000);
                // Tell demuxer to reset growing file mode 
                mp2DemuxUpdateFilesize(demux_vars->demuxer, demux_vars->parser, -1);
                eos_sent = 1;
                continue;
            }
            mc_sleep(100);
            eos_wait_count++;
            mp2DemuxUpdateFilesize(demux_vars->demuxer, demux_vars->parser, 0);
        }
        else
        {
            printf("  demuxer - mp2DemuxPush returned: Error\n");
            demux_vars->demuxing_error = true;
            break;
        }
    }

exit:
    printf("\n");
    printf("demuxing done\n");

    free_demuxer_data(demux_vars);
    demux_vars->demuxing_done = true;

    printf("demuxer thread leaving\n");

#ifdef _WIN32
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


static void show_usage()
{
    printf("\n");
    printf("********************************************************************************\n\n");
    printf("This sample application demonstrates the MP2 demultiplexer's\n");
    printf("'growing files' support.\n");
    printf("It will slowly copy the input file to simulate a growing file \n");
    printf("and demultiplex the copied file simultaneously\n\n");
    printf("Usage:\n\n"); 
    printf("sample_demux_mp2_growing_file -i <input file> <options> \n\n");
    printf("<options> is one of the following:\n\n");
    printf("\t-s Size in bytes of chunks to copy. (Default 188 * 10)\n");
    printf("\t-d Delay between two copied chunks in msec. (Default 10)\n");
    printf("\t-c Slow copy only, don't demultiplex\n");
    printf("\t-x Force index mode\n");
    
    // TODO - options for copy speed, chunk size ...
}


static bool parse_cmd(demuxer_vars *demux_vars, int32_t argc, char** argv)
{
    mcmediatypes_t  stream_mediatype = mctUnknown;

    for (int i = 1; i < argc; i++)
    {
        if (*(argv[i]) == '-')
        {
            char* input = argv[i] + 1;

            switch (*input)
            {
            case 'i':
                demux_vars->input_filename  = input + 2;
                break;

            case 'c':
                demux_vars->copy_only = true;
                break;

            case 'd':
                demux_vars->copy_delay_ms = atoi(input + 2);
                break;

            case 's':
                demux_vars->copy_chunk_size = atoi(input + 2);
                break;

            case 'x':
                demux_vars->index_mode = true;
                break;

            default:
                printf ("\nError: Unknown input parameter %s!\n", input);
                return false;
            }
        }
    }

    return true;
}


int main(int argc, char *argv[])
{
    demuxer_vars demux_vars;

    memset(&demux_vars, 0, sizeof(demuxer_vars));

    // default: copy the equivalent of 10 TS packs every 10 msec. 
    demux_vars.copy_chunk_size = 188 * 10;
    demux_vars.copy_delay_ms = 10;

    if (argc < 2) 
    {    
        show_usage();
        return 1;
    }

    if ( !parse_cmd( &demux_vars, argc, argv) ) 
    {
        show_usage();
        return 1;
    }

    // get file size, note does not handle files >= 4GB
	struct stat statbuf;
    stat(demux_vars.input_filename, &statbuf);
    demux_vars.input_file_size = statbuf.st_size;
    if(demux_vars.input_file_size < 188)
    {
        printf("\nInvalid input file size\n");
        return 1;
    }

    char *ptr;
    char ext[32];
    strcpy(ext, ".mpeg");
    ptr = strrchr(demux_vars.input_filename, '.');
    if (ptr)
    {
        strncpy(demux_vars.output_base_name, demux_vars.input_filename, ptr - demux_vars.input_filename);
        if (strlen(ptr) < sizeof(ext))
            strcpy(ext, ptr);   // keep the extension if present, h263 elementary stream
                                // is only detected if .263 or .h263 extension
    }
    else
        strncpy(demux_vars.output_base_name, demux_vars.input_filename, strlen(demux_vars.output_base_name));

    // create copy file name
    strcpy(demux_vars.growing_file_name, demux_vars.output_base_name);
    strcat(demux_vars.growing_file_name, "_copy");
    strcat(demux_vars.growing_file_name, ext);

    demux_vars.org_file = fopen(demux_vars.input_filename, "rb");
    if(!demux_vars.org_file)
    {
        printf("\nFailed to open source file\n");
        return 1;
    }

    // delete it first
    remove(demux_vars.growing_file_name);

    demux_vars.copy_file = fopen(demux_vars.growing_file_name, "w+bc");
    if(!demux_vars.copy_file)
    {
        printf("\nFailed to open file for copying\n");
        return 1;
    }

    demux_vars.copy_buf = (uint8_t*)malloc(demux_vars.copy_chunk_size);


    // run file copy in a thread;
    mc_beginthread(FileCopyThread, &demux_vars );

    if(!demux_vars.copy_only)
    {
        // run demuxer in a simultaneous thread
        mc_beginthread(DemuxThread, &demux_vars );
    }
    else
        demux_vars.demuxing_done = true;
  
  
    // wait for the threads to finish
    while ( !(demux_vars.filecopy_done && demux_vars.demuxing_done)) 
    {
        mc_sleep(100);
    };


    fclose(demux_vars.org_file);
    fclose(demux_vars.copy_file);
    free(demux_vars.copy_buf);

    printf("\nFinished\n");


    return 0;
}
