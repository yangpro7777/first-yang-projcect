/**
 @file  mfimport_defs_dv.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_dv.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_0822D974_4D3C_4DFF_AA65_88299C4BA215
#define UUID_0822D974_4D3C_4DFF_AA65_88299C4BA215

#include "dv_info.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for dv. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_dv_sample_info_s
{
    // native info
    dv_video_info_ex frame_info; //!<  extended dv info - video system parameters, defined in dv_info.h

} mfimport_dv_sample_info_t;

#pragma pack(pop)

#endif
