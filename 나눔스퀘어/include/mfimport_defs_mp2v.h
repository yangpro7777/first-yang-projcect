/**
 @file  mfimport_defs_mp2v.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_mp2v.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_2438A97D_66A0_47A8_974E_03BFE0FC2540
#define UUID_2438A97D_66A0_47A8_974E_03BFE0FC2540

#include "mcdefs.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for mp2v. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_mp2v_sample_info_s
{
    // native info
    struct SEQ_ParamsEx seq_params;                 //!< video sequence header, defined in mcdefs.h
    struct SEQ_Ext_Params seq_ext_params;           //!< video sequnce extension, defined in mcdefs.h
    struct SEQ_Disp_Ext_Params seq_disp_ext_params; //!< sequence display extension, defined in mcdefs.h
    struct GOP_Params gop_params;                   //!< group of pictures header, defined in mcdefs.h
    struct PIC_ParamsEx pic_params;                 //!< video picture header and coding extension, defined in mcdefs.h
    UserData_Params user_data;                      //!< user data from the decoder associated with current decoded picture, defined in mcdefs.h

} mfimport_mp2v_sample_info_t;

#pragma pack(pop)

#endif
