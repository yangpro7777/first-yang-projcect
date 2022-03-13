/**
 @file  mfimport_defs_aac.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_aac.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_599C9CCF_5FF7_412D_8FA3_DEA22800E915
#define UUID_599C9CCF_5FF7_412D_8FA3_DEA22800E915

#include "dec_aac.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for aac. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_aac_sample_info_s
{
    // native info
    aac_decoded_frame_info frame_info;           //!< information about the decoded AAC frame, defined in dec_aac.h

} mfimport_aac_sample_info_t;

#pragma pack(pop)

#endif
