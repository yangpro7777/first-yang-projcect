/**
 @file  mfimport_defs_ddp.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_ddp.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_39EEA12A_6F51_497C_ADA3_B958D2765953
#define UUID_39EEA12A_6F51_497C_ADA3_B958D2765953

#include "dec_ddp.h"

#pragma pack(push, 1)

/**
 @brief Extended sample information for Dolby Digital. See mfi_sample_info_t.p_ext_info and
 mfi_sample_info_t.ext_info_len.
 @details Contains the info defined by the decoders which would normally be
 returned by the decoders auxinfo interface.
 **/
typedef struct mfimport_dd_sample_info_s
{
    // native info
    ac3_frame_hdr frame_info;          //!< information about the audio stream, defined in dec_ddp.h
    dd_gen_info_tt gen_info;           //!< information about the audio stream, defined in dec_ddp.h
    dd_mix_info_tt mix_info;           //!< information about the audio stream, defined in dec_ddp.h
    dd_prod_info_tt prod_info;         //!< information about the audio stream, defined in dec_ddp.h
    dd_timecode_info_tt timecode_info; //!< information about the audio stream, defined in dec_ddp.h
    dd_addbsi_info_tt addbsi_info;     //!< information about the audio stream, defined in dec_ddp.h
    dd_auxdata_info_tt auxdata_info;   //!< information about the audio stream, defined in dec_ddp.h

} mfimport_dd_sample_info_t;

#pragma pack(pop)

#endif
