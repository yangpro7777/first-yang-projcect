/**
 @file  mfimport_defs_avc.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_avc.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_74D985F1_51FE_4663_B042_A946958B80AE
#define UUID_74D985F1_51FE_4663_B042_A946958B80AE

#include "dec_avc.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for avc. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_avc_sample_info_s
{
    // native info
    h264_seq_par_set_t seq_par_set; //!< sequence paramter set, defined in dec_avc.h, see ISO/IEC 14496-10 for reference
    h264_pic_par_set_t pic_par_set; //!< picture parameter set, defined in dec_avc.h, See ISO/IEC 14496-10 for reference
    user_data_t user_data;          //!< user data, defined in dec_avc.h

    // for compatiblity
    struct GOP_Params gop_params;   //!< group of pictures header, defined in mcdefs.h
    struct PIC_ParamsEx pic_params; //!< video picture header and coding extension, defined in mcdefs.h

} mfimport_avc_sample_info_t;

#pragma pack(pop)

#endif
