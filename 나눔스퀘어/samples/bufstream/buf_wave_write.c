/* ----------------------------------------------------------------------------
 * File: buf_wave_write.c
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

#include "bufstrm.h"
#include "auxinfo.h"
#include "buf_wave.h"


#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM			1
#endif 

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT	3
#endif

#define WAVE_BUFFER_SIZE		1024 * 1024
#define PCM_BUFFER_SIZE			1024 * 1024


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

	struct buf_pcm_info_s wave_info;

	uint8_t wav_buffer[WAVE_BUFFER_SIZE];

	uint8_t wave_headers_written;

	uint8_t encoder_mode;
	uint8_t aes3_channel;
	uint32_t aes3_payload_length;

	uint8_t num_channels;
	int32_t sample_rate;
	uint8_t bits_per_sample;

	uint32_t qt_lpcm_flags;

	uint32_t total_sample_bytes;
};


//implementation structure
struct impl_stream
{
#ifdef WIN32IO
	HANDLE io;
#else
	FILE *io;
#endif
	uint8_t bfr[PCM_BUFFER_SIZE];
	uint32_t idx;       // read-write index
	uint32_t bfr_size;  // allocated size
	uint32_t chunk_size;
	uint64_t bytecount;
	uint64_t file_size;

	struct wave_variables_s w_vars;
};


// bitstream structure
typedef struct buf_bs_s
{
	uint8_t *rdbfr;
	uint8_t *rdptr;
	uint8_t *rdmax;
	uint32_t bfr;
	int32_t incnt;
	uint8_t end_of_stream;
	uint8_t bytecnt;

} buf_bs_t;


static void bs_flush_buffer(buf_bs_t *bs, int32_t N)
{
	int32_t incnt1;
	uint8_t byt;

	if (N < 32)
		bs->bfr <<= N;
	else
		bs->bfr = 0;

	incnt1 = bs->incnt -= N;

	if (incnt1 <= 24)
	{
		if (bs->rdptr < bs->rdmax - 4)
		{
			do
			{
				byt = *bs->rdptr++;
				bs->bfr |= byt << (24 - incnt1);
				incnt1 += 8;
				bs->bytecnt++;
			}
			while (incnt1 <= 24);
		}
		else
		{
			do
			{
				if (bs->rdptr >= bs->rdmax)
				{
					bs->end_of_stream = 1;
					return;
				}
	
				byt = *bs->rdptr++;
				bs->bfr |= byt << (24 - incnt1);
				bs->bytecnt++;
				incnt1 += 8;
			}
			while (incnt1 <= 24);
		}
		bs->incnt = incnt1;
	}
}


static int32_t bs_init(buf_bs_t *bs, uint8_t *ptr, int32_t len)
{
	bs->rdbfr = ptr;
	bs->rdptr = ptr;
	bs->rdmax = ptr + len;
	bs->bfr = 0;
	bs->incnt = 0;
	bs->end_of_stream = 0;
	bs->bytecnt = 0;
	bs_flush_buffer(bs, 0);

	return 0;
}


static uint32_t bs_get_bits(buf_bs_t *bs, int32_t N)
{
	uint32_t val;

	val = bs->bfr >> (32 - N);
	bs_flush_buffer(bs, N);

	return val;
}


//---------------------------------------------------------------------------
//
// process_dvd_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_dvd_lpcm(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *pcm_buffer = ptr;
	uint8_t *chan_ptr1[8], *chan_ptr2[8];
	uint32_t i, chan;
	int32_t bytes_per_sample;
	buf_bs_t bbs;

	switch (w_vars->bits_per_sample)
	{
	case 16:
		// just swap the endian of the bytes
		for (i = 0; i < bytes_to_process; i += 2)
		{
			w_vars->wav_buffer[i] = pcm_buffer[i + 1];
			w_vars->wav_buffer[i + 1] = pcm_buffer[i];
		}
		break;

	case 20:
		// setup the channel pointers
		bytes_per_sample = w_vars->num_channels * 3;
		for (i = 0; i < w_vars->num_channels; i++)
		{
			chan_ptr1[i] = w_vars->wav_buffer + (i * 3);
			chan_ptr2[i] = chan_ptr1[i] + bytes_per_sample;
		}

		// setup bitstream
		bs_init(&bbs, pcm_buffer, bytes_to_process);

		i = bytes_to_process;
		while (i > 0)
		{
			// get 16 bits for each channel for the first sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr1[chan][2] = bs_get_bits(&bbs, 8);
				chan_ptr1[chan][1] = bs_get_bits(&bbs, 8);
				i -= 2;
			}

			// get 16 bits for each channel for the second sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr2[chan][2] = bs_get_bits(&bbs, 8);
				chan_ptr2[chan][1] = bs_get_bits(&bbs, 8);
				i -= 2;
			}

			// get 4 bits for each channel for the first sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr1[chan][0] = bs_get_bits(&bbs, 4);
				chan_ptr1[chan] += (bytes_per_sample << 1);
			}

			// get 4 bits for each channel for the second sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr2[chan][0] = bs_get_bits(&bbs, 4);
				chan_ptr2[chan] += (bytes_per_sample << 1);
			}

			i -= w_vars->num_channels;
		}
		break;

	case 24:
		// setup the channel pointers
		bytes_per_sample = w_vars->num_channels * 3;
		for (i = 0; i < w_vars->num_channels; i++)
		{
			chan_ptr1[i] = w_vars->wav_buffer + (i * 3);
			chan_ptr2[i] = chan_ptr1[i] + bytes_per_sample;
		}

		// setup bitstream
		bs_init(&bbs, pcm_buffer, bytes_to_process);

		i = bytes_to_process;
		while (i > 0)
		{
			// get 16 bits for each channel for the first sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr1[chan][2] = bs_get_bits(&bbs, 8);
				chan_ptr1[chan][1] = bs_get_bits(&bbs, 8);
				i -= 2;
			}

			// get 16 bits for each channel for the second sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr2[chan][2] = bs_get_bits(&bbs, 8);
				chan_ptr2[chan][1] = bs_get_bits(&bbs, 8);
				i -= 2;
			}

			// get 8 bits for each channel for the first sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr1[chan][0] = bs_get_bits(&bbs, 8);
				i--;
				chan_ptr1[chan] += (bytes_per_sample << 1);
			}

			// get 8 bits for each channel for the second sample
			for (chan = 0; chan < w_vars->num_channels; chan++)
			{
				chan_ptr2[chan][0] = bs_get_bits(&bbs, 8);
				i--;
				chan_ptr2[chan] += (bytes_per_sample << 1);
			}
		}
		break;
	}

	return bytes_to_process;
}


//---------------------------------------------------------------------------
//
// process_hdmv_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_hdmv_lpcm(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *pcm_buffer = ptr;
	uint8_t *buffer = w_vars->wav_buffer;
	uint32_t i, len = bytes_to_process;
	uint32_t bytes_written = 0;

	if (w_vars->encoder_mode)
	{
		// get rid of the 4-byte header header
		pcm_buffer += 4;
		len -= 4;
	}

	switch (w_vars->bits_per_sample)
	{
	case 16:
		// just swap the endian of the bytes
		for (i = 0; i < len; i += 2)
		{
			buffer[i] = pcm_buffer[i + 1];
			buffer[i + 1] = pcm_buffer[i];
			bytes_written += 2;
		}
		break;

	case 20:
	case 24:
		while (len > 0)
		{
			buffer[0] = pcm_buffer[2];
			buffer[1] = pcm_buffer[1];
			buffer[2] = pcm_buffer[0];
			buffer += 3;
			pcm_buffer += 3;
			bytes_written += 3;
			len -= 3;
		}
		break;
	}

	return bytes_written;
}


//---------------------------------------------------------------------------
//
// process_302m_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_302m_lpcm(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int32_t i;
	uint32_t value;
	uint32_t bytes_written = 0;
	uint8_t *buffer = &w_vars->wav_buffer[0];
	uint8_t *pcm_buffer = ptr;

	i = bytes_to_process;

	if (w_vars->encoder_mode)
	{
		// get rid of the 4-byte header header
		pcm_buffer += 4;
		i -= 4;
	}

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
    case 16:
		while (i > 0)
		{
			buffer[0] = BitReverseTable256[pcm_buffer[0]];
			buffer[1] = BitReverseTable256[pcm_buffer[1]];

			value = (pcm_buffer[2] << 16) | (pcm_buffer[3] << 8) | pcm_buffer[4];
			
			buffer[2] = BitReverseTable256[(unsigned char)((value & 0x000FF000) >> 12)];
			buffer[3] = BitReverseTable256[(unsigned char)((value & 0x00000FF0) >> 4)];

			buffer += 4;
			bytes_written += 4;

			pcm_buffer += 5;

			i -= 5;
		}
		break;
		
	case 20:
		while (i > 0)
		{
			value = (pcm_buffer[0] << 16) | (pcm_buffer[1] << 8) | pcm_buffer[2];
			
			buffer[0] = BitReverseTable256[(unsigned char)((value & 0x00F00000) >> 20)];
			buffer[1] = BitReverseTable256[(unsigned char)((value & 0x000FF000) >> 12)];
			buffer[2] = BitReverseTable256[(unsigned char)((value & 0x00000FF0) >> 4)];

			value = (pcm_buffer[3] << 16) | (pcm_buffer[4] << 8) | pcm_buffer[5];

			buffer[3] = BitReverseTable256[(unsigned char)((value & 0x00F00000) >> 20)];
			buffer[4] = BitReverseTable256[(unsigned char)((value & 0x000FF000) >> 12)];
			buffer[5] = BitReverseTable256[(unsigned char)((value & 0x00000FF0) >> 4)];

			buffer += 6;
			bytes_written += 6;

			pcm_buffer += 6;
			
			i -= 6;
		}
		break;
		
	case 24:
		while (i > 0)
		{
			buffer[0] = BitReverseTable256[pcm_buffer[0]];
			buffer[1] = BitReverseTable256[pcm_buffer[1]];
			buffer[2] = BitReverseTable256[pcm_buffer[2]];

			value = (pcm_buffer[3] << 24) | (pcm_buffer[4] << 16) | (pcm_buffer[5] << 8) | pcm_buffer[6];

			buffer[3] = BitReverseTable256[(unsigned char)((value & 0x0FF00000) >> 20)];
			buffer[4] = BitReverseTable256[(unsigned char)((value & 0x000FF000) >> 12)];
			buffer[5] = BitReverseTable256[(unsigned char)((value & 0x00000FF0) >> 4)];
			
			buffer += 6;
			bytes_written += 6;

			pcm_buffer += 7;

			i -= 7;
		}
		break;
	}
  
	return bytes_written;
}


//---------------------------------------------------------------------------
//
// process_331m_lpcm
//
//---------------------------------------------------------------------------

static uint32_t process_331m_lpcm(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int32_t i, k;
	uint32_t bytes_written = 0;
	uint32_t value1, value2;
	uint8_t *buffer = &w_vars->wav_buffer[0];
	uint8_t *pcm_buffer = ptr;

	i = bytes_to_process;
	
	while (i > 0)
	{
		for (k = 0; k < 8; k++)
		{
			value1 = (pcm_buffer[0] << 24) | (pcm_buffer[1] << 16) | (pcm_buffer[2] << 8) | pcm_buffer[3];
			pcm_buffer += 4;
			i -= 4;

			if (k < w_vars->wav_common_chunk.numChannels)
			{
				value2 = (value1 & 0x000F0F0F) << 12;  // mask off the MSB's and shift them into place
				value1 = (value1 & 0xF0F0F000) >> 4;   // mask off the LSB's and shift them into place
				value1 |= value2;
				
				buffer[0] = (uint8_t)((value1 & 0xFF000000) >> 24);
				buffer[1] = (uint8_t)((value1 & 0x00FF0000) >> 16);
				buffer[2] = (uint8_t)((value1 & 0x0000FF00) >> 8);

				buffer += 3;
				bytes_written += 3;
			}
		}
	}
	
	return bytes_written;
}


//---------------------------------------------------------------------------
//
// process_382m_lpcm_demuxer
//
//---------------------------------------------------------------------------

static uint32_t process_382m_lpcm_demuxer(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int32_t i, j, k;
	int32_t channel;
	uint8_t *buffer;
	uint8_t *pcm_buffer = ptr;
	
	channel = w_vars->aes3_channel;

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 16:
		i = bytes_to_process;
		buffer = &w_vars->wav_buffer[channel * 2];
		k = w_vars->wav_common_chunk.numChannels * 2;
		j = 0;
			
		while (i > 0)
		{
			buffer[j + 0] = pcm_buffer[0];
			buffer[j + 1] = pcm_buffer[1];
			j += k;
			pcm_buffer += 2;
			i -= 2;
		}
		break;
			
	case 24:
		i = bytes_to_process;
		buffer = &w_vars->wav_buffer[channel * 3];
		k = w_vars->wav_common_chunk.numChannels * 3;
		j = 0;
			
		while (i > 0)
		{
			buffer[j + 0] = pcm_buffer[0];
			buffer[j + 1] = pcm_buffer[1];
			buffer[j + 2] = pcm_buffer[2];
			j += k;
			pcm_buffer += 3;
			i -= 3;
		}
		break;
	}

	if (channel == w_vars->wav_common_chunk.numChannels - 1)
	{
		uint32_t bytes_written = 0;
		
		switch (w_vars->wav_common_chunk.bitsPerSample)
		{
		case 16:
			i = bytes_to_process / 2;
			bytes_written = w_vars->wav_common_chunk.numChannels * 2 * i;
			break;
		case 24:
			i = bytes_to_process / 3;
			bytes_written = w_vars->wav_common_chunk.numChannels * 3 * i;
			break;
		}

		return bytes_written;
	}

	return 0;
}


//---------------------------------------------------------------------------
//
// process_382m_lpcm_encoder
//
//---------------------------------------------------------------------------

static uint32_t process_382m_lpcm_encoder(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	int32_t i, j, k;
	uint32_t bytes_written = 0;
	int32_t channel = 0;
	uint8_t *buffer;
	uint8_t *pcm_buffer = ptr;

	while (channel < w_vars->wav_common_chunk.numChannels)
	{
		switch (w_vars->wav_common_chunk.bitsPerSample)
		{
		case 16:
			i = bytes_to_process / w_vars->wav_common_chunk.numChannels;
			buffer = &w_vars->wav_buffer[channel * 2];
			k = w_vars->wav_common_chunk.numChannels * 2;
			j = 0;
			
			while (i > 0)
			{
				buffer[j + 0] = pcm_buffer[0];
				buffer[j + 1] = pcm_buffer[1];
				j += k;
				pcm_buffer += 2;
				bytes_written += 2;
				i -= 2;
			}
			break;
			
		case 24:
			i = bytes_to_process / w_vars->wav_common_chunk.numChannels;
			buffer = &w_vars->wav_buffer[channel * 3];
			k = w_vars->wav_common_chunk.numChannels * 3;
			j = 0;
			
			while (i > 0)
			{
				buffer[j + 0] = pcm_buffer[0];
				buffer[j + 1] = pcm_buffer[1];
				buffer[j + 2] = pcm_buffer[2];
				j += k;
				pcm_buffer += 3;
				bytes_written += 3;
				i -= 3;
			}
			break;
		}
		channel++;
	}

	return bytes_written;
}


//---------------------------------------------------------------------------
//
// process_qt_pcm_be
//
//---------------------------------------------------------------------------

static uint32_t process_qt_pcm_be(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *buffer = &w_vars->wav_buffer[0];
	uint8_t *pcm_buffer = ptr;
	uint32_t i;

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 8:
		// unsigned
		memcpy(buffer, pcm_buffer, bytes_to_process);
		break;

	case 16:
		i = bytes_to_process;
		while (i > 0)
		{
			buffer[0] = pcm_buffer[1];
			buffer[1] = pcm_buffer[0];

			i -= 2;
			buffer += 2;
			pcm_buffer += 2;
		}
		break;
			
	case 20:
	case 24:
		i = bytes_to_process;
		while (i > 0)
		{
			buffer[0] = pcm_buffer[2];
			buffer[1] = pcm_buffer[1];
			buffer[2] = pcm_buffer[0];

			i -= 3;
			buffer += 3;
			pcm_buffer += 3;
		}
		break;

	case 32:
		i = bytes_to_process;
		while (i > 0)
		{
			buffer[0] = pcm_buffer[3];
			buffer[1] = pcm_buffer[2];
			buffer[2] = pcm_buffer[1];
			buffer[3] = pcm_buffer[0];

			i -= 4;
			buffer += 4;
			pcm_buffer += 4;
		}
		break;
	}

	return bytes_to_process;
}


//---------------------------------------------------------------------------
//
// process_qt_pcm_le
//
//---------------------------------------------------------------------------

static uint32_t process_qt_pcm_le(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *buffer = &w_vars->wav_buffer[0];
	uint8_t *pcm_buffer = ptr;
	uint32_t i;
	int32_t svalue;

	switch (w_vars->wav_common_chunk.bitsPerSample)
	{
	case 8:
		// signed
		i = bytes_to_process;
		while (i > 0)
		{
			svalue = pcm_buffer[0];
			buffer[0] = (uint8_t)(svalue + 128);
			i--;
			buffer++;
			pcm_buffer++;
		}
		break;

	case 16:
	case 20:
	case 24:
	case 32:
		memcpy(buffer, pcm_buffer, bytes_to_process);
		break;
	}

	return bytes_to_process;
}


//---------------------------------------------------------------------------
//
// process_qt_fp_be
//
//---------------------------------------------------------------------------

static uint32_t process_qt_fp_be(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *buffer = &w_vars->wav_buffer[0];
	uint8_t *pcm_buffer = ptr;
	uint32_t i;

	switch (w_vars->bits_per_sample)
	{
	case 32:
		i = bytes_to_process;
		while (i > 0)
		{
			buffer[0] = pcm_buffer[3];
			buffer[1] = pcm_buffer[2];
			buffer[2] = pcm_buffer[1];
			buffer[3] = pcm_buffer[0];

			i -= 4;
			buffer += 4;
			pcm_buffer += 4;
		}
		break;

	case 64:
		i = bytes_to_process;
		while (i > 0)
		{
			buffer[4] = pcm_buffer[3];
			buffer[5] = pcm_buffer[2];
			buffer[6] = pcm_buffer[1];
			buffer[7] = pcm_buffer[0];

			buffer[0] = pcm_buffer[7];
			buffer[1] = pcm_buffer[6];
			buffer[2] = pcm_buffer[5];
			buffer[3] = pcm_buffer[4];

			i -= 8;
			buffer += 8;
			pcm_buffer += 8;
		}
		break;
	}

	return bytes_to_process;
}


//---------------------------------------------------------------------------
//
// process_qt_fp_le
//
//---------------------------------------------------------------------------

static uint32_t process_qt_fp_le(bufstream_tt *bs, uint8_t *ptr, uint32_t bytes_to_process)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *buffer = &w_vars->wav_buffer[0];
	uint8_t *pcm_buffer = ptr;

	memcpy(buffer, pcm_buffer, bytes_to_process);

	return bytes_to_process;
}


//---------------------------------------------------------------------------
//
// write_wave_headers
//
//---------------------------------------------------------------------------

static int32_t write_wave_headers(bufstream_tt *bs, uint8_t rewrite_flag)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t bytes_written;;

#ifdef WIN32IO
	WriteFile(p->io, &w_vars->wav_chunk, sizeof(wav_chunk_s), (DWORD*)&bytes_written, NULL);
#else
	bytes_written = (uint32_t)fwrite(&w_vars->wav_chunk, sizeof(uint8_t), sizeof(struct wav_chunk_s), p->io);
#endif
	if (bytes_written != sizeof(struct wav_chunk_s))
		return 1;

	if (!rewrite_flag)
		p->bytecount += bytes_written;

#ifdef WIN32IO
	WriteFile(p->io, &w_vars->wav_common_chunk, sizeof(wav_common_chunk_s), (DWORD*)&bytes_written, NULL);
#else
	bytes_written = (uint32_t)fwrite(&w_vars->wav_common_chunk, sizeof(uint8_t), sizeof(struct wav_common_chunk_s), p->io);
#endif
	if (bytes_written != sizeof(struct wav_common_chunk_s))
		return 1;

	if (!rewrite_flag)
		p->bytecount += bytes_written;

#ifdef WIN32IO
	WriteFile(p->io, &w_vars->wav_data_chunk, sizeof(wav_data_chunk_s), (DWORD*)&bytes_written, NULL);
#else
	bytes_written = (uint32_t)fwrite(&w_vars->wav_data_chunk, sizeof(uint8_t), sizeof(struct wav_data_chunk_s), p->io);
#endif
	if (bytes_written != sizeof(struct wav_data_chunk_s))
		return 1;

	if (!rewrite_flag)
		p->bytecount += bytes_written;

	w_vars->wave_headers_written = 1;

	return 0;
}


//---------------------------------------------------------------------------
//
// fill_wave_headers
//
//---------------------------------------------------------------------------

static int32_t fill_wave_headers(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;

	w_vars->wav_chunk.ckID[0] = 'R';
	w_vars->wav_chunk.ckID[1] = 'I';
	w_vars->wav_chunk.ckID[2] = 'F';
	w_vars->wav_chunk.ckID[3] = 'F';
	w_vars->wav_chunk.ckSize = sizeof(w_vars->wav_chunk.formType) + sizeof(struct wav_common_chunk_s) + sizeof(struct wav_data_chunk_s);
	w_vars->wav_chunk.formType[0] = 'W';
	w_vars->wav_chunk.formType[1] = 'A';
	w_vars->wav_chunk.formType[2] = 'V';
	w_vars->wav_chunk.formType[3] = 'E';

	w_vars->wav_common_chunk.ckID[0] = 'f';
	w_vars->wav_common_chunk.ckID[1] = 'm';
	w_vars->wav_common_chunk.ckID[2] = 't';
	w_vars->wav_common_chunk.ckID[3] = ' ';
	w_vars->wav_common_chunk.ckSize = 16;
	if ((w_vars->wave_info.audio_format == BS_QT_PCM_AUDIO) &&
		(w_vars->qt_lpcm_flags & BS_PCM_FLAGS_IS_FLOAT))
		w_vars->wav_common_chunk.audioFormat = WAVE_FORMAT_IEEE_FLOAT;
	else
		w_vars->wav_common_chunk.audioFormat = WAVE_FORMAT_PCM;
	w_vars->wav_common_chunk.numChannels = w_vars->num_channels;
	w_vars->wav_common_chunk.sampleRate = w_vars->sample_rate;
	w_vars->wav_common_chunk.bitsPerSample = w_vars->bits_per_sample;
	w_vars->wav_common_chunk.blockAlign = w_vars->wav_common_chunk.numChannels << 1;
	w_vars->wav_common_chunk.byteRate = w_vars->wav_common_chunk.sampleRate * w_vars->wav_common_chunk.blockAlign;
        
	w_vars->wav_data_chunk.ckID[0] = 'd';
	w_vars->wav_data_chunk.ckID[1] = 'a';
	w_vars->wav_data_chunk.ckID[2] = 't';
	w_vars->wav_data_chunk.ckID[3] = 'a';
	w_vars->wav_data_chunk.ckSize  = 0;

	return 0;
}


//---------------------------------------------------------------------------
//
// encoder_wave_info
//
//---------------------------------------------------------------------------

static int32_t encoder_wave_info(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint8_t *ptr = p->bfr;

	if ((ptr[0] == 'L') &&
		(ptr[1] == 'P') &&
		(ptr[2] == 'C') &&
		(ptr[3] == 'M'))
	{
		w_vars->wave_info.audio_format = BS_DVD_LPCM_AUDIO;
		w_vars->num_channels = ((ptr[4] & 0x1C) >> 2) + 1;

		if (ptr[4] & 0x01)
			w_vars->sample_rate = 96000;
		else
			w_vars->sample_rate = 48000;

		w_vars->bits_per_sample = 16;
	}
	else if ((ptr[0] == 'H') &&
			(ptr[1] == 'D') &&
			(ptr[2] == 'M') &&
			(ptr[3] == 'V') &&
			(ptr[4] == 13))	// means HDMV
	{
		w_vars->wave_info.audio_format = BS_HDMV_LPCM_AUDIO;
		switch (ptr[5] & 0x60)
		{
		case 0x00:
			w_vars->sample_rate = 48000;
			break;
		case 0x20:
			w_vars->sample_rate = 96000;
			break;
		case 0x40:
			w_vars->sample_rate = 192000;
			break;
		default:
			return 1;
		}

		w_vars->num_channels = (ptr[5] & 0x07) + 1;

		switch ((ptr[5] & 0x18) >> 3)
		{
		case 0:
			w_vars->bits_per_sample = 16;
			break;
		case 1:
			w_vars->bits_per_sample = 20;
			break;
		case 2:
			w_vars->bits_per_sample = 24;
			break;
		default:
			return 1;
		}
	}
	else if ((ptr[0] == 'A') &&
			(ptr[1] == 'E') &&
			(ptr[2] == 'S') &&
			(ptr[3] == '3'))
	{
		switch (ptr[4])
		{
		case 10:
			w_vars->wave_info.audio_format = BS_AES3_302M_AUDIO;
			break;
		case 11:
			w_vars->wave_info.audio_format = BS_AES3_331M_AUDIO;
			break;
		case 12:
			w_vars->wave_info.audio_format = BS_AES3_382M_AUDIO;
			break;
		default:
			return 1;
		}

		w_vars->num_channels = (ptr[5] & 0x07) + 1;

		switch ((ptr[5] & 0x18) >> 3)
		{
		case 0:
			w_vars->bits_per_sample = 16;
			break;
		case 1:
			w_vars->bits_per_sample = 20;
			break;
		case 2:
			w_vars->bits_per_sample = 24;
			break;
		default:
			return 1;
		}

		w_vars->sample_rate = 48000;
	}
	else
		return 1;

	return 0;
}


//---------------------------------------------------------------------------
//
// demuxer_wave_info
//
//---------------------------------------------------------------------------

static int32_t demuxer_wave_info(bufstream_tt *bs, struct dmux_chunk_info *chunk_info)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;

	// fill in the wave info structure
	if (chunk_info->flags & MCDMUX_FLAGS_DVD_LPCM)
	{
		// DVD LPCM
		w_vars->wave_info.audio_format = BS_DVD_LPCM_AUDIO;

		switch ((chunk_info->info[5] & 0xC0) >> 6)
		{
		case 1:
			w_vars->bits_per_sample = 20;
			break;
		case 2:
			w_vars->bits_per_sample = 24;
			break;
		default:
			w_vars->bits_per_sample = 16;
			break;
		}

		w_vars->sample_rate = (chunk_info->info[5] & 0x30) >> 4;
		if (w_vars->sample_rate)
			w_vars->sample_rate = 96000;
		else
			w_vars->sample_rate = 48000;

		w_vars->num_channels = (chunk_info->info[5] & 0x07) + 1;
	}
	else if (chunk_info->flags & MCDMUX_FLAGS_HDMV_LPCM)
	{
		// HDMV LPCM
		w_vars->wave_info.audio_format = BS_HDMV_LPCM_AUDIO;
		switch ((chunk_info->info[3] & 0xC0) >> 6)
		{
		case 2:
			w_vars->bits_per_sample = 20;
			break;
		case 3:
			w_vars->bits_per_sample = 24;
			break;
		default:
			w_vars->bits_per_sample = 16;
			break;
		}

		switch ((chunk_info->info[2] & 0xF0) >> 4)
		{
		case 4:
		case 5:
		case 6:
		case 7:
			w_vars->num_channels = 4;
			break;
		case 8:
		case 9:
			w_vars->num_channels = 6;
			break;
		case 10:
		case 11:
			w_vars->num_channels = 8;
			break;
		default:
			w_vars->num_channels = 2;
			break;
		}

		switch (chunk_info->info[2] & 0x0F)
		{
		case 4:
			w_vars->sample_rate = 96000;
			break;
		case 5:
			w_vars->sample_rate = 192000;
			break;
		default:
			w_vars->sample_rate = 48000;
			break;
		}
	}
	else if (chunk_info->flags & MCDMUX_FLAGS_AES3_302M)
	{
		// AES3-302M LPCM
		w_vars->wave_info.audio_format = BS_AES3_302M_AUDIO;

		switch ((chunk_info->info[3] & 0x30) >> 4)
		{
		case 1:
			w_vars->bits_per_sample = 20;
			break;
		case 2:
			w_vars->bits_per_sample = 24;
			break;
		default:
			w_vars->bits_per_sample = 16;
			break;
		}
		w_vars->num_channels = (((chunk_info->info[2] & 0xC0) >> 6) + 1) << 1;
		w_vars->sample_rate = 48000;
	}
	else if (chunk_info->flags & MCDMUX_FLAGS_AES3_331M)
	{
		// AES3-331M LPCM
		w_vars->wave_info.audio_format = BS_AES3_331M_AUDIO;

		w_vars->bits_per_sample = 24;
		w_vars->num_channels = 0;
		if (chunk_info->info[3] & 0x01)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x02)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x04)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x08)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x10)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x20)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x40)
			w_vars->num_channels++;
		if (chunk_info->info[3] & 0x80)
			w_vars->num_channels++;

		w_vars->sample_rate = 48000;
	}
	else if (chunk_info->flags & MCDMUX_FLAGS_AES3_382M)
	{
		// AES3-382M LPCM
		w_vars->wave_info.audio_format = BS_AES3_382M_AUDIO;

		w_vars->bits_per_sample = chunk_info->info[0];
		w_vars->num_channels = chunk_info->info[1];
		if (chunk_info->info[2])
			w_vars->sample_rate = 96000;
		else
			w_vars->sample_rate = 48000;
	}
	else if (chunk_info->flags & MCDMUX_FLAGS_TWOS_LPCM)
	{
		// TWOS LPCM
		w_vars->wave_info.audio_format = BS_TWOS_LPCM_AUDIO;

		w_vars->bits_per_sample = chunk_info->info[0];
		w_vars->num_channels = chunk_info->info[1];
		if (chunk_info->info[2])
			w_vars->sample_rate = 96000;
		else
			w_vars->sample_rate = 48000;
		if (chunk_info->info_len >= 7)
		{
			w_vars->qt_lpcm_flags = (chunk_info->info[3] << 24) |
									(chunk_info->info[4] << 16) |
									(chunk_info->info[5] << 8) |
									 chunk_info->info[6];
		}
		else
		{
			w_vars->qt_lpcm_flags = BS_PCM_FLAGS_IS_BIG_ENDIAN | BS_PCM_FLAGS_IS_SIGNED | BS_PCM_FLAGS_IS_PACKED;
		}
	}
	else if (chunk_info->flags & MCDMUX_FLAGS_QT_PCM)
	{
		// QT LPCM
		w_vars->wave_info.audio_format = BS_QT_PCM_AUDIO;

		w_vars->bits_per_sample = chunk_info->info[0];
		w_vars->num_channels = chunk_info->info[1];
		w_vars->sample_rate = (chunk_info->info[2] << 24) |
							  (chunk_info->info[3] << 16) |
							  (chunk_info->info[4] << 8) |
							   chunk_info->info[5];
		if (chunk_info->info_len >= 10)
		{
			w_vars->qt_lpcm_flags = (chunk_info->info[6] << 24) |
									(chunk_info->info[7] << 16) |
									(chunk_info->info[8] << 8) |
									 chunk_info->info[9];
		}
		else
		{
			w_vars->qt_lpcm_flags = BS_PCM_FLAGS_IS_BIG_ENDIAN | BS_PCM_FLAGS_IS_SIGNED | BS_PCM_FLAGS_IS_PACKED;
		}
	}
	else
		return 1;

	return 0;
}


//---------------------------------------------------------------------------
//
// write_frame
//
//---------------------------------------------------------------------------

static uint32_t write_frame(bufstream_tt *bs, uint8_t *bfr, uint32_t num_bytes)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;
	uint32_t i;
	uint32_t frame_size = num_bytes;
	uint32_t wave_size;
	uint8_t *ptr = bfr;

	// convert the audio
	switch (w_vars->wave_info.audio_format)
	{
	case BS_DVD_LPCM_AUDIO:
		wave_size = process_dvd_lpcm(bs, ptr, frame_size);
		break;
	case BS_HDMV_LPCM_AUDIO:
		wave_size = process_hdmv_lpcm(bs, ptr, frame_size);
		break;
	case BS_AES3_302M_AUDIO:
		// if we don't have a complete frame yet, just return
		if (frame_size < w_vars->aes3_payload_length)
			return 0;
		wave_size = process_302m_lpcm(bs, ptr, frame_size);
		break;
	case BS_AES3_331M_AUDIO:
		wave_size = process_331m_lpcm(bs, ptr, frame_size);
		break;
	case BS_AES3_382M_AUDIO:
		if (w_vars->encoder_mode)
			wave_size = process_382m_lpcm_encoder(bs, ptr, frame_size);
		else
			wave_size = process_382m_lpcm_demuxer(bs, ptr, frame_size);
		break;
	case BS_TWOS_LPCM_AUDIO:
	case BS_QT_PCM_AUDIO:
		if (w_vars->qt_lpcm_flags & BS_PCM_FLAGS_IS_FLOAT)
		{
			if (w_vars->qt_lpcm_flags & BS_PCM_FLAGS_IS_BIG_ENDIAN)
				wave_size = process_qt_fp_be(bs, ptr, frame_size);
			else
				wave_size = process_qt_fp_le(bs, ptr, frame_size);
		}
		else if (w_vars->bits_per_sample == 8)
		{
			if (w_vars->qt_lpcm_flags & BS_PCM_FLAGS_IS_SIGNED)
				wave_size = process_qt_pcm_le(bs, ptr, frame_size);
			else
				wave_size = process_qt_pcm_be(bs, ptr, frame_size);
		}
		else
		{
			if (w_vars->qt_lpcm_flags & BS_PCM_FLAGS_IS_BIG_ENDIAN)
				wave_size = process_qt_pcm_be(bs, ptr, frame_size);
			else
				wave_size = process_qt_pcm_le(bs, ptr, frame_size);
		}
		break;
	default:
		return 1;
	}

	if (wave_size > 0)
	{
#ifdef WIN32IO
		WriteFile(p->io, &w_vars->wav_buffer[0], wave_size, (DWORD*)&i, NULL);
#else
		i = (uint32_t)fwrite(&w_vars->wav_buffer[0], sizeof(uint8_t), wave_size, p->io);
#endif
	
		if (i != wave_size)
			return 1;

		w_vars->total_sample_bytes += i;
		p->bytecount += i;
	}

	p->idx = 0;

	return 0;
}


//---------------------------------------------------------------------------
//
// fw_usable_bytes
//
//---------------------------------------------------------------------------

static uint32_t fw_usable_bytes(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	return p->bfr_size - p->idx;
}


//---------------------------------------------------------------------------
//
// fw_confirm
//
//---------------------------------------------------------------------------

static uint32_t fw_confirm(bufstream_tt *bs, uint32_t numbytes)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	p->idx += numbytes;
	
	return numbytes;
}


//---------------------------------------------------------------------------
//
// fw_request
//
//---------------------------------------------------------------------------

static uint8_t *fw_request(bufstream_tt *bs, uint32_t numbytes)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	if (numbytes > p->bfr_size)
		return NULL;

	if (p->idx + numbytes > p->bfr_size)
		return NULL;

	return p->bfr + p->idx;
}


//---------------------------------------------------------------------------
//
// fw_copybytes
//
//---------------------------------------------------------------------------

static uint32_t fw_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
	struct impl_stream* p = bs->Buf_IO_struct;

	if (p->idx + numbytes > p->bfr_size)
		return 0;

	memcpy(p->bfr + p->idx, ptr, numbytes);

	p->idx += numbytes;

	return numbytes;
}


//---------------------------------------------------------------------------
//
// fw_chunksize
//
//---------------------------------------------------------------------------

static uint32_t fw_chunksize(bufstream_tt *bs)
{
	return bs->Buf_IO_struct->chunk_size;
}


//---------------------------------------------------------------------------
//
// fw_auxinfo
//
//---------------------------------------------------------------------------


static uint32_t fw_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
	uint64_t *ptr;
	struct dmux_chunk_info *chunk_info;
	struct a_au_struct *au;

	if (offs){};  // remove compile warning

	switch (info_ID)
	{
	case BYTECOUNT_INFO:
		ptr = (uint64_t*)info_ptr;
		if (ptr && (info_size == sizeof(uint64_t)))
			*ptr = bs->Buf_IO_struct->bytecount;
		break;

	case DMUX_CHUNK_INFO:
		chunk_info = (struct dmux_chunk_info*)info_ptr;
		if (chunk_info && (info_size == sizeof(struct dmux_chunk_info)))
		{
			struct impl_stream* p = bs->Buf_IO_struct;
			struct wave_variables_s* w_vars = &p->w_vars;
			
			if (chunk_info->flags & MCDMUX_FLAGS_UNIT_START)
			{
				w_vars->aes3_channel = (uint8_t)chunk_info->aes3_channel;
				w_vars->aes3_payload_length = chunk_info->aes3_payload_length;
			}

			if (!w_vars->wave_headers_written)
			{
				if (chunk_info->flags & MCDMUX_FLAGS_UNIT_START)
				{
					if (demuxer_wave_info(bs, chunk_info))
						return BS_ERROR;

					// fill in the wave headers
					if (fill_wave_headers(bs))
						return BS_ERROR;

					// write the wave headers
					if (write_wave_headers(bs, 0))
						return BS_ERROR;
				}
				else
				{
					p->idx = 0; // eat the bytes until we get a unit start
					return BS_OK;
				}
			}

			if (write_frame(bs, p->bfr, p->idx))
				return BS_ERROR;
		}
		break;

	case AUDIO_AU_CODE:
		au = (struct a_au_struct*)info_ptr;
		if (au && (info_size == sizeof(struct a_au_struct)))
		{
			struct impl_stream* p = bs->Buf_IO_struct;
			struct wave_variables_s* w_vars = &p->w_vars;
			uint32_t frame_bytes = p->idx;
			uint8_t *ptr = p->bfr;

			w_vars->aes3_payload_length = au->length;

			if (!w_vars->wave_headers_written)
			{
				w_vars->encoder_mode = 1;

				if (encoder_wave_info(bs))
					return BS_ERROR;

				ptr += 8;
				frame_bytes -= 8;

				// fill in the wave headers
				if (fill_wave_headers(bs))
					return BS_ERROR;

				// write the wave headers
				if (write_wave_headers(bs, 0))
					return BS_ERROR;
			}
	
			if (frame_bytes > 0)
			{
				if (write_frame(bs, ptr, frame_bytes))
					return BS_ERROR;
			}
			else
				p->idx = 0;
		}
		break;
	}

	return BS_OK;
}


//---------------------------------------------------------------------------
//
// fw_split
//
//---------------------------------------------------------------------------

static uint32_t fw_split(bufstream_tt *bs)
{
	if (!bs){};  // remove compile warning
	return BS_ERROR;
}


//---------------------------------------------------------------------------
//
// fw_done
//
//---------------------------------------------------------------------------

static void fw_done(bufstream_tt *bs, int32_t abort)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct wave_variables_s* w_vars = &p->w_vars;

	if (abort){};  // remove compile warning

#ifdef WIN32IO
	DWORD n;
	WriteFile(p->io, p->bfr, p->idx, &n, NULL);
#else
	fwrite(p->bfr, sizeof(uint8_t), p->idx, p->io);
#endif

	w_vars->total_sample_bytes += p->idx;

	w_vars->wav_chunk.ckSize += w_vars->total_sample_bytes;
	w_vars->wav_data_chunk.ckSize = w_vars->total_sample_bytes;

#ifdef WIN32IO
	if (SetFilePointerEx(p->io, 0, 0, FILE_BEGIN))
#else
	if (!fseek(p->io, 0, SEEK_SET))
#endif
	{
		write_wave_headers(bs, 1);
	}

#ifdef WIN32IO
	CloseHandle(p->io);
#else
	fclose(p->io);
#endif

	free(p);
	bs->Buf_IO_struct = NULL;
}


//---------------------------------------------------------------------------
//
// fw_free
//
//---------------------------------------------------------------------------

static void fw_free(bufstream_tt *bs)
{
	if (bs->Buf_IO_struct)
		bs->done(bs, 0);

	free(bs);
}


//---------------------------------------------------------------------------
//
// init_file_buf_write
//
//---------------------------------------------------------------------------

#ifdef _BS_UNICODE
static int32_t init_file_buf_write(bufstream_tt *bs,
                            const wchar_t* bs_filename)
#else
static int32_t init_file_buf_write(bufstream_tt *bs,
                            const char *bs_filename)
#endif
{
	bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
	if (!bs->Buf_IO_struct)
		return BS_ERROR;

#ifdef WIN32IO
	bs->Buf_IO_struct->io = 
#ifdef _BS_UNICODE
		CreateFileW(bs_filename, 
#else
		CreateFile(bs_filename, 
#endif
					GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if (bs->Buf_IO_struct->io == INVALID_HANDLE_VALUE)
#else
#ifdef _BS_UNICODE
	bs->Buf_IO_struct->io = _wfopen(bs_filename, L"wb");
#else

#if defined(__QNX__)
	bs->Buf_IO_struct->io = fopen64(bs_filename, "wb");
#else
	bs->Buf_IO_struct->io = fopen(bs_filename, "wb");
#endif
#endif
	if (!bs->Buf_IO_struct->io)
#endif
	{
		free(bs->Buf_IO_struct);
		return BS_ERROR;
	}

	bs->Buf_IO_struct->bfr_size		= PCM_BUFFER_SIZE;
	bs->Buf_IO_struct->chunk_size	= PCM_BUFFER_SIZE / 2;
	bs->Buf_IO_struct->idx			= 0;
	bs->Buf_IO_struct->bytecount	= 0;

	memset(&bs->Buf_IO_struct->w_vars, 0, sizeof(struct wave_variables_s));

	bs->usable_bytes	= fw_usable_bytes;
	bs->request			= fw_request;
	bs->confirm			= fw_confirm;
	bs->copybytes		= fw_copybytes;
	bs->split			= fw_split;
	bs->chunksize		= fw_chunksize;
	bs->free			= fw_free;
	bs->auxinfo			= fw_auxinfo;
	bs->done			= fw_done;
	bs->drive_ptr		= NULL;
	bs->drive			= NULL;
	bs->state			= NULL;
	bs->flags			= BS_FLAGS_DST;

	return BS_OK;
}


//---------------------------------------------------------------------------
//
// open_wave_buf_write
//
//---------------------------------------------------------------------------

#ifdef _BS_UNICODE
bufstream_tt *open_wave_buf_write(const wchar_t* bs_filename)
#else
bufstream_tt *open_wave_buf_write(const char *bs_filename)
#endif
{
	bufstream_tt *p;

	if (!bs_filename)
		return NULL;

	p = (bufstream_tt*)malloc(sizeof(bufstream_tt));
	if (p)
	{
		if (BS_OK != init_file_buf_write(p, bs_filename))
		{
			free(p);
			p = NULL;
		}
	}

	return p;
}


//---------------------------------------------------------------------------
//
// close_wave_buf_write
//
//---------------------------------------------------------------------------

void close_wave_buf_write(bufstream_tt* bs, int32_t abort)
{
	bs->done(bs, abort);
	bs->free(bs);
}

