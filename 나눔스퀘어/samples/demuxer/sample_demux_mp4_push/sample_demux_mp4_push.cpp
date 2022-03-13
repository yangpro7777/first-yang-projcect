/*!
*
* Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
*
* MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
* This software is protected by copyright law and international treaties.  Unauthorized
* reproduction or distribution of any portion is prohibited by law.
**/

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "mctypes.h"
#include "mcmediatypes.h"
#include "demux_mp4.h"
#include "buf_direct.h"
#include "buf_file.h"
#include "auxinfo.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
// iOS
//#include "iOS_specifics.h"
#endif
#endif

#define STRINGIFY_CASE(pfx, mt) case pfx ## mt: ret = #mt ; break

//--------------------------------------------------------------------------------------------------
const char* mediatype2string(mcmediatypes_t mt)
{
    const char* ret = NULL;
    
    switch(mt)
    {
        STRINGIFY_CASE(mct, Unknown);
        STRINGIFY_CASE(mct, MinVideoType);
        STRINGIFY_CASE(mct, MPEG1V);
        STRINGIFY_CASE(mct, MPEG2V);
        STRINGIFY_CASE(mct, MPEG4V);
        STRINGIFY_CASE(mct, H263);
        STRINGIFY_CASE(mct, H264);
        STRINGIFY_CASE(mct, MVCSub);
        STRINGIFY_CASE(mct, MVCMux);
        STRINGIFY_CASE(mct, HEVC);
        STRINGIFY_CASE(mct, VC1);
        STRINGIFY_CASE(mct, DV);
        STRINGIFY_CASE(mct, J2K);
        STRINGIFY_CASE(mct, MotionJPEG);
        STRINGIFY_CASE(mct, VC3);
        STRINGIFY_CASE(mct, DIV3);
        STRINGIFY_CASE(mct, DIV4);
        STRINGIFY_CASE(mct, MPEG4V3);
        STRINGIFY_CASE(mct, ProRes);
        STRINGIFY_CASE(mct, AV1);
        STRINGIFY_CASE(mct, MaxVideoType);


        STRINGIFY_CASE(mct, MinAudioType);
        STRINGIFY_CASE(mct, MPEG1A);
        STRINGIFY_CASE(mct, MPEG2A);
        STRINGIFY_CASE(mct, AES3_337M_MPEG1A);
        STRINGIFY_CASE(mct, AES3_337M_MPEG2A);
        STRINGIFY_CASE(mct, AAC_Generic);
        STRINGIFY_CASE(mct, AAC_RAW);
        STRINGIFY_CASE(mct, AAC_LATM);
        STRINGIFY_CASE(mct, AAC_ADTS);
        STRINGIFY_CASE(mct, AC3);
        STRINGIFY_CASE(mct, AC3_DDPlus);
        STRINGIFY_CASE(mct, AES3_337M_AC3);
        STRINGIFY_CASE(mct, AES3_337M_DDPlus);
        STRINGIFY_CASE(mct, AES3_337M_DOLBYE);
        STRINGIFY_CASE(mct, AMR);
        STRINGIFY_CASE(mct, WMA);
        STRINGIFY_CASE(mct, WMAPro);
        STRINGIFY_CASE(mct, WMALossless);
        STRINGIFY_CASE(mct, SDDS);
        STRINGIFY_CASE(mct, DTS);
        STRINGIFY_CASE(mct, MaxAudioType);
        
        STRINGIFY_CASE(mct, MinPCMType);
        STRINGIFY_CASE(mct, PCM);
        STRINGIFY_CASE(mct, DVD_LPCM);
        STRINGIFY_CASE(mct, HDMV_LPCM);
        STRINGIFY_CASE(mct, AES3_302M);
        STRINGIFY_CASE(mct, AES3_332M);
        STRINGIFY_CASE(mct, AES3_382M);
        STRINGIFY_CASE(mct, AES3_331M);
        STRINGIFY_CASE(mct, BWF);
        STRINGIFY_CASE(mct, TWOS_LPCM);
        STRINGIFY_CASE(mct, QT_PCM);
        STRINGIFY_CASE(mct, Intel_ADPCM);
        STRINGIFY_CASE(mct, MS_ADPCM);
        STRINGIFY_CASE(mct, AIFF);
        STRINGIFY_CASE(mct, ALAW);
        STRINGIFY_CASE(mct, ULAW);
        STRINGIFY_CASE(mct, MaxPCMType);
        
        STRINGIFY_CASE(mct, MinPrivateType);
        STRINGIFY_CASE(mct, DVB_Teletext);
        STRINGIFY_CASE(mct, PrivateBinary);
        STRINGIFY_CASE(mct, DVD_Subtitles);
        STRINGIFY_CASE(mct, DXSB_Subtitles);
        STRINGIFY_CASE(mct, UTF8_Subtitles);
        STRINGIFY_CASE(mct, SSA_Subtitles);
        STRINGIFY_CASE(mct, ASS_Subtitles);
        STRINGIFY_CASE(mct, USF_Subtitles);
        STRINGIFY_CASE(mct, BMP_Subtitles);
        STRINGIFY_CASE(mct, VSUB_Subtitles);
        STRINGIFY_CASE(mct, CFF_ImgSubtitles);
        STRINGIFY_CASE(mct, CFF_TxtSubtitles);
        STRINGIFY_CASE(mct, DXSA_Subtitles);
        
        STRINGIFY_CASE(mct, VBI_Data);
        STRINGIFY_CASE(mct, ANC_Data);
        STRINGIFY_CASE(mct, AES3_337M_DATA);
        STRINGIFY_CASE(mct, MaxPrivateType);
        
        default:
            ret = "<unrecognized mediatype>";
            break;
    }
    
    return ret;
}

