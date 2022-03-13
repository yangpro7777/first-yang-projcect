/* ----------------------------------------------------------------------------
 * File: buf_mem.c
 *
 * Desc: Buffered stream memory IO
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
#include "buf_mem.h"
#include "auxinfo.h"


// implementation structure
struct impl_stream
{
  uint8_t *user_bfr;
  uint32_t user_bfr_size;
  uint32_t user_idx;

  uint8_t *bfr;
  uint32_t idx;       // read-write index
  uint32_t bfr_size;  // allocated size
  uint32_t chunk_size;
};


static uint32_t fw_usable_bytes(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  return p->bfr_size - p->idx;
}


// request the buffer with at least numbytes-bytes
static uint8_t *fw_request(bufstream_tt *bs, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if(p->idx + numbytes <= p->bfr_size)
    return p->bfr + p->idx;

  if(p->idx)
  {
	if ((p->user_idx + p->idx > p->user_bfr_size) || (p->user_bfr == NULL))
      return NULL;

	memcpy(&p->user_bfr[p->user_idx], p->bfr, p->idx);
	p->user_idx += p->idx;
  }

  p->idx = 0;
  return p->bfr;
}


// confirm numbytes-bytes filled in in requested after last "request"-call
static uint32_t fw_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  bs->Buf_IO_struct->idx += numbytes;
  return numbytes;
}


// put numbytes-bytes into bufsteam
static uint32_t fw_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  uint8_t *pc;

  if ((pc = bs->request(bs, numbytes)) == NULL)
  {
    p->idx = 0;
    return 0;
  }
  memcpy(pc, ptr, numbytes);
  return bs->confirm(bs, numbytes);
}


// maximum chunk-size in buffer-mode (i.e. for "request"-call)
static uint32_t fw_chunksize(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->chunk_size;
}
  

static uint32_t fw_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  uint32_t *ptr;
  struct impl_stream* p = bs->Buf_IO_struct;
  if (offs){};

  switch (info_ID)
  {
    case BYTECOUNT_INFO:
	  ptr = (uint32_t*)info_ptr;
	  if (ptr && (info_size == sizeof(uint32_t)))
	  {
        if(p->idx)
		{
          if ((p->user_idx + p->idx > p->user_bfr_size) || (p->user_bfr == NULL))
            return 0;

	      memcpy(&p->user_bfr[p->user_idx], p->bfr, p->idx);
	      p->user_idx += p->idx;
          p->idx = 0;
		}
		*ptr = p->user_idx;
	  }
	  break;

    case FLUSH_BUFFER:
      if(NULL == p)
        return 0;

      p->user_idx = 0;
      p->idx = 0;

      break;

  }
  return BS_OK;
}


static uint32_t fw_split(bufstream_tt *bs)
{
  if (!bs){};
  return 0;
}


static void fw_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  if (Abort){};

  if (p->idx)
  {
    if ((p->user_idx + p->idx <= p->user_bfr_size) && (p->user_bfr != NULL))
      memcpy(&p->user_bfr[p->user_idx], p->bfr, p->idx);
  }

  free(p->bfr);
  free(p);
  bs->Buf_IO_struct = NULL;
}


static void fw_free(bufstream_tt *bs)
{
  if (bs->Buf_IO_struct)
    bs->done(bs, 0);

  free(bs);
}


int32_t init_mem_buf_write(bufstream_tt *bs, uint8_t *buffer, uint32_t bufsize,  void (*DisplayError)(char *txt))
{
  if (DisplayError){};  // remove compile warning

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if (!(bs->Buf_IO_struct))
  {
    return BS_ERROR;
  }

  bs->Buf_IO_struct->bfr = (uint8_t*)malloc(bufsize);
  if (!(bs->Buf_IO_struct->bfr))
  {
	free(bs->Buf_IO_struct);
    bs->Buf_IO_struct = NULL;
    return BS_ERROR;
  }

  bs->Buf_IO_struct->user_bfr      = buffer;
  bs->Buf_IO_struct->user_bfr_size = bufsize;
  bs->Buf_IO_struct->user_idx      = 0;

  bs->Buf_IO_struct->bfr_size      = bufsize;
  bs->Buf_IO_struct->chunk_size    = bufsize;
  bs->Buf_IO_struct->idx           = 0;

  bs->usable_bytes = fw_usable_bytes;
  bs->request      = fw_request;
  bs->confirm      = fw_confirm;
  bs->copybytes    = fw_copybytes;
  bs->split        = fw_split;
  bs->chunksize    = fw_chunksize;
  bs->free         = fw_free;
  bs->auxinfo      = fw_auxinfo;
  bs->done         = fw_done;
  bs->drive_ptr    = NULL;
  bs->drive        = NULL;

  bs->state        = NULL;
  bs->flags        = BS_FLAGS_DST;
  return BS_OK;
}


bufstream_tt *open_mem_buf_write(uint8_t *buffer, uint32_t bufsize, void (*DisplayError)(char *txt))
{
  bufstream_tt *p;
  p = (bufstream_tt*)malloc(sizeof(bufstream_tt));
  if (p)
  {
    if (init_mem_buf_write(p, buffer, bufsize, DisplayError) != BS_OK)
    {
      free(p);
      p = NULL;
    }
  }
  return p;
}


void close_mem_buf(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}
