/* ----------------------------------------------------------------------------
 * File: buf_tmp_file.h
 *
 * Desc: writes to temporary file.
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

bufstream_tt * open_buf_tmp_file(const char * filename, uint32_t chunksize, void (*DisplayError)(char *txt));
void close_tmp_buf(bufstream_tt* bs, int32_t Abort);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
