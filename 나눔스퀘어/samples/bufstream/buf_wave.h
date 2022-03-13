/* ----------------------------------------------------------------------------
 * File: buf_wave.h
 *
 * Desc: Buffered WAVE file I/O header
 *       use to convert wave files to one of the pcmaout data formats and to
 *       convert pcmaout data formats to wave files
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

// Configuration Option
// sometimes can improve performance on Win32 - platform
//#define WIN32IO

#ifdef WIN32IO
	#ifdef __GNUC__
		#warning WIN32IO activated, -fomit-frame-pointer must be disabled
	#else
		#pragma message("WIN32IO activated")
	#endif
	#include <windows.h>
#endif


#define BS_DVD_LPCM_AUDIO				0
#define BS_HDMV_LPCM_AUDIO				1
#define BS_AES3_302M_AUDIO				2
#define BS_AES3_331M_AUDIO				3
#define BS_AES3_382M_AUDIO				4
#define BS_TWOS_LPCM_AUDIO				5
#define BS_QT_PCM_AUDIO					6

#define BS_PCM_FLAGS_IS_FLOAT			0x01	// samples are floating point, else integer
#define BS_PCM_FLAGS_IS_BIG_ENDIAN		0x02	// samples are big endian, else little endian
#define BS_PCM_FLAGS_IS_SIGNED			0x04	// samples are signed integers, else unsigned integers
#define BS_PCM_FLAGS_IS_PACKED			0x08	// sample bits occupy the entire available bits for the channel
												// else they are high- or low-aligned within the channel
#define BS_PCM_FLAGS_IS_ALIGNED_HIGH	0x10	// sample bits are placed into the high bits of the channel
												// else low bit placement
#define BS_PCM_FLAGS_IS_NON_INTERLEAVED	0x20	// channels are not interleaved, else channels are interleaved
#define BS_PCM_FLAGS_IS_NON_MIXABLE		0x40	// format is nonmixable


struct buf_pcm_info_s
{
	uint8_t audio_format;			// 0 = DVD LPCM
									// 1 = HDMV LPCM audio, Blu Ray and AVCHD
									// 2 = AES3 302M
									// 3 = AES3 331M
									// 4 = AES3 382M
									// 5 = TWOS LPCM audio
									// 6 = QT LPCM audio
	uint8_t video_frame_rate_code;	// 1 .. 8, same as mpeg video frame rate code
};


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _BS_UNICODE

bufstream_tt *open_wave_buf_write(const wchar_t *bs_filename);

bufstream_tt *open_wave_buf_read(const wchar_t *bs_filename,
						uint32_t buf_size,
						struct buf_pcm_info_s *pcm_info);
#else

bufstream_tt *open_wave_buf_write(const char *bs_filename);

bufstream_tt *open_wave_buf_read(const char *bs_filename,
						uint32_t buf_size,
						struct buf_pcm_info_s *pcm_info);
#endif


void close_wave_buf_write(bufstream_tt* bs, int32_t abort);

void close_wave_buf_read(bufstream_tt* bs, int32_t abort);

int buf_wave_get_bytes_per_frame(bufstream_tt *bs);

#ifdef __cplusplus
}
#endif
