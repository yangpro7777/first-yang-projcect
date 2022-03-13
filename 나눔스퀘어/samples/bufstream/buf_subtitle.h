/* ----------------------------------------------------------------------------
 * File: buf_subtitle.h
 *
 * Desc: Buffered Subtitle file I/O header
 *       use to read/write text based subtitle files
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
#include "mcmediatypes.h"


struct buf_subtitle_info_s
{
	mc_stream_format_t *pFormat;
};


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _BS_UNICODE
bufstream_tt *open_subtitle_buf_write(const wchar_t *bs_filename, const wchar_t *vsub_filename, struct buf_subtitle_info_s *subtitle_info);
#else
bufstream_tt *open_subtitle_buf_write(const char *bs_filename, const char *vsub_filename, struct buf_subtitle_info_s *subtitle_info);
#endif

void close_subtitle_buf_write(bufstream_tt* bs, int32_t abort);

#ifdef __cplusplus
}
#endif