//--------------------------------------------------------------------------------------------------
void print_segment_info(mp4dmx_push_tt* demuxer)
{
    struct mp4dmux_file_info file_info;
    memset(&file_info, 0, sizeof(mp4dmux_file_info));
    mp4DemuxPushSegmentGetInfo(demuxer, &file_info);
    
    printf("\n\nSEGMENT (%d tracks available)\n", file_info.stream_count);
    
    for (int iter = 0; iter < file_info.stream_count; ++iter) {
        // print a particular stream info
        mp4dmux_stream_format_tt stream_format;
        mp4DemuxPushSegmentGetStreamInfo(demuxer, &stream_format, iter);
        
        printf("\tStream %d: %s\n", iter, mediatype2string(stream_format.format.stream_mediatype));
    }
    printf("\n");
}

//--------------------------------------------------------------------------------------------------
typedef struct thread_user_data_s {
    int32_t track_id;
    const char* output;
    bufstream_tt* output_bs;
} thread_user_data_t;

//--------------------------------------------------------------------------------------------------
void configure_output(mp4dmx_push_tt* demuxer, thread_user_data_t* data)
{
    if (!data->output_bs) {
        data->output_bs = open_file_buf_write(data->output, 8 * 1024, NULL);
    }

    struct mp4dmux_stream_settings stream_settings;
    memset(&stream_settings, 0, sizeof(mp4dmux_stream_settings));

    stream_settings.stream_num = data->track_id;
    stream_settings.bs = data->output_bs;

    mp4DemuxPushSegmentAddStream(demuxer, &stream_settings);
}

//--------------------------------------------------------------------------------------------------
void callback_func(mp4dmx_push_tt* demuxer, uint32_t event_type, void* user_data)
{
    thread_user_data_t* data = reinterpret_cast<thread_user_data_t*>(user_data);
    switch(event_type)
    {
    case MP4PDMUX_EVENT_NEW_SEGMENT:
        // print stream info for the new segment
        print_segment_info(demuxer);
        configure_output(demuxer, data);

        mp4DemuxPushSegmentStart(demuxer);
        break;

    case MP4PDMUX_EVENT_FAILURE:
        printf("Parser failure has occured!\n");
        break;
        
    case MP4PDMUX_EVENT_EOS:
        printf("End of input stream encountered, closing things down\n");
        if (data->output_bs) {
            data->output_bs->done(data->output_bs, 0);
            data->output_bs->free(data->output_bs);
            data->output_bs = NULL;
        }
        break;
        
    default:
        printf("Not sure what happened, but I guess it's all right.\n");
        break;
    }
}

//--------------------------------------------------------------------------------------------------
typedef struct app_args_s {
    char* in_file;
    char* out_file;
    int32_t sid;
} app_args_t;

#define INPUT_BUFFER_SIZE 16*1024
#define READ_CHUNK_SIZE 5*1024

//--------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    struct mp4dmux_push_settings* demuxer_settings;
    mp4dmx_push_tt* demuxer;
    bufstream_tt* bs_in;
    app_args_t args;

    arg_item_t params[] =
    {
        { IDS_INPUT_FILE, 1, &args.in_file },
        { IDI_C_STREAM_ID, 0, &args.sid },
        { IDS_OUTPUT_FILE, 0, &args.out_file }
    };

    memset(&args, 0, sizeof(app_args_t));

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params) < 0) {
        printf("\n==== MainConcept MP4 Push Mode Demuxer file sample ====\n"
            "Usage:\nsample_demux_mp4_push -i file.mp4 -sid 1 -o stream.out\n\n");
        return 1;
    }

    thread_user_data_t user_data = {0};
    user_data.track_id = args.sid;
    user_data.output = args.out_file;

    demuxer_settings = new mp4dmux_push_settings;
    memset(demuxer_settings, 0, sizeof(mp4dmux_push_settings));
    demuxer_settings->add_adts_headers = 1;
    demuxer_settings->annexb_output = 1;
    demuxer_settings->add_amr_header = 1;
    demuxer_settings->segment_callback = &callback_func;
    demuxer_settings->segment_callback_user_data = &user_data;
    demuxer_settings->input_buffer_size = INPUT_BUFFER_SIZE;

    demuxer = mp4DemuxPushNew(NULL, NULL, demuxer_settings);

    delete demuxer_settings;

    if (!demuxer) {
        return -1;
    }

    bs_in = open_bufstream_direct(NULL);

    mp4DemuxPushInitStream(demuxer, bs_in);

    FILE* file_in = fopen(args.in_file, "rb");
    
    const uint32_t buf_size = READ_CHUNK_SIZE;
    uint8_t* buf = (uint8_t*) malloc(buf_size);
    while (!feof(file_in)) {
        uint32_t read = fread(buf, 1, buf_size, file_in);
        uint32_t fed = 0;
        uint8_t* current = buf;
        
        while (fed != read) {
            uint32_t copied = bs_in->copybytes(bs_in, current, read - fed);
            fed += copied;
            current += copied;
        }
    }
    free(buf);

    bs_in->auxinfo(bs_in, 0, STREAM_END_CODE, NULL, 0); // Signal end of stream
    bs_in->free(bs_in);

    mp4DemuxPushWaitDone(demuxer);
    mp4DemuxPushCloseStream(demuxer);
    mp4DemuxPushFree(demuxer);
    
    printf("Done!\n");
    return 0;
}
