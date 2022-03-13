/*!
*
* Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.
*
* MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
* This software is protected by copyright law and international treaties.  Unauthorized
* reproduction or distribution of any portion is prohibited by law.
**/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "mctypes.h"
#include "mcmediatypes.h"
#include "demux_mp4.h"
#include "buf_direct.h"
#include "buf_file.h"
#include "auxinfo.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"

#include "decrypt_aes_ctr.h"

// very simple decryption module - only for demonstration purposes (very insecure, naive, slow and simple implementation)

class Crypto
{
public:
    Crypto()
    {
        _ctx = aesctrNew(NULL);
    }
    virtual ~Crypto()
    {
        aesctrFree(_ctx);
    }

    void SetKey(uint8_t * data, uint32_t size)
    {
        aesctrSetKey(_ctx, data, size);
    }
    void SetIV(uint8_t * data, uint32_t size)
    {
        aesctrSetIV(_ctx, data, size);
    }
    void Decrypt(uint8_t * data, uint32_t size)
    {
        aesctrDecrypt(_ctx, data, size);
    }
private:
    aes_decryptor_t _ctx;
};

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
// iOS
#include "iOS_specifics.h"
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

        if (stream_format.scheme_type)
        {
            printf("encryption scheme %c%c%c%c version %d.%d",
                (stream_format.scheme_type >> 24) & 0xFF, 
                (stream_format.scheme_type >> 16) & 0xFF, 
                (stream_format.scheme_type >> 8) & 0xFF, 
                stream_format.scheme_type & 0xFF,
                stream_format.scheme_version_major,
                stream_format.scheme_version_minor);
        }
    }
    printf("\n");
    for (int iter = 0; iter < file_info.protection_system_info_count; ++iter)
    {
        // real application should select one of available DRM schemes that it supports and perform vendor-specific authentification in order to initialize decryption

        // some most common DRM systems, for the complete list please refer to the http://dashif.org/identifiers/protection/
        static const uint8_t kNagraMediaAccess[] = { 0xad, 0xb4, 0x1c, 0x24, 0x2d, 0xbf, 0x4a, 0x6d, 0x95, 0x8b, 0x44, 0x57, 0xc0, 0xd2, 0x7b, 0x95 };
        static const uint8_t kMicrosoftPlayReady[] = { 0x9a, 0x04, 0xf0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95 };
        static const uint8_t kGoogleWideVine[] = { 0xed, 0xef, 0x8b, 0xa9, 0x79, 0xd6, 0x4a, 0xce, 0xa3, 0xc8, 0x27, 0xdc, 0xd5, 0x1d, 0x21, 0xed };
        static const uint8_t kClearKey[] = { 0x10, 0x77, 0xef, 0xec, 0xc0, 0xb2, 0x4d, 0x02, 0xac, 0xe3, 0x3c, 0x1e, 0x52, 0xe2, 0xfb, 0x4b };
        static const uint8_t kAppleFairPlay[] = { 0x29, 0x70, 0x1F, 0xE4, 0x3C, 0xC7, 0x4A, 0x34, 0x8C, 0x5B, 0xAE, 0x90, 0xC7, 0x43, 0x9A, 0x47 };
        static const uint8_t kDivXDRM[] = { 0x35, 0xBF, 0x19, 0x7B, 0x53, 0x0E, 0x42, 0xD7, 0x8B, 0x65, 0x1B, 0x4B, 0xF4, 0x15, 0x07, 0x0F };

        protection_system_info_tt protection_system_info;
        mp4DemuxPushSegmentGetProtectionSystemInfo(demuxer, &protection_system_info, iter);

        std::cout << "\tprotection system info " << std::dec << iter << " uuid ";
        for (size_t i(0); i < 16; ++i) std::cout << std::hex << uint32_t(protection_system_info.system_id[i] >> 4) << uint32_t(protection_system_info.system_id[i] & 0x0F);

        if (0 == memcmp(kNagraMediaAccess, protection_system_info.system_id, 16)) std::cout << " (Nagra MediaAccess)";
        if (0 == memcmp(kMicrosoftPlayReady, protection_system_info.system_id, 16)) std::cout << " (Microsoft PlayReady)";
        if (0 == memcmp(kGoogleWideVine, protection_system_info.system_id, 16)) std::cout << " (Google WideVine)";
        if (0 == memcmp(kClearKey, protection_system_info.system_id, 16)) std::cout << " (ClearKey)";
        if (0 == memcmp(kAppleFairPlay, protection_system_info.system_id, 16)) std::cout << " (Apple FairPlay)";
        if (0 == memcmp(kDivXDRM, protection_system_info.system_id, 16)) std::cout << " (DivX DRM)";
        std::cout << std::endl;
    }
    printf("\n");
}

