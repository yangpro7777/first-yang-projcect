/* ----------------------------------------------------------------------------
 * File: buf_wave_read.c
 *
 * Desc: Buffered WAVE file I/O implementation
 *       use to convert wave files to one of the pcmaout data formats and to
 *       convert pcmaout data formats to wave files
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32IO
	#include <sys/stat.h>
	#include <sys/types.h>
#endif

#include "auxinfo.h"
#include "buf_wave.h"


#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM			1
#endif 


static const unsigned char BitReverseTable256[] = 
{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

// AIFF Type Definitions for a wave file
typedef int8_t ID[4];

struct wav_chunk_s
{
	ID ckID;
	uint32_t ckSize;
	ID formType;
};


struct wav_common_chunk_s
{
	ID ckID;
	uint32_t ckSize;
	int16_t audioFormat;
	int16_t numChannels;
	int32_t sampleRate;
	int32_t byteRate;
	int16_t blockAlign;
	int16_t bitsPerSample;
};


struct wav_data_chunk_s
{
	ID ckID;
	uint32_t ckSize;
};

#pragma pack(pop) 


struct wave_variables_s
{
	struct wav_chunk_s wav_chunk;
	struct wav_common_chunk_s wav_common_chunk;
	struct wav_data_chunk_s wav_data_chunk;

	struct buf_pcm_info_s pcm_info;

	uint32_t samples_per_frame[5];

	uint8_t pcm_bytes_per_sample;
	uint32_t pcm_bytes_per_frame[5];
	uint8_t *pcm_buffer;
	uint32_t pcm_idx;
	uint32_t pcm_size;

	uint8_t wav_bytes_per_sample;
	uint32_t wav_bytes_per_frame[5];
	uint8_t *wav_buffer;
	uint32_t wav_idx;
	uint32_t wav_size;

	uint32_t valid_channel_mask;
	uint8_t frame_counter;
};


//implementation structure
struct impl_stream
{
#ifdef WIN32IO
	HANDLE io;
#else
	FILE *io;
#endif
	uint8_t *bfr;
	uint32_t idx;       // read-write index
	uint32_t bfr_size;  // allocated size
	uint32_t bfr_count; // filled size (for read)
	uint32_t chunk_size;
	uint64_t bytecount;
	uint64_t file_size;

	struct wave_variables_s w_vars;
};


//---------------------------------------------------------------------------
//
// process_dvd_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_dvd_lpcm(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t i;

	// just swap the endian of the bytes
	for (i = 0; i < w_vars->pcm_bytes_per_frame[w_vars->frame_counter]; i += 2)
	{
		w_vars->pcm_buffer[i] = w_vars->wav_buffer[i + 1];
		w_vars->pcm_buffer[i + 1] = w_vars->wav_buffer[i];
	}

	return w_vars->pcm_bytes_per_frame[w_vars->frame_counter];
}


//---------------------------------------------------------------------------
//
// process_hdmv_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_hdmv_lpcm(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t i, j, k;

	i = w_vars->pcm_bytes_per_frame[w_vars->frame_counter];

	// HDMV header
	w_vars->pcm_buffer[0] = (uint8_t)((i & 0x0000FF00) >> 8);
	w_vars->pcm_buffer[1] = (uint8_t)(i & 0x000000FF);
	switch (w_vars->wav_common_chunk.numChannels)
	{
	case 2:
		j = 3;
		break;
	case 4:
		j = 4;
		break;
	case 6:
		j = 8;
		break;
	case 8:
		j = 10;
		break;
	default:
		j = 0;
		break;
	}

	if (w_vars->wav_common_chunk.sampleRate == 48000)
		k = 1;
	else if (w_vars->wav_common_chunk.sampleRate == 96000)
		k = 4;
	else
		k = 5;
	w_vars->pcm_buffer[2] = (uint8_t)((j << 4) | k);

	if (w_vars->wav_common_chunk.bitsPerSample == 16)
		j = 1;
	else if (w_vars->wav_common_chunk.bitsPerSample == 20)
		j = 2;
	else
		j = 3;
	w_vars->pcm_buffer[3] = (uint8_t)(j << 6);

	memcpy(&w_vars->pcm_buffer[4], &w_vars->wav_buffer[0], w_vars->pcm_bytes_per_frame[w_vars->frame_counter]);

	return w_vars->pcm_bytes_per_frame[w_vars->frame_counter] + 4;
}


//---------------------------------------------------------------------------
//
// process_302m_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_302m_lpcm(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int i, idx, sample_size;
	unsigned int value;
	int frame;
	int iFrameSize;

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 20:
		sample_size = 1;
		break;
	case 24:
		sample_size = 2;
		break;
	default:
		sample_size = 0;
		break;
	}

	// SMPTE 302M header
	i = w_vars->pcm_bytes_per_frame[w_vars->frame_counter];
	w_vars->pcm_buffer[0] = (uint8_t)((i & 0x0000FF00) >> 8);
	w_vars->pcm_buffer[1] = (uint8_t)(i & 0x000000FF);
	w_vars->pcm_buffer[2] = (uint8_t)(((w_vars->wav_common_chunk.numChannels - 1) >> 1) << 6);
	w_vars->pcm_buffer[3] = (uint8_t)(sample_size << 4);
	idx = 4;

	frame = 0;
	iFrameSize = w_vars->wav_bytes_per_frame[w_vars->frame_counter];

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 16:
		for (i = 0; i < iFrameSize; i += w_vars->wav_bytes_per_sample)
		{
			w_vars->pcm_buffer[idx++] = BitReverseTable256[w_vars->wav_buffer[i + 0]];
			w_vars->pcm_buffer[idx++] = BitReverseTable256[w_vars->wav_buffer[i + 1]];

			value  = BitReverseTable256[w_vars->wav_buffer[i + 2]] << 12;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 3]] << 4;
			if (!frame)
				value |= 0x00900008;
			else
				value |= 0x00800008;

			w_vars->pcm_buffer[idx++] = (unsigned char)((value & 0x00FF0000) >> 16);
			w_vars->pcm_buffer[idx++] = (unsigned char)((value & 0x0000FF00) >> 8);
			w_vars->pcm_buffer[idx++] = (unsigned char) (value & 0x000000FF);
	
			frame++;
			if (frame > 191)
				frame = 0;
		}
		break;

	case 20:
		for (i = 0; i < iFrameSize; i += w_vars->wav_bytes_per_sample)
		{
			value  = BitReverseTable256[w_vars->wav_buffer[i + 0]] << 20;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 1]] << 12;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 2]] << 4;

			if (!frame)
				value |= 0x00000009;
			else
				value |= 0x00000008;

			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0x00FF0000) >> 16);
			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0x0000FF00) >> 8);
			w_vars->pcm_buffer[idx++] = (uint8_t)(value & 0x000000FF);

			value  = BitReverseTable256[w_vars->wav_buffer[i + 3]] << 20;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 4]] << 12;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 5]] << 4;
			value |= 0x00000008;

			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0x00FF0000) >> 16);
			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0x0000FF00) >> 8);
			w_vars->pcm_buffer[idx++] = (uint8_t)(value & 0x000000FF);

			frame++;
			if (frame > 191)
				frame = 0;
		}
		break;

	case 24:
		for (i = 0; i < iFrameSize; i += w_vars->wav_bytes_per_sample)
		{
			w_vars->pcm_buffer[idx++] = BitReverseTable256[w_vars->wav_buffer[i + 0]];
			w_vars->pcm_buffer[idx++] = BitReverseTable256[w_vars->wav_buffer[i + 1]];
			w_vars->pcm_buffer[idx++] = BitReverseTable256[w_vars->wav_buffer[i + 2]];
	
			value  = BitReverseTable256[w_vars->wav_buffer[i + 3]] << 20;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 4]] << 12;
			value |= BitReverseTable256[w_vars->wav_buffer[i + 5]] << 4;

			if (!frame)
				value |= 0x90000008;
			else
				value |= 0x80000008;

			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0xFF000000) >> 24);
			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0x00FF0000) >> 16);
			w_vars->pcm_buffer[idx++] = (uint8_t)((value & 0x0000FF00) >> 8);
			w_vars->pcm_buffer[idx++] = (uint8_t)(value & 0x000000FF);

			frame++;
			if (frame > 191)
				frame = 0;
		}
		break;
	}

	return 0;
}


//---------------------------------------------------------------------------
//
// process_331m_lpcm
//
//---------------------------------------------------------------------------

// always 8 channels, pad extra channels if needed!
static uint32_t process_331m_lpcm(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int i, j, iIdx, oIdx;
	unsigned int value1, value2;
	int frame;
	int samples_per_frame;

	// SMPTE 331M header
	if ((w_vars->pcm_info.video_frame_rate_code == 4) ||		// 29.97fps
		(w_vars->pcm_info.video_frame_rate_code == 7))			// 59.94fps
		w_vars->pcm_buffer[0] = w_vars->frame_counter + 1;
	else
		w_vars->pcm_buffer[0] = 0;

	i = w_vars->samples_per_frame[w_vars->frame_counter];
	w_vars->pcm_buffer[1] =  (uint8_t)(i & 0x000000FF);
	w_vars->pcm_buffer[2] = (uint8_t)((i & 0x0000FF00) >> 8);
	
	w_vars->pcm_buffer[3] = (uint8_t)w_vars->valid_channel_mask;
	oIdx = 4;
	iIdx = 0;

	frame = 0;
	samples_per_frame = w_vars->samples_per_frame[w_vars->frame_counter];

	for (i = 0; i < samples_per_frame; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (j < w_vars->wav_common_chunk.numChannels)
			{
				value1  = w_vars->wav_buffer[iIdx + 0] << 24;
				value1 |= w_vars->wav_buffer[iIdx + 1] << 16;
				value1 |= w_vars->wav_buffer[iIdx + 2] << 8;
	
				value2 = value1 & 0xF0F0F000 >> 12;
				value1 = value1 & 0x0F0F0F00 << 4;
				value1 |= value2 | (j << 24);
	
				if (!frame && !j)
					value1 |= 0x08000000;
	
				w_vars->pcm_buffer[oIdx++] = (uint8_t)((value1 & 0xFF000000) >> 24);
				w_vars->pcm_buffer[oIdx++] = (uint8_t)((value1 & 0x00FF0000) >> 16);
				w_vars->pcm_buffer[oIdx++] = (uint8_t)((value1 & 0x0000FF00) >> 8);
				w_vars->pcm_buffer[oIdx++] = (uint8_t)(value1 & 0x000000FF);
	
				iIdx += 3;
			}
			else
			{
				// pad the remaining channels
				w_vars->pcm_buffer[oIdx++] = 0;
				w_vars->pcm_buffer[oIdx++] = 0;
				w_vars->pcm_buffer[oIdx++] = 0;
				w_vars->pcm_buffer[oIdx++] = 0;
			}
		}

		frame++;
		if (frame > 191)
			frame = 0;
	}

	return w_vars->pcm_bytes_per_frame[w_vars->frame_counter] + 4;
}


//---------------------------------------------------------------------------
//
// process_382m_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_382m_lpcm(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int32_t i, j, iIdx, oIdx;
	uint8_t *ch_off[8];
	int32_t bytes_per_channel;
	int32_t samples_in_frame;

	bytes_per_channel = w_vars->pcm_bytes_per_frame[w_vars->frame_counter] / w_vars->wav_common_chunk.numChannels;
	iIdx = 0;
	for (i = 0; i < w_vars->wav_common_chunk.numChannels; i++)
	{
		ch_off[i] = &w_vars->pcm_buffer[iIdx];
		iIdx += bytes_per_channel;
	}

	iIdx = 0;
	oIdx = 0;
	samples_in_frame = w_vars->samples_per_frame[w_vars->frame_counter];
	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 16:
		for (i = 0; i < samples_in_frame; i++)
		{
			for (j = 0; j < w_vars->wav_common_chunk.numChannels; j++)
			{
				ch_off[j][oIdx + 0] = w_vars->wav_buffer[iIdx + 0];
				ch_off[j][oIdx + 1] = w_vars->wav_buffer[iIdx + 1];
				iIdx += 2;
			}
			oIdx += 2;
		}
		break;

	case 20:
	case 24:
		for (i = 0; i < samples_in_frame; i++)
		{
			for (j = 0; j < w_vars->wav_common_chunk.numChannels; j++)
			{
				ch_off[j][oIdx + 0] = w_vars->wav_buffer[iIdx + 0];
				ch_off[j][oIdx + 1] = w_vars->wav_buffer[iIdx + 1];
				ch_off[j][oIdx + 2] = w_vars->wav_buffer[iIdx + 2];
				iIdx += 3;
			}
			oIdx += 3;
		}
		break;
	}

	return w_vars->pcm_bytes_per_frame[w_vars->frame_counter];
}


//---------------------------------------------------------------------------
//
// get_frame
//
//---------------------------------------------------------------------------

static int32_t get_frame(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t i = 0;

#ifdef WIN32IO
	ReadFile(p->io, w_vars->wav_buffer, w_vars->wav_bytes_per_frame[w_vars->frame_counter], (DWORD*)&i, NULL);
#else
	i = (uint32_t)fread(w_vars->wav_buffer, sizeof(uint8_t), w_vars->wav_bytes_per_frame[w_vars->frame_counter], p->io);
#endif

	if (!i)
		return 1;

	if (i < w_vars->wav_bytes_per_frame[w_vars->frame_counter])
		memset(&w_vars->wav_buffer[i], 0, w_vars->wav_bytes_per_frame[w_vars->frame_counter] - i);

	// convert the audio
	switch (w_vars->pcm_info.audio_format)
	{
	case BS_DVD_LPCM_AUDIO:
		w_vars->pcm_size = process_dvd_lpcm(bs);
		break;
	case BS_HDMV_LPCM_AUDIO:
		w_vars->pcm_size = process_hdmv_lpcm(bs);
		break;
	case BS_AES3_302M_AUDIO:
		w_vars->pcm_size = process_302m_lpcm(bs);
		break;
	case BS_AES3_331M_AUDIO:
		w_vars->pcm_size = process_331m_lpcm(bs);
		break;
	case BS_AES3_382M_AUDIO:
		w_vars->pcm_size = process_382m_lpcm(bs);
		break;
	}

	w_vars->pcm_idx = 0;

	w_vars->frame_counter++;
	if (w_vars->frame_counter > 4)
		w_vars->frame_counter = 0; 

	return 0;
}


//---------------------------------------------------------------------------
//
// fill_buffer
//
//---------------------------------------------------------------------------

static uint32_t fill_buffer(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_read)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t bytes_left = bytes_to_read;
	uint32_t bytes_read = 0;
	uint8_t *bfr = ptr;

	if (w_vars->pcm_idx > 0)
	{
		uint32_t pcm_bytes = w_vars->pcm_size - w_vars->pcm_idx;

		if (pcm_bytes > bytes_left)
		{
			memcpy(bfr, &w_vars->pcm_buffer[w_vars->pcm_idx], bytes_left);
			w_vars->pcm_idx += bytes_left;
			bytes_read += bytes_left;
			return bytes_read;
		}

		memcpy(bfr, &w_vars->pcm_buffer[w_vars->pcm_idx], pcm_bytes);
		w_vars->pcm_idx = 0;
		bytes_read += pcm_bytes;
		bytes_left -= pcm_bytes;
		bfr += pcm_bytes;
	}

	while (bytes_left > 0)
	{
		if (get_frame(bs))
			return bytes_read;

		if (w_vars->pcm_size > bytes_left)
		{
			memcpy(bfr, &w_vars->pcm_buffer[0], bytes_left);
			w_vars->pcm_idx = bytes_left;
			bytes_read += bytes_left;
			return bytes_read;
		}

		memcpy(bfr, &w_vars->pcm_buffer[0], w_vars->pcm_size);
		bytes_read += w_vars->pcm_size;
		bytes_left -= w_vars->pcm_size;
		bfr += w_vars->pcm_size;
	}

	return bytes_read;
}


//---------------------------------------------------------------------------
//
// fr_usable_bytes
//
//---------------------------------------------------------------------------

static uint32_t fr_usable_bytes(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	return p->bfr_count - p->idx;
}


//---------------------------------------------------------------------------
//
// fr_confirm
//
//---------------------------------------------------------------------------

static uint32_t fr_confirm(bufstream_tt *bs, uint32_t numbytes)
{
	bs->Buf_IO_struct->idx       += numbytes;
	bs->Buf_IO_struct->bytecount += numbytes;

	return numbytes;
}


//---------------------------------------------------------------------------
//
// fr_request
//
//---------------------------------------------------------------------------

static uint8_t *fr_request(bufstream_tt *bs, uint32_t numbytes)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	if (p->idx + numbytes <= p->bfr_count)
		return p->bfr + p->idx;

	if (p->idx == p->bfr_count)
	{
		p->idx = 0;
		p->bfr_count = 0;
	}

	if (p->idx + numbytes > p->bfr_size)
	{
		memcpy(p->bfr, p->bfr + p->idx, p->bfr_count - p->idx);
		p->bfr_count -= p->idx;
		p->idx = 0;
	}

	p->bfr_count += fill_buffer(bs, p->bfr + p->bfr_count, p->bfr_size - p->bfr_count);

	if (p->idx + numbytes <= p->bfr_count)
		return p->bfr + p->idx;
	else
		return NULL;
}


//---------------------------------------------------------------------------
//
// fr_copybytes
//
//---------------------------------------------------------------------------

static uint32_t fr_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	if (p->bfr_count != p->idx)
	{
		if (p->idx + numbytes <= p->bfr_count)
		{
			memcpy(ptr, p->bfr + p->idx, numbytes);
			p->idx += numbytes;
			p->bytecount +=numbytes;
			return numbytes;
		}
		else
		{
			int32_t c, i;
			memcpy(ptr, p->bfr + p->idx, p->bfr_count - p->idx);
			c = numbytes - (p->bfr_count - p->idx);
			p->bytecount += p->bfr_count - p->idx;

			p->idx = 0;
			p->bfr_count = 0;

			i = fill_buffer(bs, ptr + numbytes - c, c);

			p->bytecount += i;
			c -= i;

			return numbytes - c;
		}
	}
	else
	{
		uint32_t n;

		p->idx = 0;
		p->bfr_count = 0;

		n = fill_buffer(bs, ptr, numbytes);

		p->bytecount += n;
	
		return n;
	}

//	return 0;  // remove compile warning
}


//---------------------------------------------------------------------------
//
// fr_chunksize
//
//---------------------------------------------------------------------------

static uint32_t fr_chunksize(bufstream_tt *bs)
{
	return bs->Buf_IO_struct->chunk_size;
}


//---------------------------------------------------------------------------
//
// fr_auxinfo
//
//---------------------------------------------------------------------------

static uint32_t fr_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
	uint64_t *ptr;

    if (offs){};  // remove compile warning

	switch(info_ID)
	{
	case BYTECOUNT_INFO:
		ptr = (uint64_t*)info_ptr;
		if (ptr && (info_size == sizeof(uint64_t)))
			*ptr = bs->Buf_IO_struct->bytecount;
		break;

	case FILESIZE_INFO:
		ptr = (uint64_t*)info_ptr;
		if(ptr && (info_size == sizeof(uint64_t)))
			*ptr = bs->Buf_IO_struct->file_size;
		break;
	}

	return BS_OK;
}


//---------------------------------------------------------------------------
//
// fr_split
//
//---------------------------------------------------------------------------

static uint32_t fr_split(bufstream_tt *bs)
{
    if (!bs){};  // remove compile warning
	return BS_ERROR;
}


//---------------------------------------------------------------------------
//
// fr_done
//
//---------------------------------------------------------------------------

static void fr_done(bufstream_tt *bs, int32_t abort)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	if (!p)
		return;

    if (!abort){};  // remove compile warning

    if (p->io)
#ifdef WIN32IO
		CloseHandle(p->io);
#else
		fclose(p->io);
#endif

	if (p->bfr)
		free(p->bfr);
	if (p->w_vars.pcm_buffer)
		free(p->w_vars.pcm_buffer);
	if (p->w_vars.wav_buffer)
		free(p->w_vars.wav_buffer);

	free(p);

	bs->Buf_IO_struct = NULL;
}


//---------------------------------------------------------------------------
//
// fr_free
//
//---------------------------------------------------------------------------

static void fr_free(bufstream_tt *bs)
{
	if (!bs)
		return;

	if (bs->Buf_IO_struct)
		bs->done(bs, 0);

	free(bs);
}



//---------------------------------------------------------------------------
//
// check_wave_input
//
//---------------------------------------------------------------------------

static int32_t check_wave_input(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;

	if ((w_vars->pcm_info.audio_format < BS_DVD_LPCM_AUDIO) || (w_vars->pcm_info.audio_format > BS_AES3_382M_AUDIO))
		return 1;

	if ((w_vars->wav_common_chunk.sampleRate != 48000) && (w_vars->wav_common_chunk.sampleRate != 96000))
		return 1;

	if ((w_vars->wav_common_chunk.numChannels < 1) || (w_vars->wav_common_chunk.numChannels > 8))
		return 1;

	if ((w_vars->pcm_info.audio_format == BS_AES3_302M_AUDIO) ||
		(w_vars->pcm_info.audio_format == BS_HDMV_LPCM_AUDIO))
	{
		if (w_vars->wav_common_chunk.numChannels % 2)
			return 1;
	}

	if (w_vars->pcm_info.audio_format == BS_DVD_LPCM_AUDIO)
	{
		// currently only 16 bits supported
		if (w_vars->wav_common_chunk.bitsPerSample != 16)
			return 1;

		if ((w_vars->wav_common_chunk.numChannels < 1) || (w_vars->wav_common_chunk.numChannels > 2))
			return 1;
	}
	else if (w_vars->pcm_info.audio_format == BS_AES3_331M_AUDIO)
	{
		if ((w_vars->wav_common_chunk.bitsPerSample != 20) &&
			(w_vars->wav_common_chunk.bitsPerSample != 24))
			return 1;
	}
	else
	{
		// 302M or 382M
		if ((w_vars->wav_common_chunk.bitsPerSample != 16) &&
			(w_vars->wav_common_chunk.bitsPerSample != 20) &&
			(w_vars->wav_common_chunk.bitsPerSample != 24))
			return 1;
	}

	return 0;
}


//---------------------------------------------------------------------------
//
// read_wave_headers
//
//---------------------------------------------------------------------------

static int32_t read_wave_headers(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t i;
	uint8_t emphasis = 0;
	uint8_t mute_flag = 0;
	uint8_t drc = 0x80;
	uint8_t sample_size;

#ifdef WIN32IO
	ReadFile(p->io, &w_vars->wav_chunk, sizeof(struct wav_chunk_s), (DWORD*)&i, NULL);
#else
	i = (uint32_t)fread(&w_vars->wav_chunk, sizeof(uint8_t), sizeof(struct wav_chunk_s), p->io);
#endif

	if (i != sizeof(struct wav_chunk_s))
		return 1;

#ifdef WIN32IO
	ReadFile(p->io, &w_vars->wav_common_chunk, sizeof(struct wav_common_chunk_s), (DWORD*)&i, NULL);
#else
	i = (uint32_t)fread(&w_vars->wav_common_chunk, sizeof(uint8_t), sizeof(struct wav_common_chunk_s), p->io);
#endif

	if (i != sizeof(struct wav_common_chunk_s))
		return 1;

#ifdef WIN32IO
	ReadFile(p->io, &w_vars->wav_data_chunk, sizeof(struct wav_data_chunk_s), (DWORD*)&i, NULL);
#else
	i = (uint32_t)fread(&w_vars->wav_data_chunk, sizeof(uint8_t), sizeof(struct wav_data_chunk_s), p->io);
#endif

	if (i != sizeof(struct wav_data_chunk_s))
		return 1;

	if (check_wave_input(bs))
		return 1;

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 16:
		sample_size = 0;
		break;
	case 20:
		sample_size = 1;
		break;
	case 24:
		sample_size = 2;
		break;
	default:
		return 1;
	}

	if (w_vars->pcm_info.audio_format == BS_DVD_LPCM_AUDIO)
	{
		// make a dummy LPCM header like the one pcmaout generates
		// make the first four bytes 'LPCM'
		p->bfr[0] = 'L';
		p->bfr[1] = 'P';
		p->bfr[2] = 'C';
		p->bfr[3] = 'M';

		if (w_vars->wav_common_chunk.sampleRate == 48000)
			p->bfr[4] = 0;
		else
			p->bfr[4] = 1;

		if (emphasis)
			p->bfr[4] |= 2;

		p->bfr[4] |= (mute_flag & 0x1) << 7;
		p->bfr[4] |= (sample_size & 0x3) << 5;
		p->bfr[4] |= ((w_vars->wav_common_chunk.numChannels - 1) & 0x7) << 2;
		p->bfr[5] = drc;
		p->bfr[6] = 0;		// future use
		p->bfr[7] = 0;		// future use
	}
	else if (w_vars->pcm_info.audio_format == BS_HDMV_LPCM_AUDIO)
	{
		// send a 8-byte dummy frame with the HDMV parameters for the muxer
		// make the first four bytes 'HDMV'
		p->bfr[0]  = 'H';
		p->bfr[1]  = 'D';
		p->bfr[2]  = 'M';
		p->bfr[3]  = 'V';
		p->bfr[4]  = 13;	// means HDMV
		if (w_vars->wav_common_chunk.sampleRate == 48000)
			p->bfr[5] = 0x00;
		else if (w_vars->wav_common_chunk.sampleRate == 96000)
			p->bfr[5] = 0x20;
		else
			p->bfr[5] = 0x40;
		p->bfr[5] |= (sample_size & 0x3) << 3;
		p->bfr[5] |=  (w_vars->wav_common_chunk.numChannels - 1) & 0x7;
		p->bfr[6]  = 0; // future use
		p->bfr[7]  = 0; // future use
	}
	else
	{
		// send a 8-byte dummy frame with the AES3 parameters for the muxer
		// make the first four bytes 'AES3'
		p->bfr[0]  = 'A';
		p->bfr[1]  = 'E';
		p->bfr[2]  = 'S';
		p->bfr[3]  = '3';
		if (w_vars->pcm_info.audio_format == BS_AES3_302M_AUDIO)
			p->bfr[4]  = 10;	// 302M
		else if (w_vars->pcm_info.audio_format == BS_AES3_331M_AUDIO)
			p->bfr[4]  = 11;	// 331M
		else
			p->bfr[4]  = 12;	// 382M

		if (w_vars->pcm_info.video_frame_rate_code > 0)
			p->bfr[5]  = (w_vars->pcm_info.video_frame_rate_code - 1) << 5;
		else
			p->bfr[5]  = 3 << 5;	// 29.97fps
		p->bfr[5] |= (sample_size & 0x3) << 3;
		p->bfr[5] |=  (w_vars->wav_common_chunk.numChannels - 1) & 0x7;
		p->bfr[6]  = 0; // future use
		p->bfr[7]  = 0; // future use
	}

	p->bfr_count = 8;

	return 0;
}


//---------------------------------------------------------------------------
//
// setup_audio_buffers
//
//---------------------------------------------------------------------------

static int32_t setup_audio_buffers(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int32_t i;

	if (w_vars->pcm_info.audio_format == BS_DVD_LPCM_AUDIO)
	{
		if (w_vars->wav_common_chunk.sampleRate == 48000)
			w_vars->samples_per_frame[0] = 80;
		else
			w_vars->samples_per_frame[0] = 160;

		w_vars->pcm_bytes_per_frame[0] = w_vars->samples_per_frame[0] << 1;
		w_vars->pcm_bytes_per_sample = 2; // 16 bits

		if (w_vars->wav_common_chunk.numChannels > 1)
		{
			w_vars->pcm_bytes_per_frame[0] <<= w_vars->wav_common_chunk.numChannels - 1;
			w_vars->pcm_bytes_per_sample <<= w_vars->wav_common_chunk.numChannels - 1;
		}
	}
	else if (w_vars->pcm_info.audio_format == BS_HDMV_LPCM_AUDIO)
	{
		if (w_vars->wav_common_chunk.sampleRate == 48000)
			w_vars->samples_per_frame[0] = 240;
		else if (w_vars->wav_common_chunk.sampleRate == 96000)
			w_vars->samples_per_frame[0] = 480;
		else
			w_vars->samples_per_frame[0] = 960;

		w_vars->samples_per_frame[1] = w_vars->samples_per_frame[0];
		w_vars->samples_per_frame[2] = w_vars->samples_per_frame[0];
		w_vars->samples_per_frame[3] = w_vars->samples_per_frame[0];
		w_vars->samples_per_frame[4] = w_vars->samples_per_frame[0];

		switch (w_vars->wav_common_chunk.bitsPerSample)
		{
		case 20:
			w_vars->pcm_bytes_per_sample = 3;
			w_vars->wav_bytes_per_sample = 3;
			break;
		case 24:
			w_vars->pcm_bytes_per_sample = 3;
			w_vars->wav_bytes_per_sample = 3;
			break;
		default: // 16-bit
			w_vars->pcm_bytes_per_sample = 2;
			w_vars->wav_bytes_per_sample = 2;
			break;
		}

		if (w_vars->wav_common_chunk.numChannels > 1)
		{
			w_vars->pcm_bytes_per_sample = w_vars->pcm_bytes_per_sample * (uint8_t)w_vars->wav_common_chunk.numChannels;
			w_vars->wav_bytes_per_sample = w_vars->wav_bytes_per_sample * (uint8_t)w_vars->wav_common_chunk.numChannels;
		}
	}
	else
	{
		switch (w_vars->pcm_info.video_frame_rate_code)
		{
		case 1:		// 23.976fps
			w_vars->samples_per_frame[0] = 2002;
			w_vars->samples_per_frame[1] = 2002;
			w_vars->samples_per_frame[2] = 2002;
			w_vars->samples_per_frame[3] = 2002;
			w_vars->samples_per_frame[4] = 2002;
			break;
		case 2:		// 24fps
			w_vars->samples_per_frame[0] = 2000;
			w_vars->samples_per_frame[1] = 2000;
			w_vars->samples_per_frame[2] = 2000;
			w_vars->samples_per_frame[3] = 2000;
			w_vars->samples_per_frame[4] = 2000;
			break;
		case 3:		// 25fps
			w_vars->samples_per_frame[0] = 1920;
			w_vars->samples_per_frame[1] = 1920;
			w_vars->samples_per_frame[2] = 1920;
			w_vars->samples_per_frame[3] = 1920;
			w_vars->samples_per_frame[4] = 1920;
			break;
		case 4:		// 29.97fps
			w_vars->samples_per_frame[0] = 1602;
			w_vars->samples_per_frame[1] = 1601;
			w_vars->samples_per_frame[2] = 1602;
			w_vars->samples_per_frame[3] = 1601;
			w_vars->samples_per_frame[4] = 1602;
			break;
		case 5:		// 30fps
			w_vars->samples_per_frame[0] = 1600;
			w_vars->samples_per_frame[1] = 1600;
			w_vars->samples_per_frame[2] = 1600;
			w_vars->samples_per_frame[3] = 1600;
			w_vars->samples_per_frame[4] = 1600;
			break;
		case 6:		// 50fps
			w_vars->samples_per_frame[0] = 960;
			w_vars->samples_per_frame[1] = 960;
			w_vars->samples_per_frame[2] = 960;
			w_vars->samples_per_frame[3] = 960;
			w_vars->samples_per_frame[4] = 960;
			break;
		case 7:		// 59.94fps
			w_vars->samples_per_frame[0] = 801;
			w_vars->samples_per_frame[1] = 801;
			w_vars->samples_per_frame[2] = 800;
			w_vars->samples_per_frame[3] = 801;
			w_vars->samples_per_frame[4] = 801;
			break;
		case 8:		// 60fps
			w_vars->samples_per_frame[0] = 800;
			w_vars->samples_per_frame[1] = 800;
			w_vars->samples_per_frame[2] = 800;
			w_vars->samples_per_frame[3] = 800;
			w_vars->samples_per_frame[4] = 800;
			break;
		}

		if (w_vars->pcm_info.audio_format == BS_AES3_382M_AUDIO)
		{
			// do it SMPTE 382M style
			switch (w_vars->wav_common_chunk.bitsPerSample)
			{
			case 20:
				w_vars->pcm_bytes_per_sample = 3;
				w_vars->wav_bytes_per_sample = 3;
				break;
			case 24:
				w_vars->pcm_bytes_per_sample = 3;
				w_vars->wav_bytes_per_sample = 3;
				break;
			default: // 16-bit
				w_vars->pcm_bytes_per_sample = 2;
				w_vars->wav_bytes_per_sample = 2;
				break;
			}

			if (w_vars->wav_common_chunk.numChannels > 1)
			{
				w_vars->pcm_bytes_per_sample = w_vars->pcm_bytes_per_sample * (uint8_t)w_vars->wav_common_chunk.numChannels;
				w_vars->wav_bytes_per_sample = w_vars->wav_bytes_per_sample * (uint8_t)w_vars->wav_common_chunk.numChannels;
			}
		}
		else if (w_vars->pcm_info.audio_format == BS_AES3_331M_AUDIO)
		{
			// do it SMPTE 331M style
	
			// 20 or 24 bits
			w_vars->pcm_bytes_per_sample = 4;
			w_vars->wav_bytes_per_sample = 3;
	
			// always output 8 channels!
			w_vars->pcm_bytes_per_sample <<= 3;
			if (w_vars->wav_common_chunk.numChannels > 1)
				w_vars->wav_bytes_per_sample = w_vars->wav_bytes_per_sample * (uint8_t)w_vars->wav_common_chunk.numChannels;
	
			for (i = 0; i < w_vars->wav_common_chunk.numChannels - 1; i++)
				w_vars->valid_channel_mask |= (1 << i);
		}
		else
		{
			// do it SMPTE 302M style
			switch (w_vars->wav_common_chunk.bitsPerSample)
			{
			case 20:
				w_vars->pcm_bytes_per_sample = 6;   // for 2 channels!
				w_vars->wav_bytes_per_sample = 6; // for 2 channels!
				break;
			case 24:
				w_vars->pcm_bytes_per_sample = 7;   // for 2 channels!
				w_vars->wav_bytes_per_sample = 6; // for 2 channels!
				break;
			default: // 16-bit
				w_vars->pcm_bytes_per_sample = 5;   // for 2 channels!
				w_vars->wav_bytes_per_sample = 4; // for 2 channels!
				break;
			}

			if (w_vars->wav_common_chunk.numChannels > 2)
			{
				w_vars->pcm_bytes_per_sample = w_vars->pcm_bytes_per_sample * (uint8_t)(w_vars->wav_common_chunk.numChannels - 2); 
				w_vars->wav_bytes_per_sample = w_vars->wav_bytes_per_sample * (uint8_t)(w_vars->wav_common_chunk.numChannels - 2);
			}
		}
	}

	for (i = 0; i < 5; i++)
	{
		w_vars->pcm_bytes_per_frame[i] = w_vars->samples_per_frame[i] * w_vars->pcm_bytes_per_sample;
		w_vars->wav_bytes_per_frame[i] = w_vars->samples_per_frame[i] * w_vars->wav_bytes_per_sample;
	}

	i = w_vars->pcm_bytes_per_frame[0];
	if ((w_vars->pcm_info.audio_format == BS_AES3_302M_AUDIO) ||
		(w_vars->pcm_info.audio_format == BS_AES3_331M_AUDIO))
		i += 4; // for the SMPTE headers

	if (w_vars->pcm_info.audio_format == BS_HDMV_LPCM_AUDIO)
		i += 4; // for the HDMV header

	w_vars->pcm_buffer = (unsigned char*)malloc(i);
	if (!w_vars->pcm_buffer)
		return 1;

	if ((w_vars->pcm_info.audio_format != BS_DVD_LPCM_AUDIO) &&
		(w_vars->pcm_info.audio_format != BS_HDMV_LPCM_AUDIO))
	{
		w_vars->wav_buffer = (unsigned char*)malloc(w_vars->wav_bytes_per_frame[0]);
		if (!w_vars->wav_buffer)
			return 1;
	}

	return 0;
}


int buf_wave_get_bytes_per_frame(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;

	return w_vars->pcm_bytes_per_frame[w_vars->frame_counter];
}


//---------------------------------------------------------------------------
//
// init_file_buf_read
//
//---------------------------------------------------------------------------

#ifdef _BS_UNICODE
static int32_t init_file_buf_read(bufstream_tt *bs,
									const wchar_t* bs_filename,
									uint32_t buf_size,
									struct buf_pcm_info_s *pcm_info)
#else
static int32_t init_file_buf_read(bufstream_tt *bs,
									const char *bs_filename,
									uint32_t buf_size,
									struct buf_pcm_info_s *pcm_info)
#endif
{
#ifdef WIN32IO
	BY_HANDLE_FILE_INFORMATION file_info;
#else
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__QNX__)
	struct _stati64 stat_data;
#else
#if defined(__QNX__)
	struct stat64 stat_data;
#else
	struct stat stat_data;
#endif
#endif
	int32_t fde;
#endif

	bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
	if (!bs->Buf_IO_struct)
		return BS_ERROR;

#ifdef WIN32IO
	if ((bs->Buf_IO_struct->io = 
#ifdef _BS_UNICODE
		CreateFileW(bs_filename,
#else 
		CreateFile(bs_filename,
#endif
					GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
					NULL)) == INVALID_HANDLE_VALUE)
#else

#ifdef _BS_UNICODE
	bs->Buf_IO_struct->io = _wfopen(bs_filename, L"rb");
#else
#if !defined(__QNX__)
	bs->Buf_IO_struct->io = fopen(bs_filename, "rb");
#else
	bs->Buf_IO_struct->io = fopen64(bs_filename, "rb");
#endif
#endif
#endif
	if (!bs->Buf_IO_struct->io)
		return BS_ERROR;

#ifdef WIN32IO
	if (!GetFileInformationByHandle(bs->Buf_IO_struct->io, &file_info))
		return BS_ERROR;

	bs->Buf_IO_struct->file_size = ((uint64_t)file_info.nFileSizeHigh << 32) | file_info.nFileSizeLow;
#else
#ifdef _BS_UNICODE
	fde = _wstati64(bs_filename, &stat_data);
#else
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__QNX__)
	fde = _stati64(bs_filename, &stat_data);
#else
#if defined(__QNX__)
	fde = stat64(bs_filename, &stat_data);
#else
	fde = stat(bs_filename,&stat_data);
#endif
#endif
#endif

	if (fde < 0)
		return BS_ERROR;

	bs->Buf_IO_struct->file_size = stat_data.st_size;
#endif

	bs->Buf_IO_struct->bfr = (uint8_t*)malloc(buf_size);
	if (!bs->Buf_IO_struct->bfr)
		return BS_ERROR;

	bs->Buf_IO_struct->bfr_size		= buf_size;
	bs->Buf_IO_struct->bfr_count	= 0;
	bs->Buf_IO_struct->chunk_size	= buf_size;
	bs->Buf_IO_struct->idx			= 0;
	bs->Buf_IO_struct->bytecount	= 0;

	memset(&bs->Buf_IO_struct->w_vars, 0, sizeof(struct wave_variables_s));
	memcpy(&bs->Buf_IO_struct->w_vars.pcm_info, pcm_info, sizeof(struct buf_pcm_info_s));

	bs->usable_bytes	= fr_usable_bytes;
	bs->request			= fr_request;
	bs->confirm			= fr_confirm;
	bs->copybytes		= fr_copybytes;
	bs->split			= fr_split;
	bs->chunksize		= fr_chunksize;
	bs->free			= fr_free;
	bs->auxinfo			= fr_auxinfo;
	bs->done			= fr_done;
	bs->drive_ptr		= NULL;
	bs->drive			= NULL;
	bs->state			= 0;
	bs->flags			= 0;

	// now read the wave headers
	if (read_wave_headers(bs))
		return BS_ERROR;

	if (setup_audio_buffers(bs))
		return BS_ERROR;

	return BS_OK;
}


//---------------------------------------------------------------------------
//
// open_wave_buf_read
//
//---------------------------------------------------------------------------

#ifdef _BS_UNICODE
bufstream_tt *open_wave_buf_read(const wchar_t* bs_filename,
								uint32_t buf_size,
								struct buf_pcm_info_s *pcm_info)
#else
bufstream_tt *open_wave_buf_read(const char *bs_filename,
								uint32_t buf_size,
								struct buf_pcm_info_s *pcm_info)
#endif
{
	bufstream_tt *p;

	if (!bs_filename || !pcm_info)
		return NULL;

	p = (bufstream_tt*)malloc(sizeof(bufstream_tt));
	if (p)
	{
		if (BS_OK != init_file_buf_read(p, bs_filename, buf_size, pcm_info))
		{
			fr_free(p);
			p = NULL;
		}
	}
	return p;
}


//---------------------------------------------------------------------------
//
// close_wave_buf_read
//
//---------------------------------------------------------------------------

void close_wave_buf_read(bufstream_tt* bs, int32_t abort)
{
	bs->done(bs, abort);
	bs->free(bs);
}

