/* ----------------------------------------------------------------------------
 * File: buf_null.h
 *
 * Desc: Empty buffered stream IO for first pass of 2-pass operation
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

// no output is actually generated with this bufstream class
// use for first pass of 2-passes

// open_null_buf_write() - close_file_buf() are designed to be used
// by "normal" application to replace fopen-fclose pair
//

#ifdef __cplusplus
extern "C" {
#endif

bufstream_tt *open_null_buf_write(
  uint32_t bufsize, void (*DisplayError)(char *txt));

void close_null_buf(bufstream_tt* bs, int32_t Abort);

#ifdef __cplusplus
}
#endif
