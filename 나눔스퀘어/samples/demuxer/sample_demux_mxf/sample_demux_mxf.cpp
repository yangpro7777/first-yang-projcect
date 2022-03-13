
/* ----------------------------------------------------------------------------
 * File: sample_demux_mxf.cpp
 * Desc: command-line sample for MXF demuxer
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
#include <string.h>
#include <stdarg.h>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <unistd.h>
#endif
#if defined(__APPLE__)
#include <mach/mach_time.h>
#else
#include <ctime>
#endif

#include "demux_mxf.h"
#include "auxinfo.h"
#include "buf_file.h"

#include "dm_reader.h"
#include "sample_common_misc.h"

 //#define SHOW_AUX_INFO // catch demuxer auxinfo and print timecode and chunk info

#define MAX_STREAMS		64	// the max streams we will handle

struct global_vars_struct;

enum MXF_SEEK_RESULT
{
    MXF_SEEK_SUCCESSFULL = 0,
    MXF_SEEK_EOF = 1,
    MXF_SEEK_ERROR
};
struct stream_info_struct
{
    mxfdmux_stream_info info;
    mxfdmux_stream_settings set;
    mxfdmux_stream_format_tt stream_format;

    int64_t total_demuxed;

    global_vars_struct *vars;
    int idx;
};


struct stream_to_get_struct
{
    int32_t ID;
    char path[2048];
};

struct global_vars_struct
{
    mxfdmux_tt *demuxer;
    mxfdmux_settings dmux_set;
    mxfdmux_file_info file_info;
    mxfdmux_parser_settings parser_set;
    int32_t parser;

    char input_file[2048];

    int32_t dm_demo;
    uint16_t preview_range;

    int32_t stream_count;
    stream_info_struct streams[MAX_STREAMS];
    mxfdmux_dm_track_info_tt dm_tracks[MAX_STREAMS];

    // streams to demux
    int32_t num_streams_to_get;
    stream_to_get_struct streams_to_get[MAX_STREAMS];

    int64_t prev_processed;
    int64_t curr_processed;
    int64_t seek_position_start;
    int64_t seek_position_stop;

    bool is_growing_file;

    bool print_auxinfo;
};

static int detect_contents(global_vars_struct *vars)
{
    int32_t i, j, k;

    mc_time_code_t  time_code;
    memset(&time_code, 0, sizeof(mc_time_code_t));

    printf("\nContainer contents:\n");

    if (0 == mxfDemuxGetStartTimecode(vars->demuxer, &time_code))
        printf("\n  Start timecode: %.02d:%.02d:%.02d:%.02d\n", time_code.Hour, time_code.Min, time_code.Sec, time_code.Frm);

    if (mxfDemuxGetFileInfo(vars->demuxer, &vars->file_info))
    {
        printf("Unable to get file info\n");
        return 1;
    }

    vars->stream_count = vars->file_info.stream_count;
    if (vars->stream_count > MAX_STREAMS)
        vars->stream_count = MAX_STREAMS;

    printf("\n  Essence streams: %d \n\n", vars->file_info.stream_count);
    for (i = 0; i < vars->stream_count; i++)
    {
        if (mxfDemuxGetStreamFormat(vars->demuxer, &vars->streams[i].stream_format, i))
        {
            printf("Unable to get stream format for stream %d\n", i);
            return 1;
        }

        char stream_type[100] = "";

        switch (vars->streams[i].stream_format.format.stream_mediatype)
        {
            case mctMPEG2V:     sprintf(stream_type, "MPEG-2 video stream");    break;
            case mctMPEG4V:     sprintf(stream_type, "MPEG-4 video stream");    break;
            case mctDV:         sprintf(stream_type, "DV video stream");        break;
            case mctJ2K:        sprintf(stream_type, "JPEG2000 video stream");  break;
            case mctVC3:        sprintf(stream_type, "VC3 video stream");       break;
            case mctH264:       sprintf(stream_type, "AVC video stream");       break;
            case mctAES3_331M:  sprintf(stream_type, "AES 331M audio stream");  break;
            case mctAES3_382M:  sprintf(stream_type, "AES 382M audio stream");  break;
            case mctPCM:        sprintf(stream_type, "PCM audio stream");       break;
            case mctAIFF:       sprintf(stream_type, "AIFF audio stream");      break;
            case mctALAW:       sprintf(stream_type, "A-law audio stream");     break;
            case mctMPEG1A:     sprintf(stream_type, "MPEG-1 audio stream");    break;
            case mctVBI_Data:   sprintf(stream_type, "VBI data stream");        break;
            case mctANC_Data:   sprintf(stream_type, "ANC data stream");        break;
            case mctProRes:   sprintf(stream_type, "ProRes video stream");        break;
            case mctHEVC:   sprintf(stream_type, "HEVC video stream");        break;
            default:  sprintf(stream_type, "Unknown stream");
        }

        printf("    Stream ID %d : Track ID %d, %s, %llu samples", vars->streams[i].stream_format.ID, vars->streams[i].stream_format.track_id, stream_type, vars->streams[i].stream_format.sample_count);
        if (vars->streams[i].stream_format.format.stream_mediatype > mctMinVideoType && vars->streams[i].stream_format.format.stream_mediatype < mctMaxVideoType)
            printf(", AFD 0x%01X", vars->streams[i].stream_format.active_format_descriptor);
        vars->streams[i].idx = i;
        printf("\n");
    }

    printf("\n");

    for (i = 0; i < vars->stream_count; i++)
    {
        printf("    Stream ID %d : positionable: duration %lld, sample count %lld \n",
            vars->streams[i].stream_format.ID,
            vars->streams[i].stream_format.positionable_duration,
            vars->streams[i].stream_format.positionable_sample_count);
    }

    printf("\n");

    printf("\n  Descriptive Metadata Schemes: %d\n\n", vars->file_info.dm_schemes_count);
    for (i = 0; i < vars->file_info.dm_schemes_count; i++)
    {
        char chUL[100] = "   ";
        for (int j = 0; j < 16; j++)
        {
            char chVal[4] = "";
            sprintf(chVal, " %02X", vars->file_info.dm_schemes[i][j]);
            strcat(chUL, chVal);
        }
        strcat(chUL, "\n");
        printf(chUL);
    }

    printf("\n  Descriptive Metadata Framework sets: %d\n\n", vars->file_info.dm_framework_set_count);


    printf("\n  Descriptive Metadata tracks: %d\n", vars->file_info.dm_tracks_count);

    for (i = 0; i < vars->file_info.dm_tracks_count; i++)
    {
        if (mxfDemuxGetDMTrackInfo(vars->demuxer, &vars->dm_tracks[i], i))
        {
            printf("Unable to get info for DM track stream %d\n", i);
            return 1;
        }

        char track_type[25] = "unknown";

        switch (vars->dm_tracks[i].track_type)
        {
            case MXF_TRACK_TYPE_TIMLINE: sprintf(track_type, "Track type: Timeline"); break;
            case MXF_TRACK_TYPE_STATIC: sprintf(track_type, "Track type: Static"); break;
            case MXF_TRACK_TYPE_EVENT: sprintf(track_type, "Track type: Event"); break;
        }

        printf("\n   Track ID %d, %s, Duration %lld, Segments %d\n", vars->dm_tracks[i].track_id, track_type, vars->dm_tracks[i].duration, vars->dm_tracks[i].num_segments);

        if (vars->dm_tracks[i].num_segments)
        {
            for (j = 0; j < vars->dm_tracks[i].num_segments; j++)
            {
                mxfdmux_dm_segment_info_tt *segment = vars->dm_tracks[i].segments + j;

                char ch_track_ids[100] = "all";
                if (segment->num_track_ids)
                {
                    sprintf(ch_track_ids, "");
                    for (k = 0; k < segment->num_track_ids; k++)
                    {
                        uint32_t track_id = *(segment->track_ids + k);
                        char ch_id[10];
                        sprintf(ch_id, " %d,", track_id);
                        strcat(ch_track_ids, ch_id);
                    }
                }

                printf("\n      DM segment %d: Duration %lld, Start Position %lld\n", j, segment->duration, segment->start_position);
                printf("        Refers to essence tracks: %s\n", ch_track_ids);
                print_dm_framework_type(segment->framework_type);
            }

        }
    }

    printf("\n");

    return 0;
}


static uint32_t demux_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
    dmux_chunk_info *chunk_info;
    struct impl_stream* p = bs->Buf_IO_struct;
    uint64_t *ptr;
    mc_time_code_t *time_code_info;
    sample_struct *sample_info;

    stream_info_struct *stream_info = (stream_info_struct*)bs->drive_ptr;

    switch (info_ID)
    {
        case DMUX_CHUNK_INFO:
            chunk_info = (dmux_chunk_info*)info_ptr;
            if (chunk_info && (info_size == sizeof(dmux_chunk_info)))
            {
                stream_info->vars->curr_processed = chunk_info->file_pos;
                stream_info->total_demuxed += chunk_info->length;
                if (stream_info->vars->print_auxinfo) {
                    printf("DMUX_CHUNK_INFO %d:\n", stream_info->idx);
                    // show AES-3 331 header bytes - only sent when mxfdmux_settings->deliver_aes3_331_headers = 1
                    if (chunk_info->info_len && (chunk_info->flags & MCDMUX_FLAGS_AES3_331M))
                    {
                        printf("		AES-3 331 header bytes = 0x%02X%02X%02X%02X\n", chunk_info->info[0], chunk_info->info[1], chunk_info->info[2], chunk_info->info[3]);
                    }
                    printf("		file_pos = %d (0x%08X)\n", chunk_info->file_pos, chunk_info->file_pos);
                    printf("		pack_pos = %d (0x%08X)\n", chunk_info->pack_pos, chunk_info->pack_pos);
                    printf("		length = %d\n", chunk_info->length);
                    printf("		aes3_channel = %d\n", chunk_info->aes3_channel);
                    printf("		aes3_payload_length = %d\n", chunk_info->aes3_payload_length);
                    printf("		PTS = %lld\n", chunk_info->PTS);

                    printf("\n");
                }
            }
            break;

        case SAMPLE_INFO:
            if (stream_info->vars->print_auxinfo) {
                sample_info = (sample_struct*)info_ptr;
                if (sample_info && (info_size == sizeof(sample_struct)))
                {
                    printf("SAMPLE_INFO %d:\n", stream_info->idx);
                    printf("		rtStart = %lld\n", sample_info->rtStart);
                    printf("		rtStop = %lld\n", sample_info->rtStop);
                }
            }
            break;

        case TIMECODE_INFO:
            if (stream_info->vars->print_auxinfo) {
                time_code_info = (mc_time_code_t*)info_ptr;
                if (time_code_info && (info_size == sizeof(mc_time_code_t)))
                {
                    printf("TIMECODE_INFO %d:\n", stream_info->idx);
                    printf("		%.02d:%.02d:%.02d:%.02d\n", time_code_info->Hour, time_code_info->Min, time_code_info->Sec, time_code_info->Frm);
                }
            }
            break;
    }

    return BS_OK;
}


static void show_usage()
{
    printf("\nUsage: \n   sample_demux_mxf  <input filename>  <options> id=filename  id=filename ... \n");
    printf("Where 'id' is the stream ID of the essence stream to demux into 'filename'.\n");
    printf("Run with input filename only to get the list of streams printed.\n\n");
    printf("Options:\n");
    printf("   -pcm Enable PCM output format.\n");
    printf("   -d Enable Descriptive Metadata parser demo.\n");
    printf("   -p Specify preview seek range, e.g. -p 500.\n");
    printf("   -e External index file path\n");
    printf("   -n MXF Demuxer external index mode\n");
    printf("      0 - Build index without saving it to file (default).\n");
    printf("      1 - Try to use external index, build it and save to file otherwise.\n");
    printf("      2 - Use external index only, refuse to open container without index file.\n");
    printf("   -s Seek position start in frame, e.g. -s 10.\n");
    printf("   -t Seek position stop in frame, e.g. -t 1000.\n");
    printf("   -g Process input file as growing\n");
    printf("   -lf License file.\n");
    printf("   -print_auxinfo Print auxinfo.\n");

}

int32_t parse_args(int argc, char *argv[], char *opt_str, char *opt_value, global_vars_struct& global_vars){
    int32_t i(1);
    int32_t num_opts(0);
    char *ptr;
    while (i < argc)
    {
        strcpy(opt_str, argv[i]);
        if (i == 1)
        {
            strcpy(global_vars.input_file, opt_str);
        }
        else if (0 == strcmp(opt_str, "-d"))
        {
            global_vars.dm_demo = 1;
            num_opts++;
        } else if ( 0 == strcmp(opt_str, "-pcm")){
            global_vars.dmux_set.format_pcm_output_flag = 1;
            num_opts++;
        } else if( 0 == strcmp(opt_str, "-p") )
        {
            strcpy(opt_value, argv[++i]);
            global_vars.preview_range = atoi(opt_value);
            num_opts++;
        } else if( 0 == strcmp(opt_str, "-n") )
        {
            strcpy(opt_value, argv[++i]);
            global_vars.dmux_set.index_mode = atoi(opt_value);
            num_opts++;
        }
        else if (0 == strcmp(opt_str, "-e"))
        {
            strcpy(opt_value, argv[++i]);
            global_vars.dmux_set.index_file_path = opt_value;
            num_opts++;
        } else if( 0 == strcmp(opt_str, "-s") )
        {
            strcpy(opt_value, argv[++i]);
            global_vars.seek_position_start = atoi(opt_value);
            num_opts++;
        } else if( 0 == strcmp(opt_str, "-t") )
        {
            strcpy(opt_value, argv[++i]);
            global_vars.seek_position_stop = atoi(opt_value);
            num_opts++;
        }
        else if (0 == strcmp(opt_str, "-lf"))
        {
            license_file_path = argv[++i];
            num_opts++;
        }
        else if (0 == strcmp(opt_str, "-g")) {
            global_vars.is_growing_file = true;
            num_opts++;
        }
        else if (0 == strcmp(opt_str, "-print_auxinfo")) {
            global_vars.print_auxinfo = true;
            num_opts++;
        }
        else
        {
            ptr = strrchr(opt_str, '=');
            if (!ptr)
            {
                show_usage();
                return -1;
            }
            ptr[0] = 0;
            ptr++;

            if (sscanf(opt_str, "%d", &global_vars.streams_to_get[global_vars.num_streams_to_get].ID) != 1)
            {
                show_usage();
                return -1;
            }
            strcpy(global_vars.streams_to_get[global_vars.num_streams_to_get].path, ptr);

            global_vars.num_streams_to_get++;
        }
        i++;
    }
    return 0;
}

void cleanup(global_vars_struct& global_vars){
    if (global_vars.parser)
        mxfDemuxFreeParser(global_vars.demuxer, global_vars.parser);
    if (global_vars.demuxer)
        mxfDemuxFree(global_vars.demuxer);

    printf("\n");

    for (int32_t i = 0; i < MAX_STREAMS; i++)
    {
        if (global_vars.streams[i].set.bs)
        {
            int64_t bytes_written = 0;
            global_vars.streams[i].set.bs->auxinfo(global_vars.streams[i].set.bs, 0, BYTECOUNT_INFO, &bytes_written, sizeof(bytes_written));
            printf("Stream ID %d demuxed %lld bytes\n", global_vars.streams[i].stream_format.ID, global_vars.streams[i].total_demuxed);
            close_file_buf(global_vars.streams[i].set.bs, 0);
        }
    }
    printf("\n");
}

static long long int monotonic_time()
{
#if defined(_WIN32) || defined(_WIN64)
    return static_cast<long long int>(clock() / CLOCKS_PER_SEC);
#elif defined(__APPLE__)
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    return static_cast<long long int>(mach_absolute_time() * info.numer / info.denom / 1000000000);
#else
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return static_cast<long long int>(time.tv_sec);
#endif
}

int main(int argc, char *argv[])
{
    global_vars_struct global_vars = { 0 };

    int32_t stream_num;
    int64_t prg_mod, prg_next;
    char opt_str[2048], opt_value[256];

    memset(&global_vars, 0, sizeof(global_vars_struct));

    /////////////////////////////////////////////////////////////////
    // check arguments
    if (argc < 2)
    {
        show_usage();
        return 1;
    }

    if( parse_args(argc, argv, opt_str, opt_value, global_vars) < 0 ){
        return -1;
    }
    ///////////////////////////////////////////////////////////////////////
    // open input file

    global_vars.dmux_set.parse_external_streams = 1;    // link OP-Atom files
    global_vars.dmux_set.deliver_aes3_331_headers = 1;  // output AES-3 331 audio with element headers
    global_vars.dmux_set.preview_range = global_vars.preview_range;
    global_vars.dmux_set.is_growing_file = global_vars.is_growing_file ? 1 : 0;

    callbacks_t callbacks;
    init_callbacks(callbacks);
    global_vars.demuxer = mxfDemuxNew(&callbacks, NULL, &global_vars.dmux_set);
    if (!global_vars.demuxer)
    {
        printf("Unable create instance of demuxer\n");
        return 1;
    }

    if (mxfDemuxOpen(global_vars.demuxer, NULL, global_vars.input_file))
    {
        printf("Unable to open input file\n");
        cleanup(global_vars);
        return -1;
    }


    ///////////////////////////////////////////////////////////////////////
    // print streams info
    if( detect_contents(&global_vars) ){
        cleanup(global_vars);
        return -1;
    }


    /////////////////////////////////////////////////////////////////////////
    // Demonstrate retrieval and parsing of Descriptive Metadata Frameworks.

    if (global_vars.dm_demo)
        read_descriptive_metadata(global_vars.demuxer, &global_vars.file_info, global_vars.dm_tracks);

    ////////////////////////////////////////////////////////////////


    if (global_vars.num_streams_to_get == 0)
    {
        //we're done
        mxfDemuxFree(global_vars.demuxer);
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    // create parser

    global_vars.parser = mxfDemuxNewParser(global_vars.demuxer, &global_vars.parser_set);
    if (!global_vars.parser)
    {
        printf("Unable to create new parser\n");
        cleanup(global_vars);
        return -1;
    }

    stream_num = -1;

    //////////////////////////////////////////////////////////////////////////
    // create output streams
    {
        int32_t j(0);
        for (int32_t i = 0; i < global_vars.num_streams_to_get; i++)
        {
            // find the ID in the stream list
            bool found = false;
            while (j < global_vars.stream_count)
            {
                if (global_vars.streams_to_get[i].ID == global_vars.streams[j].stream_format.ID)
                {
                    found = true;
                    // this is a stream we want
                    global_vars.streams[j].set.stream_num = j;
                    global_vars.streams[j].set.bs = open_file_buf_write(global_vars.streams_to_get[i].path, 10 * 1024 * 1024, NULL);

                    global_vars.streams[j].vars = &global_vars;

                    global_vars.streams[j].set.bs->drive_ptr = (struct drive_struct*)&global_vars.streams[j];

                    // set custom auxinfo handler to retrieve AES-3 331 header, timestamps, timecodes etc..
                    global_vars.streams[j].set.bs->auxinfo = demux_auxinfo;

                    if (mxfDemuxAddStream(global_vars.demuxer, global_vars.parser, &global_vars.streams[j].set))
                    {
                        printf("Unable to add stream %d\n", global_vars.streams_to_get[i].ID);
                        cleanup(global_vars);
                        return -1;
                    }

                    if (stream_num < 0)
                        stream_num = j;

                    break;
                }
                j++;
            }

            if (!found)
            {
                // stream not found!
                printf("Invalid stream ID %d\n", global_vars.streams_to_get[i].ID);
                show_usage();
                cleanup(global_vars);
                return -1;
            }

        }
    }


    mxfdmux_seek_settings_t seek_set;
    memset(&seek_set, 0, sizeof(mxfdmux_seek_settings_t));

    ///////////////////////////////////////////////////////////////////////////////////
    // seek to end - example

    uint64_t _sample_count = global_vars.streams[stream_num].stream_format.positionable_sample_count;

    if (global_vars.streams[stream_num].stream_format.format.stream_mediatype > mctMinPCMType &&
        global_vars.streams[stream_num].stream_format.format.stream_mediatype < mctMaxPCMType)
    {
        printf("Seek to audio stream end of positionable range\n");
        seek_set.seek_flags = SEEKING_FLAG_BY_TIME | SEEKING_FLAG_RETURN_REF_TIME | SEEKING_FLAG_RETURN_TIME;
        // seek by time
        mc_audio_format *audio_fmt = (mc_audio_format*)global_vars.streams[stream_num].stream_format.format.pFormat;

        double clocks_per_sample = (27000000.0 / (double)audio_fmt->samples_per_sec) - 0.1;
        // position to last sample:
        seek_set.start_pos = (int64_t)(double(_sample_count - 1) * clocks_per_sample);
        if (mxfDemuxSetPositions(global_vars.demuxer, global_vars.parser, &seek_set))
        {
            printf("Unable to seek to end of positionable range\n");
        }
    }
    else if (global_vars.streams[stream_num].stream_format.format.stream_mediatype > mctMinVideoType &&
        global_vars.streams[stream_num].stream_format.format.stream_mediatype < mctMaxVideoType)
    {
        printf("Seek to video stream end of positionable range\n");
        // position to last Frame:

        // seek by frame number
        seek_set.seek_flags = SEEKING_FLAG_BY_FRAME | SEEKING_FLAG_RETURN_REF_FRAME | SEEKING_FLAG_RETURN_TIME;
        seek_set.start_pos = _sample_count - 1;

        // alternatively  seek by time
        //mc_video_format *video_fmt = (mc_video_format*)global_vars.streams[stream_num].stream_format.format.pFormat;
        //double fps = 10000000.0 / (double)video_fmt->avg_time_per_frame;
        //clocks_per_sample = 27000000.0 / fps;
        //seek_set.start_pos = (int64_t)(double (sample_count -1) * clocks_per_sample);

        if (mxfDemuxSetPositions(global_vars.demuxer, global_vars.parser, &seek_set))
        {
            printf("Unable to seek to end of positionable range\n");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // demultiplex

    printf("\nSeek to start position\n");
    seek_set.seek_flags = SEEKING_FLAG_BY_FRAME | SEEKING_FLAG_RETURN_REF_FRAME | SEEKING_FLAG_RETURN_TIME;
  if( global_vars.seek_position_stop == -1 ){
        global_vars.seek_position_stop = global_vars.streams[stream_num].stream_format.index_num_entries;
    }

  if( global_vars.seek_position_start > global_vars.streams[stream_num].stream_format.index_num_entries ) {
        printf("\nWrong seek position!\n");
        return -1;
    }

    seek_set.start_pos = global_vars.seek_position_start;
    seek_set.stop_pos = global_vars.seek_position_stop;

    int32_t _result = mxfDemuxSetPositions(global_vars.demuxer, global_vars.parser, &seek_set);
  if( _result != MXF_SEEK_SUCCESSFULL ){
        printf("\nUnable to seek on start position\n");
        cleanup(global_vars);
        return -1;
    }

    if (seek_set.seek_flags & (SEEKING_FLAG_BY_TIME | SEEKING_FLAG_RETURN_REF_TIME))
        printf("\nPositioned to ref time %.2f sec, start sending data...\n\n", (double)(seek_set.ref_time) / 27000000.0);
    else if (seek_set.seek_flags & (SEEKING_FLAG_BY_ABSOLUTE_OFFSET | SEEKING_FLAG_RETURN_REF_OFFSET))
        printf("\nPositioned to ref offset 0x%16lX, start sending data...\n\n", seek_set.ref_offset);
    else if (seek_set.seek_flags & (SEEKING_FLAG_BY_FRAME | SEEKING_FLAG_RETURN_REF_FRAME))
        printf("\nPositioned to ref frame #%ld, start sending data...\n\n", seek_set.ref_frame);

    // now do the demuxing
    int64_t previous_file_size = global_vars.file_info.file_size;
    long long int file_size_update_time = monotonic_time();
    while (true) {
        const int32_t i = mxfDemuxPush(global_vars.demuxer, global_vars.parser);
        if (global_vars.is_growing_file) {
            mxfDemuxUpdateFilesize(global_vars.demuxer, global_vars.parser, 0);
            mxfDemuxGetFileInfo(global_vars.demuxer, &global_vars.file_info);
        }
        if (i > 0) {
            if (global_vars.curr_processed > global_vars.prev_processed) {
                printf("Processed %lld bytes of %lld ...\n", global_vars.curr_processed, global_vars.file_info.file_size);
                global_vars.prev_processed = global_vars.curr_processed;
            }
        }
        else if (global_vars.is_growing_file) {
            if (previous_file_size < global_vars.file_info.file_size) {
                // File is still growing ...
                previous_file_size = global_vars.file_info.file_size;
                file_size_update_time = monotonic_time();
            }
            else if (monotonic_time() - file_size_update_time > 10) {
                // File is not growing for last 10 seconds
                printf("File is not growing anymore, exiting ...\n");
                break;
            }
            // File is still growing, sleep and continue ...
#if defined(_WIN32) || defined(_WIN64)
            Sleep(100);
#else
            usleep(100 * 1000);
#endif
        }
        else {
            if (i < 0)
                printf("mxfDemuxPush returned negative value\n");
            break;
        }
    }

    cleanup(global_vars);
    return 0;
}

