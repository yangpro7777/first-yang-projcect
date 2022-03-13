/* ----------------------------------------------------------------------------
 * File: buf_index.c
 *
 * Desc: Captures mcindextypes.h type muxer index output to a file or
 *       set of files
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bufstrm.h"
#include "auxinfo.h"
#include "mcindextypes.h"


#ifdef _BS_UNICODE

#define BSSTR_L(b) L##b
#define BSSTR_CHAR wchar_t
#define BSSTR_CPY(target, source) wcscpy(target,source)
#define BSSTR_LEN(text) (int)wcslen(text)
#define BSSTR_STRRCHR(text,character) wcsrchr(text,character)
#define BSSTR_SPRINTF swprintf
#define BSSTR_FOPEN _wfopen
#define BSSTR_REMOVE _wremove

#else

#define BSSTR_L(b) b
#define BSSTR_CHAR char
#define BSSTR_CPY(target, source) strcpy(target,source)
#define BSSTR_LEN(text) strlen(text)
#define BSSTR_STRRCHR(text,character) strrchr(text,character)
#define BSSTR_SPRINTF sprintf
#define BSSTR_FOPEN fopen
#define BSSTR_REMOVE remove

#endif


//stream structure
struct index_stream_info
{
  BSSTR_CHAR stream_name[_BS_MAX_PATH]; // stream filename
  FILE *index_fp;                       // file for stream index
  mcidx_index_header_t hdr;             // index header for this stream
};

//implementation structure
struct impl_stream
{
  bufstream_tt *main_bs;                // bufstream to pass video info to
  BSSTR_CHAR main_name[_BS_MAX_PATH];   // main filename
  BSSTR_CHAR base_name[_BS_MAX_PATH];   // base filename
  FILE *base_fp;                        // file handle for base index info
  uint32_t stream_count;                // number of streams
  struct index_stream_info *streams;    // stream info
  uint8_t single_index_flag;
  uint8_t append_buffer[4096];
};


static uint32_t idx_usable_bytes(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->usable_bytes (bs->Buf_IO_struct->main_bs);
}

static uint8_t *idx_request(bufstream_tt *bs, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->request (bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t idx_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->confirm (bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t idx_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->copybytes (bs->Buf_IO_struct->main_bs, ptr, numbytes);
}

static uint32_t idx_split(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->split (bs->Buf_IO_struct->main_bs);
}

static uint32_t idx_chunksize(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->chunksize (bs->Buf_IO_struct->main_bs);
}


static uint32_t idx_container_info(struct impl_stream* p, void *info_ptr, uint32_t info_size)
{
  mcidx_file_header_t *pFileHdr;
  int32_t name_len = 0;

  if ((p->streams != NULL) || !info_ptr || (info_size < MCIDX_INDEX_EXT_VERSION_0100_SIZE))
    return BS_ERROR;

  pFileHdr = (mcidx_file_header_t*)info_ptr;
  if (pFileHdr->nIndexCount == 0)
    return BS_ERROR;

  p->stream_count = pFileHdr->nIndexCount;

  p->streams = (struct index_stream_info*)malloc(p->stream_count * sizeof(struct index_stream_info));
  if (!p->streams)
    return BS_ERROR;

  memset(p->streams, 0, p->stream_count * sizeof(struct index_stream_info));

  if (pFileHdr->nMediaNameByteLength == 0)
  {
    name_len = (uint32_t)BSSTR_LEN(p->main_name);
    if (name_len > 0)
    {
#ifdef _BS_UNICODE
      name_len *= 2;
	  pFileHdr->nFlags |= MCIDX_STRING_IS_UNICODE;
#endif
      pFileHdr->nMediaNameByteLength = name_len;
    }
  }

  // write the header out
  if (fwrite(info_ptr, 1, info_size, p->base_fp) != info_size)
    return BS_ERROR;

  if (name_len > 0)
  {
    // write the header out
    if (fwrite(p->main_name, 1, name_len, p->base_fp) != name_len)
      return BS_ERROR;
  }

  return BS_OK;
}


static uint32_t idx_stream_info(struct impl_stream* p, uint32_t stream_idx, void *info_ptr, uint32_t info_size)
{
  struct index_stream_info *pStream;

  if (!p->streams || !info_ptr || (info_size < MCIDX_INDEX_HDR_VERSION_0100_SIZE) || (stream_idx >= p->stream_count))
    return BS_ERROR;

  pStream = &p->streams[stream_idx];

  if (!pStream->index_fp)
  {
    // save the header for later
    memcpy(&pStream->hdr, info_ptr, MCIDX_INDEX_HDR_VERSION_0100_SIZE);

#ifdef _BS_UNICODE
    BSSTR_SPRINTF(pStream->stream_name, _BS_MAX_PATH, BSSTR_L("%s.%u"), p->base_name, stream_idx);
#else
    BSSTR_SPRINTF(pStream->stream_name, BSSTR_L("%s.%u"), p->base_name, stream_idx);
#endif
    pStream->index_fp = BSSTR_FOPEN(pStream->stream_name, BSSTR_L("w+b"));
    if (!pStream->index_fp)
      return BS_ERROR;

    if (!p->single_index_flag)
    {
      mcidx_ext_index_header_t ext_hdr;
      BSSTR_CHAR *pStr;

	  // make an external index header and put it in the base index file
	  memset(&ext_hdr, 0, sizeof(mcidx_ext_index_header_t));
	  memcpy(ext_hdr.indexIdentifier, MCIDX_INDEX_EXT_ID, sizeof(MCIDX_INDEX_EXT_ID));
	  ext_hdr.nVersion = MCIDX_INDEX_EXT_VERSION_0100;
	  pStr = BSSTR_STRRCHR(pStream->stream_name, '\\');
	  if (pStr)
		  pStr++;
	  else
		  pStr = pStream->stream_name;
	  ext_hdr.nExtNameByteLength = (uint32_t)BSSTR_LEN(pStr);
#ifdef _BS_UNICODE
	  ext_hdr.nExtNameByteLength *= 2;
	  ext_hdr.nFlags |= MCIDX_STRING_IS_UNICODE;
#endif

      // write the external header
	  if (fwrite(&ext_hdr, 1, sizeof(mcidx_ext_index_header_t), p->base_fp) != sizeof(mcidx_ext_index_header_t))
        return BS_ERROR;

      // now the filename
	  if (fwrite(pStr, 1, ext_hdr.nExtNameByteLength, p->base_fp) != ext_hdr.nExtNameByteLength)
        return BS_ERROR;
    }
  }

  // write the header out
  if (fwrite(info_ptr, 1, info_size, pStream->index_fp) != info_size)
    return BS_ERROR;

  return BS_OK;
}


static uint32_t idx_au_info(struct impl_stream* p, uint32_t stream_idx, void *info_ptr, uint32_t info_size)
{
  struct index_stream_info *pStream;

  if (!p->streams || !info_ptr || (info_size == 0) || (stream_idx >= p->stream_count))
    return BS_ERROR;

  pStream = &p->streams[stream_idx];

  if (!pStream->index_fp)
    return BS_ERROR;

  // write the au out
  if (fwrite(info_ptr, 1, info_size, pStream->index_fp) != info_size)
    return BS_ERROR;

  pStream->hdr.nItemCount++;

  return BS_OK;
}


static uint32_t idx_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  switch(info_ID)
  {
    case INDEX_CONTAINER_INFO:
      return idx_container_info(p, info_ptr, info_size);
      break;

    case INDEX_STREAM_INFO:
      return idx_stream_info(p, offs, info_ptr, info_size);
      break;

    case INDEX_AU_INFO:
      return idx_au_info(p, offs, info_ptr, info_size);
      break;

    default:
    // pass it on if we don't handle it
      if(p->main_bs)
        p->main_bs->auxinfo(p->main_bs, offs, info_ID, info_ptr, info_size);
  }
  return BS_OK;
}


static void idx_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (p->streams)
  {
    uint32_t i;
    for (i = 0; i < p->stream_count; i++)
    {
      struct index_stream_info *pStream = &p->streams[i];
      if (pStream->index_fp)
	  {
        if (!fseek(pStream->index_fp, 0, SEEK_SET))
        {
          // write the index hdr with the correct AU count
          fwrite(&pStream->hdr, 1, MCIDX_INDEX_HDR_VERSION_0100_SIZE, pStream->index_fp);

          if (p->single_index_flag)
          {
            if (!fseek(pStream->index_fp, 0, SEEK_SET))
            {
              // append the index to the base index
              size_t bytes_read = 1;
              while (bytes_read > 0)
              {
                bytes_read = fread(p->append_buffer, 1, sizeof(p->append_buffer), pStream->index_fp);
                if (bytes_read > 0)
                  fwrite(p->append_buffer, 1, bytes_read, p->base_fp);
			  }
            }
          }
        }

		fclose(pStream->index_fp);
        if (p->single_index_flag)
        {
          // delete the stream index
          BSSTR_REMOVE(pStream->stream_name);
        }
	  }
    }
	free(p->streams);
  }

  if (p->base_fp)
	  fclose(p->base_fp);

  free(p);
  bs->Buf_IO_struct = NULL;
}


static void idx_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


int32_t init_bufstream_write_with_index(bufstream_tt *bs,
                                        bufstream_tt *main_bs,
                                        const BSSTR_CHAR *main_name,
                                        const BSSTR_CHAR *base_name,
                                        uint8_t single_index_flag)
{
  FILE *fp;

  memset (bs, 0, sizeof (bufstream_tt));

  fp = BSSTR_FOPEN(base_name, BSSTR_L("wb"));
  if (!fp)
  {
    return BS_ERROR;
  }

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if(!bs->Buf_IO_struct)
  {
    fclose(fp);
    return BS_ERROR;
  }
  memset (bs->Buf_IO_struct, 0, sizeof(struct impl_stream));


  bs->usable_bytes = idx_usable_bytes;
  bs->request      = idx_request;
  bs->confirm      = idx_confirm;
  bs->copybytes    = idx_copybytes;
  bs->split        = idx_split;
  bs->chunksize    = idx_chunksize;
  bs->free         = idx_free;
  bs->auxinfo      = idx_auxinfo;
  bs->done         = idx_done;
  bs->state        = NULL;
  bs->drive_ptr    = NULL;
  bs->drive        = NULL;

  bs->flags        = 0;

  bs->Buf_IO_struct->main_bs = main_bs;
  BSSTR_CPY(bs->Buf_IO_struct->main_name, main_name);
  BSSTR_CPY(bs->Buf_IO_struct->base_name, base_name);
  bs->Buf_IO_struct->base_fp = fp;
  bs->Buf_IO_struct->single_index_flag = single_index_flag;

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_index(bufstream_tt *main_bs,
                                              const BSSTR_CHAR *main_name,
                                              const BSSTR_CHAR *index_base_name,
                                              uint8_t single_index_flag)
{
  bufstream_tt *p;

  if(!main_bs || !index_base_name)
  {
    return NULL;
  }

  p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
  if(p)
  {
    if(BS_OK != init_bufstream_write_with_index (p, main_bs, main_name, index_base_name, single_index_flag))
    {
      free(p);
      p = NULL;
    }
  }
  return p;
}


void close_bufstream_write_with_index(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

