/**
 @file  common_audio.h
 @brief Common audio defines and data types

 @verbatim
 File: common_audio.h

 Desc: Common audio defines and data types

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_E82E9DC5_9AA6_4C04_B906_597A17D07FAB
#define UUID_E82E9DC5_9AA6_4C04_B906_597A17D07FAB

#pragma pack(push, 1)

 /**
 * @brief In this structure the decoder returns information about audio found in audio stream.
 */
struct aud_frame_hdr
{
    unsigned int header; /**< @brief unparsed header of AudioFrame */
    int ID;
    int layer;
    int protection_bit;
    int bitrate_index;
    int sampling_frequency; /**< @brief sampling frequency of audio */
    int padding_bit;
    int private_bit;
    int audioMode; /**< @brief MPEG audio mode encoded in file, one of the MODE_* consts */
    int mode_extension;
    int copyright;
    int original;
    int emphasis;

    unsigned int audioRate;  /**< @brief audio sample rate, either 32000, 44100 or 48000 */
    unsigned int sampleSize; /**< @brief size in bits of samples, will be 16 */
    int audioBitrate;        /**< @brief MPEG audio bitrate */
    int bytesPerSample;      /**< @brief bytes per audio sample (2 or 4) */
    int audioLayer;          /**< @brief MPEG audio layer encoded in file, 1 or 2 */
    int channels;            /**< @brief number of channels */
    int samplesInFrame;
    int bytesInFrame;
};
/** @} */

#pragma pack(pop)

#endif
