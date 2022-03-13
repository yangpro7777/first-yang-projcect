/*****************************************************************************
 Created: 2014/12/11
 File name: dec_hevc_misc.h
 Purpose: miscellaneous functions used by the sample decoder

 Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.
 Unauthorized  reproduction or distribution of any portion is prohibited by law.
******************************************************************************/

#ifndef UUID_515CCA7A_CC3A_4146_BA09_1639F0F6DAC6
#define UUID_515CCA7A_CC3A_4146_BA09_1639F0F6DAC6

#include <map>
#include <string>
#include <stdio.h>
#include "mctypes.h"
#include "dec_hevc.h"

#define IDN_LOCAL_NODESET (IDC_CUSTOM_START_ID + 1)
#define IDN_LOCAL_PREVIEW_MODE (IDC_CUSTOM_START_ID + 2)
#define IDN_LOCAL_CALLBACKS (IDC_CUSTOM_START_ID + 3)
#define IDN_LOCAL_ASYNC_INPUT_OUTPUT (IDC_CUSTOM_START_ID + 4)
#define IDN_LOCAL_HW_ACC (IDC_CUSTOM_START_ID + 5)
#define IDN_LOCAL_HW_ADAPTER (IDC_CUSTOM_START_ID + 6)
#define IDN_LOCAL_HW_ENUMERATE (IDC_CUSTOM_START_ID + 7)
#define IDN_LOCAL_PIXEL_RANGE (IDC_CUSTOM_START_ID + 8)
#define IDN_LOCAL_DEINTERLACING_MODE (IDC_CUSTOM_START_ID + 9)
#define IDN_LOCAL_TRANSFER_CHARACTERISTICS (IDC_CUSTOM_START_ID + 10)

const std::map<std::string, hevc_decoding_toolset_t>& enumerateDecodingToolsets();

const uint64_t NANO_SECONDS_IN_SECOND = 1000000000;

/* Wall clock time */
uint64_t time_get_count();

/* Time frequency */
uint64_t time_get_freq();

#endif
