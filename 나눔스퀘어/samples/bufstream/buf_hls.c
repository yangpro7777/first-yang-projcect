/* ----------------------------------------------------------------------------
 * File: buf_hls.c
 *
 * Desc: Captures segment output from muxers and creates a HTTP Live Streaming
 *       playlist
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


#ifdef _BS_UNICODE

#define BSSTR_L(b) L##b
#define BSSTR_CHAR wchar_t
#define BSSTR_CAT(target, append) wcscat(target,append)
#define BSSTR_CPY(target, source) wcscpy(target,source)
#define BSSTR_LEN(text) (int)wcslen(text)
#define BSSTR_STRRCHR(text,character) wcsrchr(text,character)
#define BSSTR_SPRINTF swprintf
#define BSSTR_FPRINTF fwprintf
#define BSSTR_FOPEN _wfopen

#else

#define BSSTR_L(b) b
#define BSSTR_CHAR char
#define BSSTR_CAT(target, append) strcat(target,append)
#define BSSTR_CPY(target, source) strcpy(target,source)
#define BSSTR_LEN(text) strlen(text)
#define BSSTR_STRRCHR(text,character) strrchr(text,character)
#define BSSTR_SPRINTF sprintf
#define BSSTR_FPRINTF fprintf
#define BSSTR_FOPEN fopen

#endif



//implementation structure
struct impl_stream
{
  bufstream_tt *main_bs;                // bufstream to pass video info to
  BSSTR_CHAR uri_path[_BS_MAX_PATH];    // user supplied URI path
  BSSTR_CHAR uri_name[_BS_MAX_PATH];    // full URI name
  BSSTR_CHAR media_name[_BS_MAX_PATH];  // working buffer for media name
  FILE *playlist_fp;                    // file handle for the playlist
  uint32_t segment_count;               // number of segments received
  uint64_t max_duration;                // maximum duration of the segments
};


static uint32_t hls_usable_bytes(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->usable_bytes (bs->Buf_IO_struct->main_bs);
}

static uint8_t *hls_request(bufstream_tt *bs, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->request (bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t hls_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->confirm (bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t hls_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->copybytes (bs->Buf_IO_struct->main_bs, ptr, numbytes);
}

static uint32_t hls_split(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->split (bs->Buf_IO_struct->main_bs);
}

static uint32_t hls_chunksize(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->chunksize (bs->Buf_IO_struct->main_bs);
}


static uint32_t add_new_segment(struct impl_stream* p, void *info_ptr, uint32_t info_size)
{
  struct output_segment_info *segment_info = (struct output_segment_info*)info_ptr;

  if (!info_ptr || (info_size != sizeof(struct output_segment_info)))
    return BS_ERROR;

  p->media_name[0] = 0;
  p->media_name[1] = 0;
  p->uri_name[0] = 0;
  p->uri_name[1] = 0;

  p->main_bs->auxinfo (p->main_bs, 0, FILENAME_INFO, &p->media_name[0], sizeof(p->media_name));

  if (BSSTR_LEN(p->uri_path))
    BSSTR_CPY(p->uri_name, p->uri_path);

  if (BSSTR_LEN(p->media_name))
  {
	  BSSTR_CHAR *pPtr;
	  pPtr = BSSTR_STRRCHR(p->media_name, '\\');
	  if (!pPtr)
		pPtr = BSSTR_STRRCHR(p->media_name, '/');
	  if (pPtr)
		  pPtr++;
	  else
		  pPtr = p->media_name;
	  BSSTR_CAT(p->uri_name, pPtr);
  }
  else
  {
#ifdef _BS_UNICODE
	  BSSTR_SPRINTF(p->media_name, _BS_MAX_PATH, BSSTR_L("streaming%u.ts"), p->segment_count);
#else
	  BSSTR_SPRINTF(p->media_name, BSSTR_L("streaming%u.ts"), p->segment_count);
#endif
	  BSSTR_CAT(p->uri_name, p->media_name);
  }

  BSSTR_FPRINTF(p->playlist_fp, BSSTR_L("#EXTINF:%u,\n\n"), (uint32_t)(segment_info->duration / 10000000.0 + 0.5));
  BSSTR_FPRINTF(p->playlist_fp, BSSTR_L("%s\n\n"), p->uri_name);

  if (segment_info->duration > p->max_duration)
    p->max_duration = segment_info->duration;

  p->segment_count++;

  return BS_OK;
}


static uint32_t hls_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  switch(info_ID)
  {
    case OUTPUT_SEGMENT_INFO:
      return add_new_segment(p, info_ptr, info_size);
      break;
    default:
    // pass it on if we don't handle it
      if(p->main_bs)
        p->main_bs->auxinfo(p->main_bs, offs, info_ID, info_ptr, info_size);
  }
  return BS_OK;
}


static void hls_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (p->playlist_fp)
  {
    BSSTR_FPRINTF(p->playlist_fp, BSSTR_L("#EXT-X-TARGETDURATION:%u\n\n"), (uint32_t)(p->max_duration / 10000000.0 + 0.5));
    BSSTR_FPRINTF(p->playlist_fp, BSSTR_L("#EXT-X-ENDLIST\n"));
    fclose(p->playlist_fp);
  }

  free(p);
  bs->Buf_IO_struct = NULL;
}


static void hls_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


int32_t init_bufstream_write_with_hls(bufstream_tt *bs,
                                      bufstream_tt *main_bs,
                                      const BSSTR_CHAR *uri_path,
                                      const BSSTR_CHAR *playlist_name)
{
  FILE *fp;

  memset (bs, 0, sizeof (bufstream_tt));

  fp = BSSTR_FOPEN(playlist_name, BSSTR_L("wt"));
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


  bs->usable_bytes = hls_usable_bytes;
  bs->request      = hls_request;
  bs->confirm      = hls_confirm;
  bs->copybytes    = hls_copybytes;
  bs->split        = hls_split;
  bs->chunksize    = hls_chunksize;
  bs->free         = hls_free;
  bs->auxinfo      = hls_auxinfo;
  bs->done         = hls_done;
  bs->state        = NULL;
  bs->drive_ptr    = NULL;
  bs->drive        = NULL;

  bs->flags        = 0;

  bs->Buf_IO_struct->main_bs = main_bs;
  if (uri_path)
    BSSTR_CPY(bs->Buf_IO_struct->uri_path, uri_path);
  bs->Buf_IO_struct->playlist_fp = fp;

  BSSTR_FPRINTF(fp, BSSTR_L("#EXTM3U\n\n"));
  BSSTR_FPRINTF(fp, BSSTR_L("#EXT-X-MEDIA-SEQUENCE:1\n\n"));

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_hls(bufstream_tt *main_bs,
                                            const BSSTR_CHAR *uri_path,
                                            const BSSTR_CHAR *playlist_name)
{
  bufstream_tt *p;

  if(!main_bs || !playlist_name)
  {
    return NULL;
  }

  p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
  if(p)
  {
    if(BS_OK != init_bufstream_write_with_hls (p, main_bs, uri_path, playlist_name))
    {
      free(p);
      p = NULL;
    }
  }
  return p;
}


void close_bufstream_write_with_hls(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

