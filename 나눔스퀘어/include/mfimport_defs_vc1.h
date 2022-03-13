/**
 @file  mfimport_defs_vc1.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_vc1.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_8E7F385D_8FA6_4D00_B562_502B39EAE41B
#define UUID_8E7F385D_8FA6_4D00_B562_502B39EAE41B

#include "mcdefs.h"
#include "dec_vc1.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for vc1. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_vc1_sample_info_s
{
    // native info
    vc1_sequence_layer seq_layer; //!< sequence and layer parameters, defined in dec_vc1.h, see SMPTE 421M-2006 for reference

    // for compatiblity
    struct PIC_ParamsEx pic_params; //!< video picture header and coding extension, defined in mcdefs.h

} mfimport_vc1_sample_info_t;

#pragma pack(pop)

#endif
