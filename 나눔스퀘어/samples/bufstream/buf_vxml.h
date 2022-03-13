/* ----------------------------------------------------------------------------
 * File: buf_vxml.h
 *
 * Desc: Buffered stream splitter for XML index output of a video stream
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

// user data options
#define SESSION_UD_FIRST_SEQHDR   0x00  // put user data after the first sequence hdr
#define SESSION_UD_ALL_SEQHDR     0x01  // put user data after all sequence hdrs

//choose unique names for SESSION-messages (to be able to build bufstream-filter)

#define SESSION_USER_DATA     0x00200001

typedef struct tag_session_user_data_tt
{
  uint32_t udata_len;
  uint8_t *udata;  // buffer filled with user data
} session_user_data_tt;


#ifdef __cplusplus
extern "C" {
#endif

bufstream_tt *open_bufstream_write_with_vxml_index (
  bufstream_tt *main_bs,
  bufstream_tt *video_idx,
  uint32_t flag,
  void (*DisplayError)(char *txt));

void close_bufstream_write_with_vxml_index (
  bufstream_tt* bs,
  int32_t Abort);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
