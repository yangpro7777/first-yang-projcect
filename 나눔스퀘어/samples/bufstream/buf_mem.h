/* ----------------------------------------------------------------------------
 * File: buf_mem.h
 *
 * Desc: Buffered stream memory IO
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

// open_file_buf_XXXX() - close_file_buf() are designed to be used
// by "normal" application to replace fopen-fclose pair
//

#ifdef __cplusplus
extern "C" {
#endif

bufstream_tt *open_mem_buf_write(
  uint8_t *buffer, 
  uint32_t bufsize, 
  void (*DisplayError)(char *txt));

void close_mem_buf(bufstream_tt* bs, int32_t Abort);

#ifdef __cplusplus
}
#endif
