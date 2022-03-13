/* ----------------------------------------------------------------------------
 * File: buf_single.h
 *
 * Desc: Stream Buffer to single-frame implementation
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

// callback to the application, this can be modified if needed
typedef void (*buf_single_app_callback)(void *app_structure, uint8_t *bfr, int32_t num_bytes_in_buffer);


#ifdef __cplusplus
extern "C" {
#endif

// call to open a bufstream to use with the encoders
bufstream_tt *open_mem_buf_single(uint8_t *buffer,
                                  uint32_t bufsize,
                                  uint32_t chunk_size,
                                  uint8_t is_muxed_stream,
                                  void *app_structure,
                                  buf_single_app_callback app_callback);

// call to close a bufstream
void close_mem_buf_single(bufstream_tt *bs);

#ifdef __cplusplus
}
#endif
