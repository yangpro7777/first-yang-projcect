/**
 @file  mfimport_defs_vc3.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_vc3.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_B36C7A47_253D_4F8C_9C33_04D97F3B5136
#define UUID_B36C7A47_253D_4F8C_9C33_04D97F3B5136

#include "mcdefs.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for vc3. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_vc3_sample_info_s
{
    // for compatiblity
    struct SEQ_ParamsEx seq_params; //!< video sequence header, defined in mcdefs.h
    struct PIC_ParamsEx pic_params; //!< video picture header and coding extension, defined in mcdefs.h

} mfimport_vc3_sample_info_t;

#pragma pack(pop)

#endif