typedef uint32_t(MC_EXPORT_API * auxinfo_t)(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size);
typedef uint32_t(MC_EXPORT_API * copybytes_t)(bufstream_tt *bs, uint8_t *ptr, uint32_t numSamples);

//--------------------------------------------------------------------------------------------------

typedef std::vector<uint8_t> iv_t, kid_t, cenc_key_t;
typedef std::map<kid_t, cenc_key_t> key_map_t;

static uint8_t char2hex(char c)
{
    if ('0' <= c && c <= '9') return static_cast<uint8_t>(c - '0');
    else if ('A' <= c && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
    else if ('a' <= c && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    else throw std::runtime_error("invalid character encountered");
}

static std::vector<uint8_t> unhex(const std::string & value)
{
    std::vector<uint8_t> result;

    if (value.empty() || value.size() % 2) throw std::runtime_error("invalid hex string length");
    result.reserve(value.size() / 2);

    for (size_t i(0); i < value.size() / 2; ++i)
    {
        char c1 = value.at(i * 2);
        char c2 = value.at(i * 2 + 1);

        uint8_t digit = (char2hex(c1) << 4) | char2hex(c2);
        result.push_back(digit);
    }
    return result;
}

static std::vector<uint8_t> key2bin(const std::string & value)
{
    std::vector<uint8_t> result;
    result.reserve(value.size());

    if (value.find("0x") == 0)
    {
        result = unhex(value.substr(2));
    }
    else
    {
        std::copy(value.begin(), value.end(), std::back_inserter(result));
    }
    return result;
}

typedef struct thread_user_data_s {
    int32_t track_id;
    const char* output;
    bufstream_tt* output_bs;
    auxinfo_t original_auxinfo;
    copybytes_t original_copybytes;
    sample_encryption_info_tt * sample_encryption_info;
    uint32_t offset;
    Crypto crypto;
    key_map_t keys;
    kid_t last_kid;
    iv_t last_iv;
    uint8_t annexb_output;
} thread_user_data_t;

//--------------------------------------------------------------------------------------------------

static uint32_t MC_EXPORT_API local_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
    thread_user_data_t * data = reinterpret_cast<thread_user_data_t*>(bs->drive_ptr);
    switch (info_ID)
    {
    case DMXU_SAMPLE_ENCRYPTION_INFO:
    if (info_ptr && info_size == sizeof(sample_encryption_info_tt))
    {
        data->sample_encryption_info = reinterpret_cast<sample_encryption_info_tt*>(info_ptr);
        data->offset = 0;

        kid_t kid;
        kid.assign(data->sample_encryption_info->KID, data->sample_encryption_info->KID + 16);

        iv_t iv;
        iv.assign(data->sample_encryption_info->IV, data->sample_encryption_info->IV + data->sample_encryption_info->IV_size);

        if (kid != data->last_kid)
        {
            key_map_t::iterator i = data->keys.find(kid);
            if (i != data->keys.end())
            {
                cenc_key_t & key = i->second;
                data->crypto.SetKey(&key[0], static_cast<uint32_t>(key.size()));

                data->last_kid = kid;
            }
            else
            {
                std::cerr << "key for KID wasn't specified" << std::endl;
            }
        }
        if (iv != data->last_iv && data->last_kid != kid_t())
        {
            data->crypto.SetIV(&data->sample_encryption_info->IV[0], data->sample_encryption_info->IV_size);

            data->last_iv = iv;
        }
    }
        break;
    default:
        break;
    }
    return data->original_auxinfo(bs, offs, info_ID, info_ptr, info_size);
}

template<typename T>
static bool check_intersection(T x1, T x2, T y1, T y2, T & z1, T & z2)
{
    z1 = (std::max<T>)(x1, y1);
    z2 = (std::min<T>)(x2, y2);
    return (x1 <= y2 && y1 <= x2);
}

static uint32_t MC_EXPORT_API local_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numSamples)
{
    thread_user_data_t * data = reinterpret_cast<thread_user_data_t*>(bs->drive_ptr);
    if (data->sample_encryption_info && data->sample_encryption_info->is_encrypted)
    {
        if (data->last_kid == kid_t())
        {
            std::cerr << "decryption is impossible if key wasn't set!" << std::endl;
        }
        else
        {
            if (data->sample_encryption_info->sub_sample_count > 0)
            {
                uint32_t sub_sample_offset(0);
                for (uint32_t i(0); i < data->sample_encryption_info->sub_sample_count; ++i)
                {
                    sub_sample_offset += data->sample_encryption_info->sub_samples[i].bytes_of_clear_data;
                    const uint32_t sub_sample_length = data->sample_encryption_info->sub_samples[i].bytes_of_encrypted_data;

                    uint32_t decryption_start(0), decryption_end(0);
                    // check for intersection of sub-sample and currently supplied data
                    if (check_intersection<uint32_t>(data->offset, data->offset + numSamples, sub_sample_offset, sub_sample_offset + sub_sample_length, decryption_start, decryption_end))
                    {
                        decryption_start -= data->offset;
                        decryption_end -= data->offset;

                        data->crypto.Decrypt(ptr + decryption_start, decryption_end - decryption_start);
                    }
                    sub_sample_offset += sub_sample_length;
                }
            }
            else
            {
                // no sub-sampling, entire sample is encrypted
                data->crypto.Decrypt(ptr, numSamples);
            }
        }
    }
    data->offset += numSamples;

    return data->original_copybytes(bs, ptr, numSamples);
}

