/* ----------------------------------------------------------------------------
 * File: buf_subtitle_write.c
 *
 * Desc: Buffered Subtitle file I/O implementation
 *       use to read/write text based subtitle files
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
#include "buf_subtitle.h"
#include "auxinfo.h"


#define SUBTITLE_BUFFER_SIZE	1024 * 1024
#define BS_DEFAULT_DURATION		2700000			// 100ms?

struct subtitle_au_s
{
	uint8_t *pPtr;
	uint32_t len;
	uint64_t pts;
	uint64_t duration;
};


struct subtitle_variables_s
{
	struct buf_subtitle_info_s subtitle_info;

	mc_stream_format_t format;
	mc_subtitle_format_t subtitle_format;

	uint8_t subtitle_buffer[SUBTITLE_BUFFER_SIZE];

	uint8_t headers_written;

	uint8_t au_queued;
	struct subtitle_au_s au;
	uint32_t au_count;

	// for VSUB
	FILE *vsub_fp;
	uint64_t filepos;

	uint32_t total_sample_bytes;
};


//implementation structure
struct impl_stream
{
	FILE *io;
	uint8_t bfr[SUBTITLE_BUFFER_SIZE];
	uint32_t idx;       // read-write index
	uint32_t bfr_size;  // allocated size
	uint32_t chunk_size;
	uint64_t bytecount;
	uint64_t file_size;

	struct subtitle_variables_s s_vars;
};

// at the end
static void get_iso_639_1_from_639_2(uint16_t *pSrc, uint8_t *pDst);


//---------------------------------------------------------------------------
//
// process_dxsb
//
//---------------------------------------------------------------------------

static uint32_t process_dxsb(struct subtitle_variables_s* s_vars, struct subtitle_au_s *pAU)
{
	// just copy it for now
	memcpy(s_vars->subtitle_buffer, pAU->pPtr, pAU->len);
	return pAU->len;
}


//---------------------------------------------------------------------------
//
// process_utf8 (srt)
//
//---------------------------------------------------------------------------

static void format_time_utf8(int64_t time, char *sz)
{
	int64_t ptime = time < 0 ? -time : time;
	int64_t time_ms = ptime / 27000;

	int32_t	Hour = (int32_t)(time_ms / (1000 * 60 * 60));
	int32_t	Sec = (int32_t)((time_ms / 1000) % 60);
	int32_t	Min = (int32_t)((time_ms / (1000 * 60)) % 60);

	int32_t Ms = (int32_t)(((ptime % 27000000) / 27000.0) + 0.5);

	if (time < 0)
		sprintf(sz, "-%02d:%02d:%02d,%03d", Hour, Min, Sec, Ms);
	else
		sprintf(sz, "%02d:%02d:%02d,%03d", Hour, Min, Sec, Ms);
}


static uint32_t process_utf8(struct subtitle_variables_s* s_vars, struct subtitle_au_s *pAU)
{
	char str[132], str1[25], str2[25];
	uint32_t idx = 0;

	sprintf(str, "%u\n", s_vars->au_count + 1);
	memcpy(s_vars->subtitle_buffer, str, strlen(str));
	idx += (uint32_t)strlen(str);

	format_time_utf8(pAU->pts, str1);
	format_time_utf8(pAU->pts + pAU->duration, str2);
	sprintf(str, "%s --> %s\n", str1, str2);
	memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
	idx += (uint32_t)strlen(str);

	memcpy(&s_vars->subtitle_buffer[idx], pAU->pPtr, pAU->len);
	idx += pAU->len;

	s_vars->subtitle_buffer[idx] = 0;
	idx++;

	strcpy(str, "\n");
	memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
	idx += (uint32_t)strlen(str);

	// and again
	memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
	idx += (uint32_t)strlen(str);

	return idx;
}


//---------------------------------------------------------------------------
//
// process_ssa
//
//---------------------------------------------------------------------------

static void format_time_ssa(int64_t time, char *sz)
{
	int64_t ptime = time < 0 ? -time : time;
	int64_t time_ms = ptime / 27000;

	int32_t	Hour = (int32_t)(time_ms / (1000 * 60 * 60));
	int32_t	Sec = (int32_t)((time_ms / 1000) % 60);
	int32_t	Min = (int32_t)((time_ms / (1000 * 60)) % 60);

	int32_t Cs = (int32_t)(((ptime % 27000000) / 270000.0) + 0.5);

	if (time < 0)
		sprintf(sz, "-%01d:%02d:%02d.%02d", Hour, Min, Sec, Cs);
	else
		sprintf(sz, "%01d:%02d:%02d.%02d", Hour, Min, Sec, Cs);
}


static uint32_t process_ssa(struct subtitle_variables_s* s_vars, struct subtitle_au_s *pAU)
{
	char str[132], str1[25], str2[25];
	uint32_t i, idx = 0;
	uint8_t *pPtr = pAU->pPtr;
	int32_t len = pAU->len;

	format_time_ssa(pAU->pts, str1);
	format_time_ssa(pAU->pts + pAU->duration, str2);

	sprintf(str, "Dialogue: Marked=0,%s,%s", str1, str2);
	memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
	idx += (uint32_t)strlen(str);

	// skip the leading number and first comma
	i = 0;
	while ((i < 2) && (len > 0))
	{
		if (pPtr[0] == ',')
			i++;

		if (i < 2)
		{
			len--;
			pPtr++;
		}
	}

	if (len == 0)
	{
		// not found?
		memcpy(&s_vars->subtitle_buffer[idx], pAU->pPtr, pAU->len);
		idx += pAU->len;
	}
	else
	{
		memcpy(&s_vars->subtitle_buffer[idx], pPtr, len);
		idx += len;
	}

	strcpy(str, "\n");
	memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
	idx += (uint32_t)strlen(str);

	return idx;
}


//---------------------------------------------------------------------------
//
// process_usf
//
//---------------------------------------------------------------------------

static uint32_t process_usf(struct subtitle_variables_s* s_vars, struct subtitle_au_s *pAU)
{
	// just copy it for now
	memcpy(s_vars->subtitle_buffer, pAU->pPtr, pAU->len);
	return pAU->len;
}


//---------------------------------------------------------------------------
//
// process_vsub
//
//---------------------------------------------------------------------------

static void format_time_vsub(int64_t time, char *sz)
{
	int64_t ptime = time < 0 ? -time : time;
	int64_t time_ms = ptime / 27000;

	int32_t	Hour = (int32_t)(time_ms / (1000 * 60 * 60));
	int32_t	Sec = (int32_t)((time_ms / 1000) % 60);
	int32_t	Min = (int32_t)((time_ms / (1000 * 60)) % 60);

	int32_t Ms = (int32_t)(((ptime % 27000000) / 27000.0) + 0.5);

	if (time < 0)
		sprintf(sz, "-%02d:%02d:%02d:%03d", Hour, Min, Sec, Ms);
	else
		sprintf(sz, "%02d:%02d:%02d:%03d", Hour, Min, Sec, Ms);
}


static uint32_t process_vsub(struct subtitle_variables_s* s_vars, struct subtitle_au_s *pAU)
{
	char str[132], str1[25];
	uint32_t i, idx = 0;
	uint8_t *pPtr = pAU->pPtr;
	int32_t len = pAU->len;

	// write entry to index file
	format_time_vsub(pAU->pts, str1);

	sprintf(str, "timestamp: %s, filepos: %01X%08X\n", str1, (uint32_t)(s_vars->filepos >> 32), (uint32_t)(s_vars->filepos & 0xFFFFFFFF));
	memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
	idx += (uint32_t)strlen(str);

	// copy data to sub file
	i = (uint32_t)fwrite(pPtr, sizeof(uint8_t), len, s_vars->vsub_fp);
	if (i != len)
		return 0;

	s_vars->filepos += len;

	return idx;
}


//---------------------------------------------------------------------------
//
// write_subtitle
//
//---------------------------------------------------------------------------

static uint32_t write_subtitle(bufstream_tt *bs, struct subtitle_au_s *pAU)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct subtitle_variables_s* s_vars = &p->s_vars;
	uint32_t i;
	uint32_t subtitle_size = 0;

	// convert the subtitle
	switch (s_vars->subtitle_info.pFormat->stream_mediatype)
	{
	case mctDXSB_Subtitles:
		subtitle_size = process_dxsb(s_vars, pAU);
		break;
	case mctUTF8_Subtitles:
		subtitle_size = process_utf8(s_vars, pAU);
		break;
	case mctSSA_Subtitles:
	case mctASS_Subtitles:
		subtitle_size = process_ssa(s_vars, pAU);
		break;
	case mctUSF_Subtitles:
		subtitle_size = process_usf(s_vars, pAU);
		break;
	case mctVSUB_Subtitles:
		subtitle_size = process_vsub(s_vars, pAU);
		break;
	default:
		return 1;
	}

	if (subtitle_size > 0)
	{
		i = (uint32_t)fwrite(&s_vars->subtitle_buffer[0], sizeof(uint8_t), subtitle_size, p->io);
		if (i != subtitle_size)
			return 1;

		s_vars->total_sample_bytes += i;
		p->bytecount += i;
		s_vars->au_count++;
	}
	else
		return 1;

	p->idx = 0;
	pAU->len = 0;

	return 0;
}


//---------------------------------------------------------------------------
//
// write_headers
//
//---------------------------------------------------------------------------

static char *vsub_header = "# VobSub index file, v7 (do not modify this line!)\n\n# Settings\n\n";
static char *vsub_footer = "# Language index in use\nlangidx: 0\n\nid: ";

static char *ssa_header = "[Script Info]\n; This is a Sub Station Alpha v4 script.\nScriptType: v4.00\n\n[Events]\n";
static char *ass_header = "[Script Info]\n; This is an Advanced Sub Station Alpha v4+ script.\nScriptType: v4.00+\n\n[Events]\n";

static uint32_t write_headers(bufstream_tt *bs)
{
	struct impl_stream* p = bs->Buf_IO_struct;
	struct subtitle_variables_s* s_vars = &p->s_vars;
	char str[132];
	int32_t idx;
	size_t bytes_written;

	// write the header
	switch (s_vars->subtitle_info.pFormat->stream_mediatype)
	{
	case mctDXSB_Subtitles:
		break;
	case mctUTF8_Subtitles:
		// AKA SRT, no header
		break;
	case mctSSA_Subtitles:
	case mctASS_Subtitles:
		idx = 0;
		if ((s_vars->format.cbExtraBytes > 0) && (s_vars->format.pbExtraData != NULL))
		{
			memcpy(&s_vars->subtitle_buffer[idx], s_vars->format.pbExtraData, s_vars->format.cbExtraBytes);
			idx += s_vars->format.cbExtraBytes;
			strcpy(str, "\n");
			memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
			idx += (int32_t)strlen(str);

			s_vars->subtitle_buffer[idx] = 0;
			if (!strstr((char*)&s_vars->subtitle_buffer[0], "[Events]"))
			{
				strcpy(str, "[Events]\n");
				memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
				idx += (int32_t)strlen(str);
			}
		}
		else
		{
			if (s_vars->subtitle_info.pFormat->stream_mediatype == mctSSA_Subtitles)
			{
				memcpy(&s_vars->subtitle_buffer[idx], ssa_header, strlen(ssa_header));
				idx += (int32_t)strlen(ssa_header);
			}
			else
			{
				memcpy(&s_vars->subtitle_buffer[idx], ass_header, strlen(ass_header));
				idx += (int32_t)strlen(ass_header);
			}
		}

		bytes_written = fwrite(s_vars->subtitle_buffer, sizeof(uint8_t), idx, p->io);
		if (bytes_written != idx)
			return 1;

		s_vars->total_sample_bytes += (uint32_t)bytes_written;
		p->bytecount += bytes_written;
		break;
	case mctUSF_Subtitles:
		break;
	case mctVSUB_Subtitles:
		idx = 0;
		if ((s_vars->format.cbExtraBytes > 0) && (s_vars->format.pbExtraData != NULL))
		{
			memcpy(&s_vars->subtitle_buffer[idx], vsub_header, strlen(vsub_header));
			idx += (int32_t)strlen(vsub_header);

			memcpy(&s_vars->subtitle_buffer[idx], s_vars->format.pbExtraData, s_vars->format.cbExtraBytes);
			idx += s_vars->format.cbExtraBytes;
			strcpy(str, "\n\n");
			memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
			idx += (int32_t)strlen(str);
		}

		memcpy(&s_vars->subtitle_buffer[idx], vsub_footer, strlen(vsub_footer));
		idx += (int32_t)strlen(vsub_footer);

		get_iso_639_1_from_639_2(&s_vars->subtitle_format.language[0], (uint8_t*)&str[0]);
		str[2] = 0;

		strcat(str, ", index: 0\n");
		memcpy(&s_vars->subtitle_buffer[idx], str, strlen(str));
		idx += (int32_t)strlen(str);

		bytes_written = fwrite(s_vars->subtitle_buffer, sizeof(uint8_t), idx, p->io);
		if (bytes_written != idx)
			return 1;

		s_vars->total_sample_bytes += (uint32_t)bytes_written;
		p->bytecount += bytes_written;
		break;
	default:
		return 1;
	}

	s_vars->headers_written = 1;

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
	struct impl_stream* p = bs->Buf_IO_struct;
	struct subtitle_variables_s* s_vars = &p->s_vars;
	uint64_t *ptr;

    if (offs){};  // remove compile warning

	switch (info_ID)
	{
	case BYTECOUNT_INFO:
		ptr = (uint64_t*)info_ptr;
		if (ptr && (info_size == sizeof(uint64_t)))
			*ptr = bs->Buf_IO_struct->bytecount;
		break;

	case SAMPLE_INFO:
		if (info_ptr && (info_size == sizeof(struct sample_struct)))
		{
			struct sample_struct *sample_info = (struct sample_struct*)info_ptr;

			if (s_vars->au_queued)
			{
				if (s_vars->au.len != p->idx)
					return BS_ERROR;

				if (sample_info->bTimestampValid && (sample_info->rtStop > sample_info->rtStart))
					s_vars->au.duration = sample_info->rtStop - sample_info->rtStart;

				s_vars->au.pPtr = p->bfr;
				if (write_subtitle(bs, &s_vars->au))
					return BS_ERROR;

				s_vars->au_queued = 0;
			}
		}
		break;

	case DMUX_CHUNK_INFO:
		if (info_ptr && (info_size == sizeof(struct dmux_chunk_info)))
		{
			struct dmux_chunk_info *chunk_info = (struct dmux_chunk_info*)info_ptr;
			
			if (!s_vars->headers_written)
			{
				if (chunk_info->flags & MCDMUX_FLAGS_UNIT_START)
				{
					// write the headers
					if (write_headers(bs))
						return BS_ERROR;
				}
				else
				{
					p->idx = 0; // eat the bytes until we get a unit start
					return BS_OK;
				}
			}

			if (s_vars->au_queued)
			{
				if (s_vars->au.len != p->idx)
					return BS_ERROR;

				s_vars->au.pPtr = p->bfr;
				if (write_subtitle(bs, &s_vars->au))
					return BS_ERROR;

				s_vars->au_queued = 0;
			}

			if (chunk_info->flags & MCDMUX_FLAGS_UNIT_START)
			{
				if (s_vars->au.len > 0)
				{
					// we did not get an end unit?
					if (s_vars->au.len != p->idx)
						return BS_ERROR;

					s_vars->au.pPtr = p->bfr;
					if (write_subtitle(bs, &s_vars->au))
						return BS_ERROR;
				}

				s_vars->au.pts = chunk_info->PTS;
				s_vars->au.len = chunk_info->length;
				s_vars->au.duration = BS_DEFAULT_DURATION;
			}
			else
			{
				s_vars->au.len += chunk_info->length;
			}

			if (chunk_info->flags & MCDMUX_FLAGS_UNIT_END)
				s_vars->au_queued = 1;
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
	struct subtitle_variables_s* s_vars = &p->s_vars;

    if (abort){};  // remove compile warning

	if (p->idx > 0)
	{
		fwrite(p->bfr, sizeof(uint8_t), p->idx, p->io);
		s_vars->total_sample_bytes += p->idx;
	}
	fclose(p->io);

	if (s_vars->format.pbExtraData)
		free(s_vars->format.pbExtraData);

	if (s_vars->vsub_fp)
		fclose(s_vars->vsub_fp);

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
                            const wchar_t* bs_filename,
                            const wchar_t* vsub_filename,
							struct buf_subtitle_info_s *subtitle_info)
#else
static int32_t init_file_buf_write(bufstream_tt *bs,
                            const char *bs_filename,
							const char *vsub_filename,
							struct buf_subtitle_info_s *subtitle_info)
#endif
{
	struct subtitle_variables_s* s_vars;
	struct impl_stream* p_impl;

	p_impl = (struct impl_stream*)malloc(sizeof(struct impl_stream));
	if (!p_impl)
		return BS_ERROR;
	memset(p_impl, 0, sizeof(struct impl_stream));

	s_vars = &p_impl->s_vars;
	memcpy(&s_vars->subtitle_info, subtitle_info, sizeof(struct buf_subtitle_info_s));
	memcpy(&s_vars->format, subtitle_info->pFormat, sizeof(mc_stream_format_t));

	if (subtitle_info->pFormat->pFormat)
	{
		// copy it local
		memcpy(&s_vars->subtitle_format, subtitle_info->pFormat->pFormat, sizeof(mc_subtitle_format_t));
		s_vars->format.pFormat = &s_vars->subtitle_format;
	}

	if ((subtitle_info->pFormat->cbExtraBytes > 0) && (subtitle_info->pFormat->pbExtraData != NULL))
	{
		s_vars->format.pbExtraData = (uint8_t*)malloc(subtitle_info->pFormat->cbExtraBytes);
		if (!s_vars->format.pbExtraData)
		{
			free(p_impl);
			return BS_ERROR;
		}
		memcpy(s_vars->format.pbExtraData, subtitle_info->pFormat->pbExtraData, subtitle_info->pFormat->cbExtraBytes);
	}

	if (subtitle_info->pFormat->stream_mediatype == mctVSUB_Subtitles)
	{
#ifdef _BS_UNICODE
		s_vars->vsub_fp = _wfopen(vsub_filename, L"wb");
#else
#if !defined(__QNX__)
		s_vars->vsub_fp = fopen(vsub_filename, "wb");
#else
		s_vars->vsub_fp = fopen64(vsub_filename, "wb");
#endif
#endif
		if (!s_vars->vsub_fp)
		{
			if (s_vars->format.pbExtraData)
				free(s_vars->format.pbExtraData);
			free(p_impl);
			return BS_ERROR;
		}
	}

#ifdef _BS_UNICODE
	p_impl->io = _wfopen(bs_filename, L"wb");
#else
#if !defined(__QNX__)
	p_impl->io = fopen(bs_filename, "wb");
#else
	p_impl->io = fopen64(bs_filename, "wb");
#endif
#endif
	if (!p_impl->io)
	{
		if (s_vars->format.pbExtraData)
			free(s_vars->format.pbExtraData);
		if (s_vars->vsub_fp)
			fclose(s_vars->vsub_fp);
		free(p_impl);
		return BS_ERROR;
	}

	p_impl->bfr_size	= SUBTITLE_BUFFER_SIZE;
	p_impl->chunk_size	= SUBTITLE_BUFFER_SIZE / 2;

	bs->Buf_IO_struct   = p_impl;
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
// open_subtitle_buf_write
//
//---------------------------------------------------------------------------

#ifdef _BS_UNICODE
bufstream_tt *open_subtitle_buf_write(const wchar_t* bs_filename,
									  const wchar_t* vsub_filename,
									  struct buf_subtitle_info_s *subtitle_info)
#else
bufstream_tt *open_subtitle_buf_write(const char *bs_filename,
									  const char *vsub_filename,
									  struct buf_subtitle_info_s *subtitle_info)
#endif
{
	bufstream_tt *p;

	if (!bs_filename)
		return NULL;

	if (!subtitle_info || !subtitle_info->pFormat)
		return NULL;

	switch (subtitle_info->pFormat->stream_mediatype)
	{
//	case mctDXSB_Subtitles:
	case mctUTF8_Subtitles:
	case mctSSA_Subtitles:
	case mctASS_Subtitles:
//	case mctUSF_Subtitles:
		break;
	case mctVSUB_Subtitles:
		if (!vsub_filename)
			return NULL;
		break;
	default:
		return NULL;
	}

	p = (bufstream_tt*)malloc(sizeof(bufstream_tt));
	if (p)
	{
		if (BS_OK != init_file_buf_write(p, bs_filename, vsub_filename, subtitle_info))
		{
			free(p);
			p = NULL;
		}
	}

	return p;
}


//---------------------------------------------------------------------------
//
// close_subtitle_buf_write
//
//---------------------------------------------------------------------------

void close_subtitle_buf_write(bufstream_tt* bs, int32_t abort)
{
	bs->done(bs, abort);
	bs->free(bs);
}


// brute force
static void get_iso_639_1_from_639_2(uint16_t *pSrc, uint8_t *pDst)
{
	uint8_t ch1 = (uint8_t)(pSrc[0] & 0xFF);
	uint8_t ch2 = (uint8_t)(pSrc[1] & 0xFF);
	uint8_t ch3 = (uint8_t)(pSrc[2] & 0xFF);

	switch (ch1){
	case 'a': case 'A':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){ case 'r': case 'R': pDst[0] = 'a'; pDst[1] = 'a'; return;  } break; // aar = aa
		case 'b': case 'B':
			switch (ch3){ case 'k': case 'K': pDst[0] = 'a'; pDst[1] = 'b'; return;	 } break; // abk = ab
		case 'f': case 'F':
			switch (ch3){ case 'r': case 'R': pDst[0] = 'a'; pDst[1] = 'f'; return;	 } break; // afr = af
		case 'k': case 'K':
			switch (ch3){ case 'a': case 'A': pDst[0] = 'a'; pDst[1] = 'k'; return;	 } break; // aka = ak
		case 'l': case 'L':
			switch (ch3){ case 'b': case 'B': pDst[0] = 's'; pDst[1] = 'q'; return;	 } break; // alb = sq
		case 'm': case 'M':
			switch (ch3){ case 'h': case 'H': pDst[0] = 'a'; pDst[1] = 'm'; return;	 } break; // amh = am
		case 'r': case 'R':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'a'; pDst[1] = 'r'; return; // ara = ar
				case 'g': case 'G': pDst[0] = 'a'; pDst[1] = 'n'; return; // arg = an
				case 'm': case 'M': pDst[0] = 'h'; pDst[1] = 'y'; return; // arm = hy
			} break; 
		case 's': case 'S':
			switch (ch3){ case 'm': case 'M': pDst[0] = 'a'; pDst[1] = 's'; return; } break; // asm = as
		case 'v': case 'V':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'a'; pDst[1] = 'v'; return; // ava = av
				case 'e': case 'E': pDst[0] = 'a'; pDst[1] = 'e'; return; // ave = ae
			} break;
		case 'y': case 'Y':
			switch (ch3){ case 'm': case 'M': pDst[0] = 'a'; pDst[1] = 'y'; return; } break; // aym = ay
		case 'z': case 'Z':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'a'; pDst[1] = 'z'; return; } break; // aze = az
		} break;
	case 'b': case 'B':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'k': case 'K': pDst[0] = 'b'; pDst[1] = 'a'; return; // bak = ba
				case 'm': case 'M': pDst[0] = 'b'; pDst[1] = 'm'; return; // bam = bm
				case 'q': case 'Q': pDst[0] = 'e'; pDst[1] = 'u'; return; // baq = eu
			} break; 
		case 'e': case 'E':
			switch (ch3){
				case 'l': case 'L': pDst[0] = 'b'; pDst[1] = 'e'; return; // bel = be
				case 'n': case 'N': pDst[0] = 'b'; pDst[1] = 'n'; return; // ben = bn
			} break; 
		case 'i': case 'I':
			switch (ch3){
				case 'h': case 'H': pDst[0] = 'b'; pDst[1] = 'h'; return; // bih = bh
				case 's': case 'S': pDst[0] = 'b'; pDst[1] = 'i'; return; // bis = bi
			} break; 
		case 'o': case 'O':
			switch (ch3){
				case 'd': case 'D': pDst[0] = 'b'; pDst[1] = 'o'; return; // bod = bo
				case 's': case 'S': pDst[0] = 'b'; pDst[1] = 's'; return; // bos = bs
			} break; 
		case 'r': case 'R':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'b'; pDst[1] = 'r'; return; } break; // bre = br
		case 'u': case 'U':
			switch (ch3){
				case 'l': case 'L': pDst[0] = 'b'; pDst[1] = 'g'; return; // bul = bg
				case 'r': case 'R': pDst[0] = 'm'; pDst[1] = 'y'; return; // bur = my
			} break; 
		} break;
	case 'c': case 'C':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){ case 't': case 'T': pDst[0] = 'c'; pDst[1] = 'a'; return; } break; // cat = ca
		case 'e': case 'E':
			switch (ch3){ case 's': case 'S': pDst[0] = 'c'; pDst[1] = 's'; return; } break; // ces = cs
		case 'h': case 'H':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'c'; pDst[1] = 'h'; return; // cha = ch
				case 'e': case 'E': pDst[0] = 'c'; pDst[1] = 'e'; return; // che = ce
				case 'i': case 'I': pDst[0] = 'z'; pDst[1] = 'h'; return; // chi = zh
				case 'u': case 'U': pDst[0] = 'c'; pDst[1] = 'u'; return; // chu = cu
				case 'v': case 'V': pDst[0] = 'c'; pDst[1] = 'v'; return; // chv = cv
			} break; 
		case 'o': case 'O':
			switch (ch3){
				case 'r': case 'R': pDst[0] = 'k'; pDst[1] = 'w'; return; // cor = kw
				case 's': case 'S': pDst[0] = 'c'; pDst[1] = 'o'; return; // cos = co
			} break; 
		case 'r': case 'R':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'c'; pDst[1] = 'r'; return; } break; // cre = cr
		case 'y': case 'Y':
			switch (ch3){ case 'm': case 'M': pDst[0] = 'c'; pDst[1] = 'y'; return; } break; // cym = cy
		case 'z': case 'Z':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'c'; pDst[1] = 's'; return; } break; // cze = cs
		} break;
	case 'd': case 'D':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'd'; pDst[1] = 'a'; return; } break; // dan = da
		case 'e': case 'E':
			switch (ch3){ case 'u': case 'U': pDst[0] = 'd'; pDst[1] = 'e'; return; } break; // deu = de
		case 'i': case 'I':
			switch (ch3){ case 'v': case 'V': pDst[0] = 'd'; pDst[1] = 'v'; return; } break; // div = dv
		case 'u': case 'U':
			switch (ch3){ case 't': case 'T': pDst[0] = 'n'; pDst[1] = 'l'; return; } break; // dut = nl
		case 'z': case 'Z':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'd'; pDst[1] = 'z'; return; } break; // dzo = dz
		} break;
	case 'e': case 'E':
		switch (ch2){
		case 'l': case 'L':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'e'; pDst[1] = 'l'; return; } break; // ell = el
		case 'n': case 'N':
			switch (ch3){ case 'g': case 'G': pDst[0] = 'e'; pDst[1] = 'n'; return; } break; // eng = en
		case 'p': case 'P':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'e'; pDst[1] = 'o'; return; } break; // epo = eo
		case 's': case 'S':
			switch (ch3){ case 't': case 'T': pDst[0] = 'e'; pDst[1] = 't'; return; } break; // est = et
		case 'u': case 'U':
			switch (ch3){ case 's': case 'S': pDst[0] = 'e'; pDst[1] = 'u'; return; } break; // eus = eu
		case 'w': case 'W':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'e'; pDst[1] = 'e'; return; } break; // ewe = ee
		} break;
	case 'f': case 'F':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'o': case 'O': pDst[0] = 'f'; pDst[1] = 'o'; return; // fao = fo
				case 's': case 'S': pDst[0] = 'f'; pDst[1] = 'a'; return; // fas = fa
			} break; 
		case 'i': case 'I':
			switch (ch3){
				case 'j': case 'J': pDst[0] = 'f'; pDst[1] = 'j'; return; // fij = fj
				case 'n': case 'N': pDst[0] = 'f'; pDst[1] = 'i'; return; // fin = fi
			} break; 
		case 'r': case 'R':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'f'; pDst[1] = 'r'; return; // fra = fr
				case 'e': case 'E': pDst[0] = 'f'; pDst[1] = 'r'; return; // fre = fr
				case 'y': case 'Y': pDst[0] = 'f'; pDst[1] = 'y'; return; // fry = fy
			} break; 
		case 'u': case 'U':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'f'; pDst[1] = 'f'; return; } break; // ful = ff
		} break;
	case 'g': case 'G':
		switch (ch2){
		case 'e': case 'E':
			switch (ch3){
				case 'o': case 'O': pDst[0] = 'k'; pDst[1] = 'a'; return; // geo = ka
				case 'r': case 'R': pDst[0] = 'd'; pDst[1] = 'e'; return; // ger = de
			} break; 
		case 'l': case 'L':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'g'; pDst[1] = 'd'; return; // gla = gd
				case 'e': case 'E': pDst[0] = 'g'; pDst[1] = 'a'; return; // gle = ga
				case 'g': case 'G': pDst[0] = 'g'; pDst[1] = 'l'; return; // glg = gl
				case 'v': case 'V': pDst[0] = 'g'; pDst[1] = 'v'; return; // glv = gv
			} break; 
		case 'r': case 'R':
			switch (ch3){
				case 'e': case 'E': pDst[0] = 'e'; pDst[1] = 'l'; return; // gre = el
				case 'n': case 'N': pDst[0] = 'g'; pDst[1] = 'n'; return; // grn = gn
			} break; 
		case 'u': case 'U':
			switch (ch3){ case 'j': case 'J': pDst[0] = 'g'; pDst[1] = 'u'; return; } break; // guj = gu
		} break;
	case 'h': case 'H':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 't': case 'T': pDst[0] = 'h'; pDst[1] = 't'; return; // hat = ht
				case 'u': case 'U': pDst[0] = 'h'; pDst[1] = 'a'; return; // hau = ha
			} break; 
		case 'e': case 'E':
			switch (ch3){
				case 'b': case 'B': pDst[0] = 'h'; pDst[1] = 'e'; return; // heb = he
				case 'r': case 'R': pDst[0] = 'h'; pDst[1] = 'z'; return; // her = hz
			} break; 
		case 'i': case 'I':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'h'; pDst[1] = 'i'; return; } break; // hin = hi
		case 'm': case 'M':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'h'; pDst[1] = 'o'; return; } break; // hmo = ho
		case 'r': case 'R':
			switch (ch3){ case 'v': case 'V': pDst[0] = 'h'; pDst[1] = 'r'; return; } break; // hrv = hr
		case 'u': case 'U':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'h'; pDst[1] = 'u'; return; } break; // hun = hu
		case 'y': case 'Y':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'h'; pDst[1] = 'y'; return; } break; // hye = hy
		} break;
	case 'i': case 'I':
		switch (ch2){
		case 'b': case 'B':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'i'; pDst[1] = 'g'; return; } break; // ibo = ig
		case 'c': case 'C':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'i'; pDst[1] = 's'; return; } break; // ice = is
		case 'd': case 'D':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'i'; pDst[1] = 'o'; return; } break; // ido = io
		case 'i': case 'I':
			switch (ch3){ case 'i': case 'I': pDst[0] = 'i'; pDst[1] = 'i'; return; } break; // iii = ii
		case 'k': case 'K':
			switch (ch3){ case 'u': case 'U': pDst[0] = 'i'; pDst[1] = 'u'; return; } break; // iku = iu
		case 'l': case 'L':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'i'; pDst[1] = 'e'; return; } break; // ile = ie
		case 'n': case 'N':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'i'; pDst[1] = 'a'; return; // ina = ia
				case 'd': case 'D': pDst[0] = 'i'; pDst[1] = 'd'; return; // ind = id
			} break; 
		case 'p': case 'P':
			switch (ch3){ case 'k': case 'K': pDst[0] = 'i'; pDst[1] = 'k'; return; } break; // ipk = ik
		case 's': case 'S':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'i'; pDst[1] = 's'; return; } break; // isl = is
		case 't': case 'T':
			switch (ch3){ case 'a': case 'A': pDst[0] = 'i'; pDst[1] = 't'; return; } break; // ita = it
		} break;
	case 'j': case 'J':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){ case 'v': case 'V': pDst[0] = 'j'; pDst[1] = 'v'; return; } break; // jav = jv
		case 'p': case 'P':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'j'; pDst[1] = 'a'; return; } break; // jpn = ja
		} break;
	case 'k': case 'K':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'l': case 'L': pDst[0] = 'k'; pDst[1] = 'l'; return; // kal = kl
				case 'n': case 'N': pDst[0] = 'k'; pDst[1] = 'n'; return; // kan = kn
				case 's': case 'S': pDst[0] = 'k'; pDst[1] = 's'; return; // kas = ks
				case 't': case 'T': pDst[0] = 'k'; pDst[1] = 'a'; return; // kat = ka
				case 'u': case 'U': pDst[0] = 'k'; pDst[1] = 'r'; return; // kau = kr
				case 'z': case 'Z': pDst[0] = 'k'; pDst[1] = 'k'; return; // kaz = kk
			} break; 
		case 'h': case 'H':
			switch (ch3){ case 'm': case 'M': pDst[0] = 'k'; pDst[1] = 'm'; return; } break; // khm = km
		case 'i': case 'I':
			switch (ch3){
				case 'k': case 'K': pDst[0] = 'k'; pDst[1] = 'i'; return; // kik = ki
				case 'n': case 'N': pDst[0] = 'r'; pDst[1] = 'w'; return; // kin = rw
				case 'r': case 'R': pDst[0] = 'k'; pDst[1] = 'y'; return; // kir = ky
			} break; 
		case 'o': case 'O':
			switch (ch3){
				case 'm': case 'M': pDst[0] = 'k'; pDst[1] = 'v'; return; // kom = kv
				case 'n': case 'N': pDst[0] = 'k'; pDst[1] = 'g'; return; // kon = rg
				case 'r': case 'R': pDst[0] = 'k'; pDst[1] = 'o'; return; // kor = ko
			} break; 
		case 'u': case 'U':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'k'; pDst[1] = 'j'; return; // kua = kj
				case 'r': case 'R': pDst[0] = 'k'; pDst[1] = 'u'; return; // kur = ku
			} break; 
		} break;
	case 'l': case 'L':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'o': case 'O': pDst[0] = 'l'; pDst[1] = 'o'; return; // lao = lo
				case 't': case 'T': pDst[0] = 'l'; pDst[1] = 'a'; return; // lat = la
				case 'v': case 'V': pDst[0] = 'l'; pDst[1] = 'v'; return; // lav = lv
			} break; 
		case 'i': case 'I':
			switch (ch3){
				case 'm': case 'M': pDst[0] = 'l'; pDst[1] = 'i'; return; // lim = li
				case 'n': case 'N': pDst[0] = 'l'; pDst[1] = 'n'; return; // lin = ln
				case 't': case 'T': pDst[0] = 'l'; pDst[1] = 't'; return; // lit = lt
			} break; 
		case 't': case 'T':
			switch (ch3){ case 'z': case 'Z': pDst[0] = 'l'; pDst[1] = 'b'; return; } break; // ltz = lb
		case 'u': case 'U':
			switch (ch3){
				case 'b': case 'B': pDst[0] = 'l'; pDst[1] = 'u'; return; // lub = lu
				case 'g': case 'G': pDst[0] = 'l'; pDst[1] = 'g'; return; // lug = lg
			} break; 
		} break;
	case 'm': case 'M':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'c': case 'C': pDst[0] = 'm'; pDst[1] = 'k'; return; // mac = mk
				case 'h': case 'H': pDst[0] = 'm'; pDst[1] = 'h'; return; // mah = mh
				case 'l': case 'L': pDst[0] = 'm'; pDst[1] = 'l'; return; // mal = ml
				case 'o': case 'O': pDst[0] = 'm'; pDst[1] = 'i'; return; // mao = mi
				case 'r': case 'R': pDst[0] = 'm'; pDst[1] = 'z'; return; // mar = mz
				case 'y': case 'Y': pDst[0] = 'm'; pDst[1] = 's'; return; // may = ms
			} break; 
		case 'k': case 'K':
			switch (ch3){ case 'd': case 'D': pDst[0] = 'm'; pDst[1] = 'k'; return; } break; // mkd = mk
		case 'l': case 'L':
			switch (ch3){
				case 'g': case 'G': pDst[0] = 'm'; pDst[1] = 'g'; return; // mlg = mg
				case 't': case 'T': pDst[0] = 'm'; pDst[1] = 't'; return; // mlt = mt
			} break; 
		case 'o': case 'O':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'm'; pDst[1] = 'n'; return; } break; // mon = mn
		case 'r': case 'R':
			switch (ch3){ case 'i': case 'I': pDst[0] = 'm'; pDst[1] = 'i'; return; } break; // mri = mi
		case 's': case 'S':
			switch (ch3){ case 'a': case 'A': pDst[0] = 'm'; pDst[1] = 's'; return; } break; // msa = ms
		case 'y': case 'Y':
			switch (ch3){ case 'a': case 'A': pDst[0] = 'm'; pDst[1] = 'y'; return; } break; // mya = my
		} break;
	case 'n': case 'N':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'u': case 'U': pDst[0] = 'n'; pDst[1] = 'a'; return; // nau = na
				case 'v': case 'V': pDst[0] = 'n'; pDst[1] = 'v'; return; // nav = nv
			} break; 
		case 'b': case 'B':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'n'; pDst[1] = 'r'; return; } break; // nbl = nr
		case 'd': case 'D':
			switch (ch3){
				case 'e': case 'E': pDst[0] = 'n'; pDst[1] = 'd'; return; // nde = nd
				case 'o': case 'O': pDst[0] = 'n'; pDst[1] = 'g'; return; // ndo = ng
			} break; 
		case 'e': case 'E':
			switch (ch3){ case 'p': case 'P': pDst[0] = 'n'; pDst[1] = 'e'; return; } break; // nep = ne
		case 'l': case 'L':
			switch (ch3){ case 'd': case 'D': pDst[0] = 'n'; pDst[1] = 'l'; return; } break; // nld = nl
		case 'n': case 'N':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'n'; pDst[1] = 'n'; return; } break; // nno = nn
		case 'o': case 'O':
			switch (ch3){
				case 'b': case 'B': pDst[0] = 'n'; pDst[1] = 'b'; return; // nob = nb
				case 'r': case 'R': pDst[0] = 'n'; pDst[1] = 'o'; return; // nor = no
			} break; 
		case 'y': case 'Y':
			switch (ch3){ case 'a': case 'A': pDst[0] = 'n'; pDst[1] = 'y'; return; } break; // nya = ny
		} break;
	case 'o': case 'O':
		switch (ch2){
		case 'c': case 'C':
			switch (ch3){ case 'i': case 'I': pDst[0] = 'o'; pDst[1] = 'c'; return; } break; // oci = oc
		case 'j': case 'J':
			switch (ch3){ case 'i': case 'I': pDst[0] = 'o'; pDst[1] = 'j'; return; } break; // oji = oj
		case 'r': case 'R':
			switch (ch3){
				case 'i': case 'I': pDst[0] = 'o'; pDst[1] = 'r'; return; // ori = or
				case 'm': case 'M': pDst[0] = 'o'; pDst[1] = 'm'; return; // orm = om
			} break; 
		case 's': case 'S':
			switch (ch3){ case 's': case 'S': pDst[0] = 'o'; pDst[1] = 's'; return; } break; // oss = os
		} break;
	case 'p': case 'P':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'p'; pDst[1] = 'a'; return; } break; // pan = pa
		case 'e': case 'E':
			switch (ch3){ case 'r': case 'R': pDst[0] = 'f'; pDst[1] = 'a'; return; } break; // per = fa
		case 'l': case 'L':
			switch (ch3){ case 'i': case 'I': pDst[0] = 'p'; pDst[1] = 'i'; return; } break; // pli = pi
		case 'o': case 'O':
			switch (ch3){
				case 'l': case 'L': pDst[0] = 'p'; pDst[1] = 'l'; return; // pol = pl
				case 'r': case 'R': pDst[0] = 'p'; pDst[1] = 't'; return; // por = pt
			} break; 
		case 'u': case 'U':
			switch (ch3){ case 's': case 'S': pDst[0] = 'p'; pDst[1] = 's'; return; } break; // pus = ps
		} break;
	case 'q': case 'Q':
		switch (ch2){
		case 'u': case 'U':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'q'; pDst[1] = 'u'; return; } break; // que = qu
		} break;
	case 'r': case 'R':
		switch (ch2){
		case 'o': case 'O':
			switch (ch3){
				case 'h': case 'H': pDst[0] = 'r'; pDst[1] = 'm'; return; // roh = rm
				case 'n': case 'N': pDst[0] = 'r'; pDst[1] = 'o'; return; // ron = ro
			} break; 
		case 'u': case 'U':
			switch (ch3){
				case 'm': case 'M': pDst[0] = 'r'; pDst[1] = 'o'; return; // rum = ro
				case 'n': case 'N': pDst[0] = 'r'; pDst[1] = 'n'; return; // run = rn
				case 's': case 'S': pDst[0] = 'r'; pDst[1] = 'u'; return; // rus = ru
			} break; 
		} break;
	case 's': case 'S':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'g': case 'G': pDst[0] = 's'; pDst[1] = 'g'; return; // sag = sg
				case 'n': case 'N': pDst[0] = 's'; pDst[1] = 'a'; return; // san = sa
			} break; 
		case 'i': case 'I':
			switch (ch3){ case 'n': case 'N': pDst[0] = 's'; pDst[1] = 'i'; return; } break; // sin = si
		case 'l': case 'L':
			switch (ch3){
				case 'k': case 'K': pDst[0] = 's'; pDst[1] = 'k'; return; // slk = sk
				case 'o': case 'O': pDst[0] = 's'; pDst[1] = 'k'; return; // slo = sk
				case 'v': case 'V': pDst[0] = 's'; pDst[1] = 'l'; return; // slv = sl
			} break; 
		case 'm': case 'M':
			switch (ch3){
				case 'e': case 'K': pDst[0] = 's'; pDst[1] = 'e'; return; // sme = se
				case 'o': case 'O': pDst[0] = 's'; pDst[1] = 'm'; return; // smo = sm
			} break; 
		case 'n': case 'N':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 's'; pDst[1] = 'n'; return; // sna = sn
				case 'd': case 'D': pDst[0] = 's'; pDst[1] = 'd'; return; // snd = sd
			} break; 
		case 'o': case 'O':
			switch (ch3){
				case 'm': case 'M': pDst[0] = 's'; pDst[1] = 'o'; return; // som = so
				case 't': case 'T': pDst[0] = 's'; pDst[1] = 't'; return; // sot = st
			} break; 
		case 'p': case 'P':
			switch (ch3){ case 'a': case 'A': pDst[0] = 'e'; pDst[1] = 's'; return; } break; // spa = es
		case 'q': case 'Q':
			switch (ch3){ case 'i': case 'I': pDst[0] = 's'; pDst[1] = 'q'; return; } break; // sqi = sq
		case 'r': case 'R':
			switch (ch3){
				case 'd': case 'D': pDst[0] = 's'; pDst[1] = 'c'; return; // srd = sc
				case 'p': case 'P': pDst[0] = 's'; pDst[1] = 'z'; return; // srp = sz
			} break; 
		case 's': case 'S':
			switch (ch3){ case 'w': case 'W': pDst[0] = 's'; pDst[1] = 's'; return; } break; // ssw = ss
		case 'u': case 'U':
			switch (ch3){ case 'n': case 'N': pDst[0] = 's'; pDst[1] = 'u'; return; } break; // sun = su
		case 'w': case 'W':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 's'; pDst[1] = 'w'; return; // swa = sw
				case 'e': case 'E': pDst[0] = 's'; pDst[1] = 'v'; return; // swe = sv
			} break; 
		} break;
	case 't': case 'T':
		switch (ch2){
		case 'a': case 'A':
			switch (ch3){
				case 'h': case 'H': pDst[0] = 't'; pDst[1] = 'y'; return; // tah = ty
				case 'm': case 'M': pDst[0] = 't'; pDst[1] = 'a'; return; // tam = ta
				case 't': case 'T': pDst[0] = 't'; pDst[1] = 't'; return; // tat = tt
			} break; 
		case 'e': case 'E':
			switch (ch3){ case 'l': case 'L': pDst[0] = 't'; pDst[1] = 'e'; return; } break; // tel = te
		case 'g': case 'G':
			switch (ch3){
				case 'k': case 'K': pDst[0] = 't'; pDst[1] = 'g'; return; // tgk = tg
				case 'l': case 'L': pDst[0] = 't'; pDst[1] = 'l'; return; // tgl = tl
			} break; 
		case 'h': case 'H':
			switch (ch3){ case 'a': case 'A': pDst[0] = 't'; pDst[1] = 'h'; return; } break; // tha = th
		case 'i': case 'I':
			switch (ch3){
				case 'b': case 'B': pDst[0] = 'b'; pDst[1] = 'o'; return; // tib = bo
				case 'r': case 'R': pDst[0] = 't'; pDst[1] = 'i'; return; // tir = ti
			} break; 
		case 'o': case 'O':
			switch (ch3){ case 'n': case 'N': pDst[0] = 't'; pDst[1] = 'o'; return; } break; // ton = to
		case 's': case 'S':
			switch (ch3){
				case 'n': case 'N': pDst[0] = 't'; pDst[1] = 'n'; return; // tsn = tn
				case 'o': case 'O': pDst[0] = 't'; pDst[1] = 's'; return; // tso = ts
			} break; 
		case 'u': case 'U':
			switch (ch3){
				case 'k': case 'K': pDst[0] = 't'; pDst[1] = 'k'; return; // tuk = tk
				case 'r': case 'R': pDst[0] = 't'; pDst[1] = 'r'; return; // tur = tr
			} break; 
		case 'w': case 'W':
			switch (ch3){ case 'i': case 'I': pDst[0] = 't'; pDst[1] = 'w'; return; } break; // twi = tw
		} break;
	case 'u': case 'U':
		switch (ch2){
		case 'i': case 'I':
			switch (ch3){ case 'g': case 'G': pDst[0] = 'u'; pDst[1] = 'g'; return; } break; // uig = ug
		case 'k': case 'K':
			switch (ch3){ case 'r': case 'R': pDst[0] = 'u'; pDst[1] = 'k'; return; } break; // ukr = uk
		case 'r': case 'R':
			switch (ch3){ case 'd': case 'D': pDst[0] = 'u'; pDst[1] = 'r'; return; } break; // urd = ur
		case 'z': case 'Z':
			switch (ch3){ case 'b': case 'B': pDst[0] = 'u'; pDst[1] = 'z'; return; } break; // uzb = uz
		} break;
	case 'v': case 'V':
		switch (ch2){
		case 'e': case 'E':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'v'; pDst[1] = 'e'; return; } break; // ven = ve
		case 'i': case 'I':
			switch (ch3){ case 'e': case 'E': pDst[0] = 'v'; pDst[1] = 'i'; return; } break; // vie = vi
		case 'o': case 'O':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'v'; pDst[1] = 'o'; return; } break; // vol = vo
		} break;
	case 'w': case 'W':
		switch (ch2){
		case 'e': case 'E':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'c'; pDst[1] = 'y'; return; } break; // wel = cy
		case 'l': case 'L':
			switch (ch3){ case 'n': case 'N': pDst[0] = 'w'; pDst[1] = 'a'; return; } break; // wln = wa
		case 'o': case 'O':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'w'; pDst[1] = 'o'; return; } break; // wol = wo
		} break;
	case 'x': case 'X':
		switch (ch2){
		case 'h': case 'H':
			switch (ch3){ case 'o': case 'O': pDst[0] = 'x'; pDst[1] = 'h'; return; } break; // xho = xh
		} break;
	case 'y': case 'Y':
		switch (ch2){
		case 'i': case 'I':
			switch (ch3){ case 'd': case 'D': pDst[0] = 'y'; pDst[1] = 'i'; return; } break; // yid = yi
		case 'o': case 'O':
			switch (ch3){ case 'r': case 'R': pDst[0] = 'y'; pDst[1] = 'o'; return; } break; // yor = yo
		} break;
	case 'z': case 'Z':
		switch (ch2){
		case 'h': case 'H':
			switch (ch3){
				case 'a': case 'A': pDst[0] = 'z'; pDst[1] = 'a'; return; // zha = za
				case 'o': case 'O': pDst[0] = 'z'; pDst[1] = 'h'; return; // zho = zh
			} break; 
		case 'u': case 'U':
			switch (ch3){ case 'l': case 'L': pDst[0] = 'z'; pDst[1] = 'u'; return; } break; // zul = zu
		} break;
	}

	// default
	pDst[0] = ' ';
	pDst[1] = ' ';

	return;
}
