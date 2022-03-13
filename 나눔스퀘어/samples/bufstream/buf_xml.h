/* ----------------------------------------------------------------------------
 * File: buf_xml.h
 *
 * Desc: Buffered stream splitter for XML index output of a program stream
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
#include "buf_vxml.h"  // for the session user data definitions

#define SESSION_USER_DATA_AUDIO     0x00200002


//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

bufstream_tt *open_bufstream_write_with_xml_index (
  bufstream_tt *main_bs,
  bufstream_tt *v_xml_bs,
  bufstream_tt *a_xml_bs,
  uint32_t flag,
  void (*DisplayError)(char *txt));

void close_bufstream_write_with_xml_index (
  bufstream_tt* bs,
  int32_t Abort);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
