/* ----------------------------------------------------------------------------
 * File: buf_index.h
 *
 * Desc: Captures index output from muxers, the index format is described in
 *       mcindextypes.h
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 *
 * ----------------------------------------------------------------------------
 */

#include "bufstrm.h"

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _BS_UNICODE

bufstream_tt *open_bufstream_write_with_index (
  bufstream_tt *main_bs,
  const wchar_t *main_name,    // optional, will be put in the index header
  const wchar_t *index_base_name,
  uint8_t single_index_flag);

#else

bufstream_tt *open_bufstream_write_with_index (
  bufstream_tt *main_bs,
  const char *main_name,      // optional, will be put in the index header
  const char *index_base_name,
  uint8_t single_index_flag);

#endif

void close_bufstream_write_with_index (
  bufstream_tt* bs, 
  int32_t Abort);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