static uint32_t MC_EXPORT_API local_copybytes_with_accumulate(bufstream_tt *bs, uint8_t *ptr, uint32_t numSamples)
{
    static std::vector<uint8_t> cache;
    static uint32_t g_size = 0;

    thread_user_data_t * data = reinterpret_cast<thread_user_data_t*>(bs->drive_ptr);

    std::copy(ptr, ptr + numSamples, std::back_inserter(cache));

    if (data->sample_encryption_info && data->sample_encryption_info->is_encrypted)
    {
        if (data->last_kid == kid_t())
        {
            std::cerr << "decryption is impossible if key wasn't set!" << std::endl;
        }
        else
        {
            if (!g_size) {
                for (uint32_t i(0); i < data->sample_encryption_info->sub_sample_count; ++i) {
                    g_size += data->sample_encryption_info->sub_samples[i].bytes_of_clear_data;
                    g_size += data->sample_encryption_info->sub_samples[i].bytes_of_encrypted_data;
                }
            }
            if (cache.size() == g_size) {
                uint32_t sub_sample_offset(0);

                for (uint32_t i(0); i < data->sample_encryption_info->sub_sample_count; ++i)
                {
                    sub_sample_offset += data->sample_encryption_info->sub_samples[i].bytes_of_clear_data;
                    const uint32_t sub_sample_length = data->sample_encryption_info->sub_samples[i].bytes_of_encrypted_data;

                    if (sub_sample_length)
                    {
                        data->crypto.Decrypt(&cache[0] + sub_sample_offset, sub_sample_length);
                    }
                    sub_sample_offset += sub_sample_length;
                }
                data->original_copybytes(bs, &cache[0], static_cast<uint32_t>(cache.size()));
                cache.clear();
                g_size = 0;
            }
        }
    }
    else if (!g_size){
        cache.clear();
        return local_copybytes(bs,ptr,numSamples);
    }
    data->offset += numSamples;
    if (!cache.empty() && cache.size() == g_size) {
        data->original_copybytes(bs, &cache[0], static_cast<uint32_t>(cache.size()));
    }
    return numSamples;
}

