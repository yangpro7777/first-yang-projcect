/**
 @file  mfimport_defs_mpa.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_mpa.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_A53FE188_915B_4532_AD3F_A89ECC612875
#define UUID_A53FE188_915B_4532_AD3F_A89ECC612875

#include "common_audio.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for mpeg. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_mpa_sample_info_s
{
    // native info
    aud_frame_hdr frame_info; //!< information about audio found in audio stream, defined in mcdefs.h

} mfimport_mpa_sample_info_t;

#pragma pack(pop)

#endif
