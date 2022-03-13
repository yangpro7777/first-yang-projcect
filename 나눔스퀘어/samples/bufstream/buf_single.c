/* ----------------------------------------------------------------------------
 * File: buf_single.c
 *
 * Desc: Stream Buffer to single-frame implementation
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
#include <memory.h>
#include "buf_single.h"
#include "auxinfo.h"


// implementation structure
struct impl_stream
{
  uint8_t *bfr;
  uint32_t idx;       // read-write index
  uint32_t bfr_size;  // allocated size
  uint32_t chunk_size;

  // the video encoder may call request to get a buffer for the next frame
  // before it sends a VIDEO_AU_CODE for the current frame. These keep
  // track if that happens so the bytes for the next frame can be copied
  // to the start of the buffer when the confirm call is made.
  uint8_t au_done_flag;
  uint8_t *last_request;

  // this bufstream is being used by the muxer, not a video or audio encoder
  uint8_t mux_flag;

  void *app_structure;
  buf_single_app_callback app_callback;
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

  p->au_done_flag = 0;
  if(p->idx + numbytes <= p->bfr_size)
    p->last_request = p->bfr + p->idx;
  else
    p->last_request = NULL;
  return p->last_request;
}


// confirm numbytes-bytes filled in in requested after last "request"-call
static uint32_t fw_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (p->au_done_flag &&
	 !p->idx &&
	 (p->last_request != p->bfr))
  {
    memcpy(p->bfr, p->last_request, numbytes);
  }
  p->idx += numbytes;
  return numbytes;
}


// put numbytes-bytes into bufsteam
static uint32_t fw_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  uint8_t *pc;

  if (p->mux_flag)
  {
    if (p->app_callback)
      p->app_callback(p->app_structure, ptr, numbytes);

    return numbytes;
  }

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
  struct impl_stream* p = bs->Buf_IO_struct;

  if (p->mux_flag)
	return BS_OK;

  if (!info_size || offs){};  // remove compile warning

  switch (info_ID)
  {
    case VIDEO_AU_CODE:
	  if (info_ptr)  // the muxer sends this message but with info_ptr = NULL
	  {
		// sent by the video encoder
        struct v_au_struct *au = (struct v_au_struct*)info_ptr;
        // v_au_struct is defined in auxinfo.h
        // au->length is the number of bytes the encoder put in the fifo 
        //    (the length of a coded frame)
        // au->type is the frame type I, B or P

        // tell the application a video frame is ready
	    if (p->app_callback)
          p->app_callback(p->app_structure, p->bfr, au->length);

        // clean the buffer after application "eats" it
        p->idx = 0;
	    p->au_done_flag = 1;
	  }
      break;

    case AUDIO_AU_CODE:
	  if (info_ptr)  // the muxer sends this message but with info_ptr = NULL
	  {
		// sent by the audio encoder
        struct a_au_struct *au = (struct a_au_struct*)info_ptr;
        // a_au_struct is defined in auxinfo.h
        // au->length is the number of bytes the encoder put in the fifo 
        //    (the length of a coded audio frame)

        // tell the application an audio frame is ready
	    if (p->app_callback)
          p->app_callback(p->app_structure, p->bfr, au->length);

        // clean the buffer after application "eats" it
        p->idx = 0;
	    p->au_done_flag = 1;
	  }
      break;

  }
  return BS_OK;
}


static uint32_t fw_split(bufstream_tt *bs)
{
  if (!bs){}; // remove compile warning
  return 0;
}


static void fw_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (Abort){};  // remove compile warning

  if (p->idx)
  {
    if (!p->mux_flag && p->app_callback)
      p->app_callback(p->app_structure, p->bfr, p->idx);
  }

  free(p);
  bs->Buf_IO_struct = NULL;
}


static void fw_free(bufstream_tt *bs)
{
  if (bs->Buf_IO_struct)
    bs->done(bs, 0);

  free(bs);
}


bufstream_tt *open_mem_buf_single(uint8_t *buffer,
								  uint32_t bufsize,
								  uint32_t chunk_size,
								  uint8_t is_muxed_stream,
								  void *app_structure,
                                  buf_single_app_callback app_callback)
{
  bufstream_tt *bs;
  bs = (bufstream_tt*)malloc(sizeof(bufstream_tt));
  if (!bs)
	return NULL;

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if (!bs->Buf_IO_struct)
  {
    free(bs);
    return NULL;
  }

  bs->Buf_IO_struct->bfr           = buffer;
  bs->Buf_IO_struct->bfr_size      = bufsize;
  bs->Buf_IO_struct->chunk_size    = chunk_size;
  bs->Buf_IO_struct->idx           = 0;
  bs->Buf_IO_struct->au_done_flag  = 0;
  bs->Buf_IO_struct->last_request  = NULL;
  bs->Buf_IO_struct->mux_flag      = is_muxed_stream;
;

  bs->Buf_IO_struct->app_structure = app_structure;
  bs->Buf_IO_struct->app_callback  = app_callback;

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

  return bs;
}


void close_mem_buf_single(bufstream_tt *bs)
{
  bs->done(bs, 0);
  bs->free(bs);
  bs = NULL;
}