void configure_output(mp4dmx_push_tt* demuxer, thread_user_data_t* data)
{
    if (!data->output_bs) {
        data->output_bs = open_file_buf_write(data->output, 8 * 1024, NULL);

        // subclassing of bufstream object - hook auxinfo & copybytes callbacks
        data->original_auxinfo = data->output_bs->auxinfo;
        data->original_copybytes = data->output_bs->copybytes;

        if (data->annexb_output) {
            data->output_bs->copybytes = local_copybytes_with_accumulate;
        } else {
            data->output_bs->copybytes = local_copybytes;
        }

        data->output_bs->auxinfo = local_auxinfo;
        data->output_bs->drive_ptr = reinterpret_cast<struct drive_struct*>(data);
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
    char* config_file;
    int32_t sid;
} app_args_t;

#define INPUT_BUFFER_SIZE 64*1024
#define READ_CHUNK_SIZE 2*1024

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
        { IDS_OUTPUT_FILE, 1, &args.out_file },
        { IDS_CONFIG_FILE, 0, &args.config_file }
    };

    memset(&args, 0, sizeof(app_args_t));

    if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params) < 0) {
        printf("\n==== MainConcept MP4 Push Mode Demuxer file sample ====\n"
            "Usage:\nsample_demux_mp4_push -i file.mp4 -sid 1 -o stream.out -c keys.conf\n"
            "keys.conf file contains hexadecimal <key-id> - <key pairs> separated by new lines, for instance\n"
            "\t0x279926496a7f5d25da69f2b3b2799a7f\n"
            "\t0xccc0f2b3b279926496a7f5d25da692f6\n"
            "\t0x676cb88f302d10227992649885984045\n"
            "\t0xccc0f2b3b279926496a7f5d25da692d6\n"
            "\n"
        );
        return EXIT_FAILURE;
    }

#if TARGET_OS_IPHONE
    char filename_iOS[256];
    if (args.in_file[0] != '/') {
        filename2iOS(args.in_file, filename_iOS);
        args.in_file = filename_iOS;
    }
#endif

    thread_user_data_t user_data = { };
    user_data.track_id = args.sid;
    user_data.output = args.out_file;

    if (args.config_file)
    {
        try
        {
            std::fstream file(args.config_file);
            if (!file.good())
            {
                std::cout << "failed to open key file " << args.config_file << std::endl;
                return EXIT_FAILURE;
            }
            std::string kid, key;
            while (file.good())
            {
                file >> kid;
                file >> key;

                user_data.keys[key2bin(kid)] = key2bin(key);
            }
        }
        catch (std::runtime_error & e)
        {
            std::cout << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    demuxer_settings = new mp4dmux_push_settings;
    memset(demuxer_settings, 0, sizeof(mp4dmux_push_settings));
    demuxer_settings->add_adts_headers = 1;
    demuxer_settings->annexb_output = 1;
    user_data.annexb_output = demuxer_settings->annexb_output;
    demuxer_settings->add_amr_header = 1;
    demuxer_settings->segment_callback = &callback_func;
    demuxer_settings->segment_callback_user_data = &user_data;
    demuxer_settings->input_buffer_size = INPUT_BUFFER_SIZE;

    demuxer = mp4DemuxPushNew(NULL, NULL, demuxer_settings);

    delete demuxer_settings;

    if (!demuxer) {
        return EXIT_FAILURE;
    }

    bs_in = open_bufstream_direct(NULL);

    mp4DemuxPushInitStream(demuxer, bs_in);

    FILE* file_in = fopen(args.in_file, "rb");
    
    const size_t buf_size = READ_CHUNK_SIZE;
    std::vector<uint8_t> buf(buf_size);
    while (!feof(file_in)) {
        size_t read = fread(&buf[0], 1, buf_size, file_in);
        size_t fed = 0;
        uint8_t* current = &buf[0];
        
        while (fed != read) {
            uint32_t copied = bs_in->copybytes(bs_in, current, static_cast<uint32_t>(read - fed));
            fed += copied;
            current += copied;
        }
    }

    bs_in->auxinfo(bs_in, 0, STREAM_END_CODE, NULL, 0); // Signal end of stream
    bs_in->free(bs_in);

    mp4DemuxPushWaitDone(demuxer);
    mp4DemuxPushCloseStream(demuxer);
    mp4DemuxPushFree(demuxer);
    
    printf("Done!\n");
    return EXIT_SUCCESS;
}
