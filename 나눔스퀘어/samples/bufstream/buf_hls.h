/* ----------------------------------------------------------------------------
 * File: buf_hls.h
 *
 * Desc: Captures segment output from muxers and creates a HTTP Live Streaming
 *       playlist
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

bufstream_tt *open_bufstream_write_with_hls (
  bufstream_tt *main_bs,
  const wchar_t *uri_path,        // URI path for the playlist items, filename will be appended
  const wchar_t *playlist_name);

#else

bufstream_tt *open_bufstream_write_with_hls (
  bufstream_tt *main_bs,
  const char *uri_path,           // URI path for the playlist items, filename will be appended
  const char *playlist_name);

#endif

void close_bufstream_write_with_hls (
  bufstream_tt* bs, 
  int32_t Abort);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
