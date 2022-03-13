/* ----------------------------------------------------------------------------
 * File: buf_rw.h
 *
 * Desc: Read/Write bufstream implementation
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

typedef void *(*BS_fnGetRC)(char* pName);
typedef void *(*BS_fnMemAlloc)(unsigned int size);
typedef	void  (*BS_fnMemFree)(void *pMemBlk);

#define BS_FILE_WRITE_MODE	0
#define BS_FILE_READ_MODE	1

#define BS_SEEK_START		0
#define BS_SEEK_CURRENT		1
#define BS_SEEK_END			2

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _BS_UNICODE

bufstream_tt *open_file_buf_rw(const wchar_t *pFilename,
							   uint32_t bufsize,
							   uint8_t initialMode,
							   BS_fnGetRC fnGetRC);

#else

bufstream_tt *open_file_buf_rw(const char *pFilename,
							   uint32_t bufsize,
							   uint8_t initialMode,
							   BS_fnGetRC fnGetRC);
#endif

#ifdef __cplusplus
}
#endif
