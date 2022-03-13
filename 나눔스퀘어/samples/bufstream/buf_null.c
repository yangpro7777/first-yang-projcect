/* ----------------------------------------------------------------------------
 * File: buf_null.c
 *
 * Desc: Empty buffered stream IO for first pass of 2-pass operation
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
#include "buf_null.h"
#include "auxinfo.h"

// implementation structure
struct impl_stream
{
  uint8_t *bfr;
  uint32_t bfr_size;  // allocated size
  uint32_t chunk_size;
  uint64_t bytecount;
};


static uint32_t fw_usable_bytes(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  return p->bfr_size;
}


// request the buffer with at least numbytes-bytes
static uint8_t *fw_request(bufstream_tt *bs, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  if (!numbytes){};  // remove compile warning
  return p->bfr;
}


// confirm numbytes-bytes filled in in requested after last "request"-call
static uint32_t fw_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  bs->Buf_IO_struct->bytecount += numbytes;
  return numbytes;
}


// pretend to put numbytes-bytes into bufsteam
static uint32_t fw_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  if (!ptr){};  // remove compile warning
  bs->Buf_IO_struct->bytecount += numbytes;
  return numbytes;
}


// maximal chunk-size in buffer-mode (i.e. for "request"-call)
static uint32_t fw_chunksize(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->chunk_size;
}


//inform bufstream about some additional info - for example
// to inform MUXer about encoding-units
static uint32_t fw_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  if (offs){};  // remove compile warning
  switch(info_ID)
  {
    case BYTECOUNT_INFO:
      {
        uint64_t *ptr = (uint64_t*)info_ptr;
        if(ptr && (info_size == sizeof(int64_t)))
          *ptr = bs->Buf_IO_struct->bytecount;
      }
      break;
  }
  return BS_OK;
}


static uint32_t fw_split(bufstream_tt *bs)
{
  if (!bs){};  // remove compile warning
  return 0;
}


static void fw_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (Abort){};  // remove compile warning
  free(p->bfr);
  free(p);
  bs->Buf_IO_struct = NULL;
}


static void fw_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


int32_t init_null_buf_write(bufstream_tt *bs, uint32_t bufsize,  void (*DisplayError)(char *txt))
{
  if (DisplayError){};  // remove compile warning

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if(!bs->Buf_IO_struct)
  {
    return BS_ERROR;
  }

  bs->Buf_IO_struct->bfr = (uint8_t*)malloc(bufsize);
  if(!bs->Buf_IO_struct->bfr)
  {
    free (bs->Buf_IO_struct);
    return BS_ERROR;
  }

  bs->Buf_IO_struct->bfr_size   = bufsize;
  bs->Buf_IO_struct->chunk_size = bufsize;
  bs->Buf_IO_struct->bytecount  = 0;

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


bufstream_tt *open_null_buf_write(uint32_t bufsize, void (*DisplayError)(char *txt))
{
  bufstream_tt *p;
  p = (bufstream_tt*)malloc(sizeof(bufstream_tt));
  if(p)
  {
    if(init_null_buf_write(p, bufsize, DisplayError) != BS_OK)
    {
      free(p);
      p = NULL;
    }
  }
  return p;
}


void close_null_buf(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

