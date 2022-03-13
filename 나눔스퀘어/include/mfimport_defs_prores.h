/**
 @file  mfimport_defs_prores.h
 @brief Media Format Importer extended sample information definitions

 @verbatim
 File: mfimport_defs_prores.h

 Desc: Media Format Importer extended sample information definitions

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_949B7F0F_7E66_42FC_8F2A_12B8BA2B78C1
#define UUID_949B7F0F_7E66_42FC_8F2A_12B8BA2B78C1

#include "mcdefs.h"

#pragma pack(push, 1)

typedef struct mfimport_prores_sample_info_s
{
    struct SEQ_ParamsEx seq_params; //!< video sequence header, defined in mcdefs.h
    struct PIC_ParamsEx pic_params; //!< video picture header and coding extension, defined in mcdefs.h

} mfimport_prores_sample_info_t;

#pragma pack(pop)

#endif
