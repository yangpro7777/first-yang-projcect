/* ----------------------------------------------------------------------------
 * File: buf_idx.c
 *
 * Desc: Buffered stream splitter for index output
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
#include "bufstrm.h"
#include "auxinfo.h"

#define MAX_VAU_BUF  300        // 10 seconds worth at 30fps
#define MAX_AAU_BUF  420        // 10 seconds worth at 48kHz

//implementation structure
struct impl_stream
{
  bufstream_tt         *main_bs;       // bufstream to pass video info to
  bufstream_tt         *video_bs;      // bufstream to pass video info to
  bufstream_tt         *audio_bs;      // bufstream to pass audio info to
  struct video_au_info *vau_info;      // buffer for video au info
  struct audio_au_info *aau_info;      // buffer for audio au info
  int32_t vau_start,    vau_end;
  int32_t aau_start,    aau_end;
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

static int32_t write_vau_info(bufstream_tt *bs)
{
  bs->Buf_IO_struct->video_bs->copybytes(bs->Buf_IO_struct->video_bs,
                                         (uint8_t*)&bs->Buf_IO_struct->vau_info[bs->Buf_IO_struct->vau_start],
                                         sizeof(struct video_au_info));
  bs->Buf_IO_struct->video_bs->auxinfo(bs->Buf_IO_struct->video_bs, 0, VIDEO_AU_INFO, NULL, 0);
  bs->Buf_IO_struct->vau_start++;
  if(bs->Buf_IO_struct->vau_start >= MAX_VAU_BUF)
    bs->Buf_IO_struct->vau_start = 0;
  return BS_OK;
}


static int32_t write_aau_info(bufstream_tt *bs)
{
  bs->Buf_IO_struct->audio_bs->copybytes(bs->Buf_IO_struct->audio_bs,
                                         (uint8_t*)&bs->Buf_IO_struct->aau_info[bs->Buf_IO_struct->aau_start],
                                         sizeof(struct audio_au_info));
  bs->Buf_IO_struct->audio_bs->auxinfo(bs->Buf_IO_struct->audio_bs, 0, AUDIO_AU_INFO, NULL, 0);
  bs->Buf_IO_struct->aau_start++;
  if(bs->Buf_IO_struct->aau_start >= MAX_AAU_BUF)
    bs->Buf_IO_struct->aau_start = 0;
  return BS_OK;
}


static void flush_vau_info(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  int32_t i;

  i = p->vau_start;
  while(i != p->vau_end)
  {
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&p->vau_info[i], sizeof(struct video_au_info));
    p->video_bs->auxinfo(p->video_bs, 0, VIDEO_AU_INFO, NULL, 0);
    i++;
    if(i >= MAX_VAU_BUF)
      i = 0;
  }  
}


static void flush_aau_info(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  int32_t i;

  i = p->aau_start;
  while(i != p->aau_end)
  {
    p->audio_bs->copybytes(p->audio_bs, (uint8_t*)&p->aau_info[i], sizeof(struct audio_au_info));
    p->audio_bs->auxinfo(p->audio_bs, 0, AUDIO_AU_INFO, NULL, 0);
    i++;
    if(i >= MAX_AAU_BUF)
      i = 0;
  }  
}


//inform bufstream about some additional info - for example
// to inform MUXer about encoding-units
static uint32_t idx_auxinfo (bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  switch(info_ID)
  {
    case FLUSH_BUFFER:
      if(p->main_bs)
        p->main_bs->auxinfo(p->main_bs, 0, info_ID, NULL, 0);
      if(p->video_bs)
        p->video_bs->auxinfo(p->video_bs, 0, info_ID, NULL, 0);
      if(p->audio_bs)
        p->audio_bs->auxinfo(p->audio_bs, 0, info_ID, NULL, 0);
      break;

    case VIDEO_STREAM_INFO:
      if(p->video_bs)
      {
        p->video_bs->copybytes(p->video_bs, (uint8_t*)info_ptr, info_size);
        p->video_bs->auxinfo(p->video_bs, 0, info_ID, NULL, 0);
      }
      break;

    case VIDEO_AU_INFO:
      if(p->video_bs)
      {
        int32_t i, j;
		int64_t i64ByteCount;

        memcpy(&p->vau_info[p->vau_end], info_ptr, info_size);
        p->vau_end++;
        if(p->vau_end >= MAX_VAU_BUF)
          p->vau_end = 0;

    // if the pointers catch up, we have to write one out
        if(p->vau_start == p->vau_end)
          write_vau_info(bs);

      // see if we can write any buffered ones out, to do so
    // check if the next au's filePos has been written out

        if(BS_OK == p->main_bs->auxinfo (p->main_bs, 0, BYTECOUNT_INFO, &i64ByteCount, sizeof (i64ByteCount)))
        {
          do
          {
            i = 0;
            j = p->vau_start + 1;
            if(j >= MAX_VAU_BUF)
              j = 0;

            if(j != p->vau_end)
            {
          // if the start filePos is > the next au's filePos, the next
          // au is a reference frame for the start au, check the next 
          // one instead
              if(p->vau_info[p->vau_start].filePos > p->vau_info[j].filePos)
              {
                j++;
                if(j >= MAX_VAU_BUF)
                  j = 0;
              }

              if((j != p->vau_end) &&
                 (p->vau_info[j].filePos < i64ByteCount))
              {
                write_vau_info(bs);
                i = 1;
              }
            }
          } while(i);
        }
      }
      break;

    case AUDIO_STREAM_INFO:
      if(p->audio_bs)
      {
        p->audio_bs->copybytes(p->audio_bs, (uint8_t*)info_ptr, info_size);
        p->audio_bs->auxinfo(p->audio_bs, 0, info_ID, NULL, 0);
      }
      break;

    case AUDIO_AU_INFO:
      if(p->audio_bs)
      {
        int32_t i, j;
		int64_t i64ByteCount;

        memcpy(&p->aau_info[p->aau_end], info_ptr, info_size);
        p->aau_end++;
        if(p->aau_end >= MAX_AAU_BUF)
          p->aau_end = 0;

      // see if we should write one out
        if(p->aau_start == p->aau_end)
          write_aau_info(bs);

        if(BS_OK == p->main_bs->auxinfo (p->main_bs, 0, BYTECOUNT_INFO, &i64ByteCount, sizeof (i64ByteCount)))
        {
          do
          {
            j = p->aau_start + 1;
            if(j >= MAX_AAU_BUF)
              j = 0;

            i = 0;
            if((j != p->aau_end) &&
               (p->aau_info[j].filePos < i64ByteCount))
            {
              write_aau_info(bs);
              i = 1;
            }
          } while(i);
        }
      }
      break;

    case SPLIT_OUTPUT:
      if(p->audio_bs)
      {
        flush_aau_info(bs);
        p->aau_start = 0;
        p->aau_end = 0;
        memset(p->aau_info, 0, sizeof(struct audio_au_info) * MAX_AAU_BUF);
        p->audio_bs->auxinfo(p->audio_bs, offs, info_ID, info_ptr, info_size);
      }
      if(p->video_bs)
      {
        flush_vau_info(bs);
        p->vau_start = 0;
        p->vau_end = 0;
        memset(p->vau_info, 0, sizeof(struct video_au_info) * MAX_VAU_BUF);
        p->video_bs->auxinfo(p->video_bs, offs, info_ID, info_ptr, info_size);
      }
      // send this message on to the main_bs as well!!!

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

  if(p->vau_info)
  {
    if(!Abort)
      flush_vau_info(bs);
    free(p->vau_info);
  }

  if(p->aau_info)
  {
    if(!Abort)
      flush_aau_info(bs);
    free(p->aau_info);
  }

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
                                        bufstream_tt *audio_bs, bufstream_tt *video_bs,
                                        void (*DisplayError)(char *txt))
{
  if(DisplayError){};  // remove compile warning

  if(!main_bs)
  {
    return BS_ERROR;
  }

  memset (bs, 0, sizeof (bufstream_tt));

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if(!bs->Buf_IO_struct)
  {
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


  if(video_bs)
  {
    bs->Buf_IO_struct->video_bs = video_bs;
    bs->Buf_IO_struct->vau_start = 0;
    bs->Buf_IO_struct->vau_end = 0;
    bs->Buf_IO_struct->vau_info = (struct video_au_info*)malloc(sizeof(struct video_au_info) * MAX_VAU_BUF);
    if(!bs->Buf_IO_struct->vau_info)
    {
      return BS_ERROR;
    }
    memset(bs->Buf_IO_struct->vau_info, 0, sizeof(struct video_au_info) * MAX_VAU_BUF);
  }

  if(audio_bs)
  {
    bs->Buf_IO_struct->audio_bs = audio_bs;
    bs->Buf_IO_struct->aau_start = 0;
    bs->Buf_IO_struct->aau_end = 0;
    bs->Buf_IO_struct->aau_info = (struct audio_au_info*)malloc(sizeof(struct audio_au_info) * MAX_AAU_BUF);
    if(!bs->Buf_IO_struct->aau_info)
    {
      return BS_ERROR;
    }
    memset(bs->Buf_IO_struct->aau_info, 0, sizeof(struct audio_au_info) * MAX_AAU_BUF);
  }

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_index(bufstream_tt *main_bs,
                                              bufstream_tt *audio_idx, bufstream_tt *video_idx,
                                              void (*DisplayError)(char *txt))
{
  bufstream_tt *p;
  p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
  if(p)
  {
    if(BS_OK != init_bufstream_write_with_index (p, main_bs, audio_idx, video_idx, DisplayError))
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

