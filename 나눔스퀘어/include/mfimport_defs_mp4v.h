/**
 @file  mfimport_defs_mp4v.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_mp4v.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_C0A94A10_4390_490F_BA82_E20563726998
#define UUID_C0A94A10_4390_490F_BA82_E20563726998

#include "mcdefs.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for mp4v. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_mp4v_sample_info_s
{
    // native info
    UserData_Info user_data; //!< information about user data found in the video stream, defined in mcdefs.h

    // for compatiblity
    struct SEQ_ParamsEx seq_params; //!< video sequence header, defined in mcdefs.h
    struct PIC_ParamsEx pic_params; //!< video picture header and coding extension, defined in mcdefs.h

} mfimport_mp4v_sample_info_t;

#pragma pack(pop)

#endif
