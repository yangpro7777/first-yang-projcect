/********************************************************************
 Created: 2007/06/07
 File name: sample_demux_mp4_file.cpp
 Purpose: command-line sample for MP4 demuxer

 Copyright (c) 2007-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#include "mctypes.h"
#include "buf_file.h"
#include "buf_wave.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "demux_mp4.h"

#define  NMP_PLAYBACK_DMX_DETECT_CHUNK_SIZE (512 * 1024)

class CExtIO : public mc_external_io_t {
    size_t m_file_pos;
    FILE* m_log_file;

    int32_t Open(char *fileName, void **pp_file_handle) {
        *pp_file_handle = 0;
        FILE* fd = fopen(fileName, "rb");
        if (fd) {
            *pp_file_handle = fd;
        }
        return fd != 0 ? 0 : 1;
    }
    int32_t Seek(void* p_file_handle, int64_t position) {
        FILE* fd = (FILE*)p_file_handle;
        m_file_pos = position;
        if (fd) {
            return fseek(fd, (long)position, SEEK_SET) == 0 ? 0 : 1;
        }
        return 1;
    }
    int32_t Read(void *p_file_handle, uint8_t *p_buffer, int32_t buffer_size) {
        FILE* fd = (FILE*)p_file_handle;
        if (m_log_file) {
            fprintf(m_log_file, "ExtIO: pos: %d, read: %d\n", (int)m_file_pos, buffer_size);
        }
        if (fd) {
            int32_t bytes = (int32_t)fread(p_buffer, 1, buffer_size, fd);
            if (bytes > 0)
                return bytes;
        }
        return 0;
    }
    void Close(void **pp_file_handle) 
    {
        fclose((FILE*)*pp_file_handle);
        *pp_file_handle = 0;
    }


    static int32_t mp4ExternalOpenCB(void *p_app_ptr, char *p_filename, void **pp_file_handle) {
        return ((CExtIO*)p_app_ptr)->Open(p_filename, pp_file_handle);
    }

    static int32_t mp4ExternalSeekCB(void *p_app_ptr, void *p_file_handle, int64_t position) {
        return ((CExtIO*)p_app_ptr)->Seek(p_file_handle, position);
    }

    static int32_t mp4ExternalReadCB(void *p_app_ptr, void *p_file_handle, uint8_t *p_buffer, int32_t buffer_size) {
        return ((CExtIO*)p_app_ptr)->Read(p_file_handle, p_buffer, buffer_size);
    }

    static void mp4ExternalCloseCB(void *p_app_ptr, void **pp_file_handle) {
        ((CExtIO*)p_app_ptr)->Close(pp_file_handle);
    }

public:
    CExtIO(const char* input_file_name, mp4dmux_settings& demuxer_set, const char* log_file_name)
        : m_log_file(0)
        , m_file_pos(0)
    {
        FILE* fd = fopen(input_file_name, "rb");
        if (fd) {
            fseek(fd, 0, SEEK_END);
            //WARNING: please check file_size
            // it should be the real file size in bytes
            // demuxer will read only first file_size bytes from the file. no more.
            file_size = ftell(fd);
            fclose(fd);
        }

        p_app_ptr = this;
        p_open = mp4ExternalOpenCB;
        p_openW = 0;
        p_seek = mp4ExternalSeekCB;
        p_read = mp4ExternalReadCB;
        p_close = mp4ExternalCloseCB;


        demuxer_set.file_length = file_size;
        demuxer_set.use_external_io = 1;
        demuxer_set.p_external_io = this;

        if (log_file_name) {
            m_log_file = fopen(log_file_name, "w");
        }
    }
};

typedef struct app_vars_s
{
    char *in_file;
    char *out_file;
    int32_t sid;
    int32_t parser;
    int32_t omit_editlist;
    bufstream_tt *out_bs;
    mp4dmux_tt *demuxer;

} app_vars_t;

static void print_fourcc(char* str, const char* fcc)
{
    char buf[64];
    uint32_t i;
    for (i = 0; i < 4; i++) {
        if (isprint(fcc[i])) {
            sprintf(buf, "%c", fcc[i]);
        } else {
            sprintf(buf, "%%%o", (uint8_t)fcc[i]);
        }
        strcat(str, buf);
    }
}

void print_usage()
{
    printf("\n==== MainConcept MP4 Demuxer file sample ====\n"
        "Usage:\nsample_demux_mp4_file.exe -i file.mp4 -sid 1 -o stream.out [-noelst] [-ext.io] [-cache.size bytes] [-ext.log file_name]\n"
        "    -noelst              Specify the option to ignore edit lists from container (old MP4 demuxer behavior)\n"
        "    -ext.io              Use external IO API\n"
        "    -ext.log file_name   Log external read operation into the file_name.\n"
        "    -cache.size bytes    Specify external IO read chunk size in bytes\n"
        "    -list.meta           List all available metadata\n"
        "    -save.meta           Save all available metadata\n"
        "\n");
}

#define IDN_NO_EDIT_LIST      (IDC_CUSTOM_START_ID + 1)
#define IDN_EXTERNAL_IO       (IDC_CUSTOM_START_ID + 2)
#define IDS_EXTERNAL_LOG_FILE (IDC_CUSTOM_START_ID + 3)
#define IDI_CACHE_SIZE        (IDC_CUSTOM_START_ID + 4)
#define IDI_LIST_META         (IDC_CUSTOM_START_ID + 5)
#define IDI_SAVE_META         (IDC_CUSTOM_START_ID + 6)

int main(int argc, char * argv[])
{
    mp4dmux_settings demuxer_set;
    mp4dmux_parser_settings parser_set;
    mp4dmux_stream_settings stream_set;
    mp4dmux_file_info file_info;
    mp4dmux_stream_format_tt stream_info;
    mp4dmux_seek_info seek_info;
    uint32_t uiRequiredBufSize = 0;
    uint32_t uiUseExternalIO = 0;
    char* sExternalLogFile = 0;
    int32_t iCacheSize = 0;
    int32_t i, ret = 1;
    int64_t byte_cnt, prg_mod, prg_next;
    int32_t iListMeta = 0;
    int32_t iSaveMeta = 0;

    APIEXTFUNC_SETPARAM mp4DemuxApiExtSetParam = NULL;

    app_vars_t vars;

    arg_item_t params[] =
    {
        //standard args
        { IDS_INPUT_FILE,          1, &vars.in_file },
        { IDI_C_STREAM_ID,         0, &vars.sid },
        { IDS_OUTPUT_FILE,         0, &vars.out_file },
        //custom args
        { IDN_NO_EDIT_LIST,        0, &vars.omit_editlist },
        { IDN_EXTERNAL_IO,         0, &uiUseExternalIO },
        { IDS_EXTERNAL_LOG_FILE,   0, &sExternalLogFile },
        { IDI_CACHE_SIZE,          0, &iCacheSize },
        { IDI_LIST_META,           0, &iListMeta },
        { IDI_SAVE_META,           0, &iSaveMeta },
    };
    static const arg_item_desc_t custom_args[] =
    {
        { IDN_NO_EDIT_LIST,        { "noelst", 0 },         ItemTypeNoArg,  1,  "ommit edit list" },
        { IDN_EXTERNAL_IO,         { "ext.io", 0 },         ItemTypeNoArg,  1,  "use External IO API" },
        { IDS_EXTERNAL_LOG_FILE,   { "ext.log", 0 },        ItemTypeString, 0,  "log external IO read operation to file" },
        { IDI_CACHE_SIZE,          { "cache.size", 0 },     ItemTypeInt,    0,  "size of demuxer read cache" },
        { IDI_LIST_META,{ "list.meta", 0 },     ItemTypeNoArg,    1,  "list all metadata" },
        { IDI_SAVE_META,{ "save.meta", 0 },     ItemTypeNoArg,    1,  "save all metadata to a xml file" },
    };

    uint32_t argsn = sizeof(params) / sizeof(arg_item_t);
    uint32_t argsd = sizeof(custom_args) / sizeof(arg_item_desc_t);

    memset(&vars, 0, sizeof(app_vars_t));

    if (parse_args(argc - 1, argv + 1, argsn, params, custom_args, argsd) < 0) {
        print_usage();
        return 0;
    }

    // retrieve SetParam APIEXT function
    mp4DemuxApiExtSetParam = (APIEXTFUNC_SETPARAM) mp4DemuxGetAPIExt(MCAPI_SetParam);

    memset(&demuxer_set, 0, sizeof(mp4dmux_settings));

    CExtIO* ext_io(0);
    if (uiUseExternalIO == 1) {
        ext_io = new CExtIO(vars.in_file, demuxer_set, sExternalLogFile);
    }
    if(iCacheSize != ITEM_NOT_INIT)
        demuxer_set.read_cache_size = iCacheSize;

    callbacks_t callbacks;
    init_callbacks(callbacks);
    // create a demuxer
    vars.demuxer = mp4DemuxNew(&callbacks, NULL, &demuxer_set);
    if (vars.demuxer == NULL)
    {
        printf("Unable to create a demuxer instance.\n");
        return 1;
    }

    // Demultiplexer honors container edit lists by default and produces the elementary stream
    // according to them. For example single series of video access units stored in container
    // could be played several times if edit list instruct so, some parts of video could be
    // excluded from playback and so on. However some use cases require elementary stream to
    // be extracted as is (e.g. for non-linear video editing). That's why option called 
    // DEMUX_MP4_PARAM_EDITLIST_ENABLED was introduced.

    uint32_t paramID = DEMUX_MP4_PARAM_EDITLIST_ENABLED;
    uint32_t paramData = (vars.omit_editlist == ITEM_NOT_INIT) ? 1 : 0;

    mp4DemuxApiExtSetParam(vars.demuxer, paramID, &paramData);

    if (mp4DemuxOpen(vars.demuxer, NULL, vars.in_file))
        goto err_exit;

    // get info about the input file
    if (mp4DemuxGetFileInfo(vars.demuxer, &file_info))
        goto err_exit;

    if (iListMeta == 1 || iSaveMeta == 1) {
        mp4dmux_metadata_info_tt Meta[10] = { 0 };
        uint32_t nMetaCount = 0;
        nMetaCount = mp4DemuxGetMetaData(vars.demuxer, Meta, nMetaCount);
        if (nMetaCount > 0) {
            int err = mp4DemuxGetMetaData(vars.demuxer, Meta, nMetaCount);
            if (err >= 0) {
                printf("Metadata: %d found\n", nMetaCount);
                for (uint32_t i = 0; i < nMetaCount; i++) {
                    printf("    %d: %s/meta\n", i, Meta[i].path);
                    if (iSaveMeta == 1) {
                        Meta[i].xml = new char[Meta[i].xml_size];
                    }
                }
                if (iSaveMeta == 1) {
                    err = mp4DemuxGetMetaData(vars.demuxer, Meta, nMetaCount);
                    if (err >= 0) {
                        for (uint32_t i = 0; i < nMetaCount; i++) {
                            char    meta_file_name[1024];
                            sprintf(meta_file_name, "meta_%d.xml", i);
                            FILE* f = fopen(meta_file_name, "wb");
                            if (f) {
                                fwrite(Meta[i].xml, 1, Meta[i].xml_size, f);
                                fclose(f);
                            }
                        }
                    }

                    for (uint32_t i = 0; i < nMetaCount; i++) {
                        delete[] Meta[i].xml;
                    }
                }
            }
        }
        else if (nMetaCount == 0) {
            printf("Metadata: no metadata found\n");
        }
    }

    mp4DemuxGetUVMetaData(vars.demuxer, NULL, 0, &uiRequiredBufSize);
    if ( uiRequiredBufSize ) {

        int8_t *pXmlBuff = new int8_t[uiRequiredBufSize];
        if ( pXmlBuff ) {

            mp4DemuxGetUVMetaData(vars.demuxer, pXmlBuff, uiRequiredBufSize, &uiRequiredBufSize);
            if ( pXmlBuff ) {

                FILE *f = fopen("metadata.xml", "wb");
                if ( f ) {

                    fwrite(pXmlBuff, uiRequiredBufSize, sizeof(uint8_t), f);
                    fclose(f);
                }
            }
        }
    }

    if (vars.sid < 0)
    {
        printf("Number of streams: %d\n\n", file_info.stream_count);

        for (i = 0; i < file_info.stream_count; i++)
        {
            char str[256];

            // get info about the stream
            if (mp4DemuxGetStreamFormat(vars.demuxer, &stream_info, i))
                goto err_exit;

            sprintf(str, "  Stream: %d, fourcc: \"%c%c%c%c\", ", i,
                    static_cast<char>((stream_info.four_cc & 0xff000000U) >> 24U),
                    static_cast<char>((stream_info.four_cc & 0xff0000U) >> 16U),
                    static_cast<char>((stream_info.four_cc & 0xff00U) >> 8U),
                    static_cast<char>(stream_info.four_cc & 0xffU));

            switch(stream_info.format.stream_mediatype)
            {
            case mctProRes:
                strcat(str, "ProRes video");
                break;
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
            case mctAV1:
                strcat(str, "AV1 video");
                break;
            case mctHEVC:
                strcat(str, "HEVC video");
                break;
            case mctDV:
                strcat(str, "DV video");
                break;
            case mctMotionJPEG:
                strcat(str, "Motion JPEG");
                break;
            case mctJ2K:
                strcat(str, "JPEG-2000 video");
                break;
            case mctVC1:
                strcat(str, "VC-1 video");
                break;
            case mctVC3:
                strcat(str, "VC-3 video");
                break;
            case mctMPEG1A:
            case mctMPEG2A:
                strcat(str, "MPEG audio");
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
            case mctAC3_DDPlus:
                strcat(str, "Dolby Digital Plus audio");
                break;
            case mctDTS:
                strcat(str, "DTS audio");
                break;
            case mctTWOS_LPCM:
                strcat(str, "TWOS LPCM audio");
                break;
            case mctQT_PCM:
                strcat(str, "Qt PCM audio");
                break;
            case mctALAW:
                strcat(str, "A-Law audio");
                break;

            case mctULAW:
                strcat(str, "U-Law audio");
                break;

            case mctIntel_ADPCM:
            case mctMS_ADPCM:
                strcat(str, "adaptive PCM audio");
                break;
            case mctAMR:
                strcat(str, "AMR audio");
                break;
            case mctPrivateBinary:
                {
                    if(stream_info.raw_decoder_config_len >= 8 && !memcmp(stream_info.raw_decoder_config, "dcmt", 4)) {
                        strcat(str, "Timecode track");

                        mc_private_format_t *private_format = (mc_private_format_t *)stream_info.format.pFormat;
                        uint32_t flags            = (private_format->reserved[3] << 24) | (private_format->reserved[2] << 16) |
                                                    (private_format->reserved[1] << 8) | private_format->reserved[0];
                        uint32_t time_scale       = (private_format->reserved[7] << 24) | (private_format->reserved[6] << 16) |
                                                    (private_format->reserved[5] << 8) | private_format->reserved[4];
                        uint32_t frame_duration   = (private_format->reserved[11] << 24) | (private_format->reserved[10] << 16) |
                                                    (private_format->reserved[9] << 8) | private_format->reserved[8];
                        uint32_t number_of_frames = (private_format->reserved[15] << 24) | (private_format->reserved[14] << 16) |
                                                    (private_format->reserved[13] << 8) | private_format->reserved[12];
                        
                        /* Assume Counter flag is set to 1 in tmcd track even if it is not, because most of streams have counter timecode format. */
                        if( 1 /*flags & 0x0008*/ ) {  // counter mode
                            uint32_t counter = ((stream_info.raw_decoder_config[4] << 24) | (stream_info.raw_decoder_config[5] << 16) |
                                               (stream_info.raw_decoder_config[6] << 8) | (stream_info.raw_decoder_config[7]));
                            uint32_t time_sec = counter / number_of_frames;
                            uint8_t frames = counter % number_of_frames;
                            uint32_t hours = time_sec / 3600;
                            uint8_t minutes = time_sec / 60 % 60;
                            uint8_t seconds = time_sec % 60;
                            sprintf(str, "%s %02d:%02d:%02d.%02d (counter = %d)", str,  hours, minutes, seconds, frames, counter);
                        }
                        else {
                            uint8_t hours = stream_info.raw_decoder_config[4];
                            uint8_t negative = (stream_info.raw_decoder_config[5] & 0x80) >> 7;
                            uint8_t minutes = stream_info.raw_decoder_config[5] & 0x7F;
                            uint8_t seconds = stream_info.raw_decoder_config[6];
                            uint8_t frames = stream_info.raw_decoder_config[7];
                            if(negative) strcat(str, " -");
                            else strcat(str, " +");
                            sprintf(str, "%s%02d:%02d:%2d.%d", str, hours, minutes, seconds, frames);
                        }
                    }
                    else {
                        strcat(str, "Other");
                    }
                }
                break;

            case mctMinAudioType:
                if (stream_info.raw_decoder_config_len == 4) {
                    strcat(str, " Audio ('");
                    print_fourcc(str, (const char*)stream_info.raw_decoder_config);
                    strcat(str, "')");
                }
                break;

            case mctMinVideoType:
                if (stream_info.raw_decoder_config_len == 4) {
                    strcat(str, " Video ('");
                    print_fourcc(str, (const char*)stream_info.raw_decoder_config);
                    strcat(str, "')");
                }
                break;

            case mctUnknown:
                if (stream_info.raw_decoder_config_len == 4) {
                    strcat(str, " Unknown ('");
                    print_fourcc(str, (const char*)stream_info.raw_decoder_config);
                    strcat(str, "')");
                }
                break;

            default:
                strcat(str, "Other");
                break;
            }

            if (stream_info.format.stream_mediatype > mctMinVideoType && stream_info.format.stream_mediatype < mctMaxVideoType) {
                const mc_video_format_t *vf = (mc_video_format_t *) stream_info.format.pFormat;
                if (vf && vf->compressorname_len) {
                    strcat(str, " ( Compressorname : ");
                    char buf[64] = {0};
                    for (int i = 0; i < vf->compressorname_len; i++) {
                        if (isprint(vf->compressorname[i])) {
                            sprintf(buf, "%c", vf->compressorname[i]);
                            strcat(str, buf);
                        } else {
                            sprintf(buf, "%%%o", (uint8_t) vf->compressorname[i]);
                            strcat(str, buf);
                        }
                    }
                    strcat(str, " )");
                }
            }

            printf("%s\n", str);
        }

        printf("\n");

        // free the demuxer and exit
        mp4DemuxFree(vars.demuxer);
        return 0;
    }

    // get info about the stream to demux
    if (mp4DemuxGetStreamFormat(vars.demuxer, &stream_info, vars.sid))
        goto err_exit;

    // setup the output file
    switch (stream_info.format.stream_mediatype)
    {
    case mctTWOS_LPCM:
    case mctQT_PCM:
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
    memset(&parser_set, 0, sizeof(mp4dmux_parser_settings));
    parser_set.add_adts_headers = 1;
    parser_set.add_amr_header = 1;
    parser_set.annexb_output = 1;
    vars.parser = mp4DemuxNewParser(vars.demuxer, &parser_set);
    if (vars.parser == 0)
        goto err_exit;

    // add the stream to demux to the parser
    memset(&stream_set, 0, sizeof(mp4dmux_stream_settings));

    stream_set.stream_num = vars.sid;
    stream_set.bs = vars.out_bs;

    if (mp4DemuxAddStream(vars.demuxer, vars.parser, &stream_set))
        goto err_exit;

    // seek to the beginning of the file
    memset(&seek_info, 0, sizeof(mp4dmux_seek_info));
    seek_info.parser_num = vars.parser;
    seek_info.stream_num = vars.sid;

    if (mp4DemuxSeek(vars.demuxer, &seek_info)) { // Seek_info will contain correct frame index values only if stream has single frame per sample. It's correct according to ISO-14496-15. Otherwise mp4dmux_settings::frame_index_length should be set.
        goto err_exit;
    }
    byte_cnt = 0;
    prg_next = 0;
    prg_mod = file_info.file_size / 100;
    if (!prg_mod)
        prg_mod = 1;

    // main demuxing loop
    // push data through the demuxer until eof or error
    while (1)
    {
        i = mp4DemuxPush(vars.demuxer, vars.parser);
        if (i < 0)
            goto err_exit;	// an error occurred
        else if (i == 0)
            break;			// end of file
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
    printf("\rProcessed %u%% ...", 100);
    ret = 0;

err_exit:

    // close everything
    if (vars.demuxer)
    {
        if (vars.parser)
            mp4DemuxFreeParser(vars.demuxer, vars.parser);

        mp4DemuxClose(vars.demuxer);
        mp4DemuxFree(vars.demuxer);
    }

    if (vars.out_bs)
        close_file_buf(vars.out_bs, 0);

    delete ext_io;
    return ret;
}

