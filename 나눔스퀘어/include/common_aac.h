/**
 @file  common_aac.h
 @brief Common AAC defines and data types

 @verbatim
 File: common_aac.h

 Desc: Common AAC defines and data types

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_F803067A_134D_4F27_880C_036B77EAB63B
#define UUID_F803067A_134D_4F27_880C_036B77EAB63B

/**
 * @name AAC audio conformance check errors
 * @{
 **/
#define INV_AAC_ERRORS                  0x00200000
#define INV_AAC_CHANNELS                INV_AAC_ERRORS +  0
#define INV_AAC_OBJECT_TYPE             INV_AAC_ERRORS +  1
#define INV_AAC_BITRATE                 INV_AAC_ERRORS +  2
#define INV_AAC_MPEG_VERSION            INV_AAC_ERRORS +  3
#define INV_AAC_SAMPLE_RATE             INV_AAC_ERRORS +  4
#define INV_AAC_HEADER_TYPE             INV_AAC_ERRORS +  5
#define INV_AAC_HF_CUTOFF               INV_AAC_ERRORS +  6
#define INV_AAC_VBR                     INV_AAC_ERRORS +  7
#define INV_AAC_HE_SIGNALLING           INV_AAC_ERRORS +  8
#define INV_AAC_HE_VS_SAMPLERATE        INV_AAC_ERRORS +  9
#define INV_AAC_HE_VS_VBR               INV_AAC_ERRORS + 10
#define INV_AAC_HE_V2_CHANNELS          INV_AAC_ERRORS + 11
#define INV_AAC_HE_NO_PROFILE           INV_AAC_ERRORS + 12
#define INV_AAC_BITS_PER_SAMPLE         INV_AAC_ERRORS + 13
#define INV_AAC_SONY_AAC                INV_AAC_ERRORS + 14
#define INV_AAC_CRC_PROTECTION          INV_AAC_ERRORS + 15
/** @} */

/**
 * @name Audio channel mode
 * @{
 **/
#define AAC_CH_MODE_AUTO    0             ///< Auto
#define AAC_CH_MODE_7_1_FRONT_CHANNEL 7   ///< 7.1 Front channels audio ( center + left/right front speakers + left/right front center speakers + left/right surround speakers + LFE )
#define AAC_CH_MODE_7_1_BACK_CHANNEL 12   ///< 7.1 Back channels audio ( center + left/right front speakers + left/right rear surround speakers  + left/right surround speakers + LFE )

#define AAC_CH_MODE_CUSTOM_BASE  0x10
#define AAC_CH_MODE_CUSTOM AAC_CH_MODE_CUSTOM_BASE     ///< Custom channel configuration
#define AAC_CH_MODE_INDEPENDENT AAC_CH_MODE_CUSTOM + 1 ///< All channels are independent
/** @} */

/**
 * @name Bits per PCM sample at input
 * @{
 **/
#define AAC_BITS_PER_SAMPLE_08     8		///<   8 bits - one byte per sample
#define AAC_BITS_PER_SAMPLE_16    16		///<   16 bits - two bytes per sample
#define AAC_BITS_PER_SAMPLE_24    24		///<   24 bits - three bytes per sample
#define AAC_BITS_PER_SAMPLE_FLOAT 32		///<   32 bits, floating point - four bytes per sample
/** @} */

/**
 * @name Header type
 * @{
 **/
#define AAC_HEADER_RAW          0 ///< no header
#define AAC_HEADER_ADTS         1 ///< ADTS header
#define AAC_HEADER_LATMLOAS     2 ///< LATM in LOAS header
#define AAC_HEADER_ADIF         3 ///< ADIF header
#define AAC_HEADER_LOAS_NOSMC   4 ///< LOAS (StreamMuxConfig signaled out-of-band) header
#define AAC_HEADER_LATM         5 ///< LATM header
#define AAC_HEADER_LATM_NOSMC   6 ///< LATM (StreamMuxConfig signaled out-of-band) header
/** @} */

/**
 * @name CRC protection
 * @{
 **/
