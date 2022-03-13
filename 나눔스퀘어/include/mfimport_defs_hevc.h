/**
 @file  mfimport_defs_hevc.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_hevc.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_812227C5_EE79_4050_BE1B_2EB28411FE4A
#define UUID_812227C5_EE79_4050_BE1B_2EB28411FE4A

#include "mcdefs.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for hevc. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_hevc_sample_info_s
{
    // native info not available yet
    //    hevc_vid_par_set_t vid_par_set;              //!< video paramter set, defined in dec_hevc.h
    //    hevc_seq_par_set_t seq_par_set;              //!< sequence paramter set, defined in dec_hevc.h
    //    hevc_pic_par_set_t pic_par_set;              //!< picture parameter set, defined in dec_hevc.h
    //    hevc_user_data_t user_data;                  //!< user data, defined in dec_hevc.h

    // for compatiblity
    struct SEQ_ParamsEx seq_params; //!< video sequence header, defined in mcdefs.h
    struct PIC_ParamsEx pic_params; //!< video picture header and coding extension, defined in mcdefs.h

} mfimport_hevc_sample_info_t;

#pragma pack(pop)

#endif