#define AAC_PROTECT_ADTS_STREAM_NO 0		///<  stream is not protected
#define AAC_PROTECT_ADTS_STREAM_YES 1 		///<  stream is protected
/** @} */

/**
 * @name AAC bitrates
 * @{
 **/
#define AAC_AUDIOBITRATE_CUSTOM 0 ///<   Custom audio bitrate
#define AAC_AUDIOBITRATE_006 1    ///<   6 kbits/sec
#define AAC_AUDIOBITRATE_007 2    ///<   7 kbits/sec
#define AAC_AUDIOBITRATE_008 3    ///<   8 kbits/sec
#define AAC_AUDIOBITRATE_010 4    ///<   10 kbits/sec
#define AAC_AUDIOBITRATE_012 5    ///<   12 kbits/sec
#define AAC_AUDIOBITRATE_014 6    ///<   14 kbits/sec
#define AAC_AUDIOBITRATE_016 7    ///<   16 kbits/sec
#define AAC_AUDIOBITRATE_020 8    ///<   20 kbits/sec
#define AAC_AUDIOBITRATE_024 9    ///<   24 kbits/sec
#define AAC_AUDIOBITRATE_028 10   ///<   28 kbits/sec
#define AAC_AUDIOBITRATE_032 11   ///<   32 kbits/sec
#define AAC_AUDIOBITRATE_040 12   ///<   40 kbits/sec
#define AAC_AUDIOBITRATE_048 13   ///<   48 kbits/sec
#define AAC_AUDIOBITRATE_056 14   ///<   56 kbits/sec
#define AAC_AUDIOBITRATE_064 15   ///<   64 kbits/sec
#define AAC_AUDIOBITRATE_080 16   ///<   80 kbits/sec
#define AAC_AUDIOBITRATE_096 17   ///<   96 kbits/sec
#define AAC_AUDIOBITRATE_112 18   ///<   112 kbits/sec
#define AAC_AUDIOBITRATE_128 19   ///<   128 kbits/sec
#define AAC_AUDIOBITRATE_160 20   ///<   160 kbits/sec
#define AAC_AUDIOBITRATE_192 21   ///<   192 kbits/sec
#define AAC_AUDIOBITRATE_224 22   ///<   224 kbits/sec
#define AAC_AUDIOBITRATE_256 23   ///<   256 kbits/sec
#define AAC_AUDIOBITRATE_320 24   ///<   320 kbits/sec
#define AAC_AUDIOBITRATE_384 25   ///<   384 kbits/sec
#define AAC_AUDIOBITRATE_448 26   ///<   448 kbits/sec
#define AAC_AUDIOBITRATE_512 27   ///<   512 kbits/sec
#define AAC_AUDIOBITRATE_640 28   ///<   640 kbits/sec
#define AAC_AUDIOBITRATE_768 29   ///<   768 kbits/sec
#define AAC_AUDIOBITRATE_896 30   ///<   896 kbits/sec
#define AAC_AUDIOBITRATE_1024 31  ///<   1024 kbits/sec
/** @} */

/**
 * @name AAC output return codes
 * @{
 **/
#define aacOutErrNone 0 ///< no error
#define aacOutCancel 1  ///< user canceled settings
#define aacOutError 2   ///< undefined error
#define aacOutDecline 3 ///< unable to use input settings, resolution, audio etc.
/** @} */

/**
 * @name Audio frame header
 * @{
 **/

typedef struct
{
    int syncword;
    int id;
    int layer;
    int protection_abs;
    int profile;
    int sampling_freq_idx;
    int private_bit;
    int channel_config;
    int original_copy;
    int home;
    int copyright_id_bit;
    int copyright_id_start;
    int frame_length;
    int adts_buffer_fullness;
    int num_of_rdb;
    int crc_check;
    int rdb_position[4 - 1];
    int crc_check_rdb;

    unsigned int sampling_frequency;
    unsigned int bits_per_sample;
    unsigned int channels;
} aac_adts_header;

#endif
