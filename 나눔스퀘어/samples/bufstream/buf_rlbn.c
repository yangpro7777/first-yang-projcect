/* ----------------------------------------------------------------------------
 * File: buf_rlbn.c
 *
 * Desc: Buffered stream splitter for DVD RLBN output
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
#include "mcdefs.h"


//implementation structure
struct impl_stream
{
  bufstream_tt *main_bs;        // main bufstream to pass info to
  bufstream_tt *rlbn_bs;        // bufstream to write rlbn info to
  uint32_t      last_vobu_rlbn; // last vobu rlbn
  uint32_t      rlbn_count;
};


static uint32_t rlbn_usable_bytes(bufstream_tt *bs)
{
  if (bs->Buf_IO_struct->main_bs)
    return bs->Buf_IO_struct->main_bs->usable_bytes (bs->Buf_IO_struct->main_bs);
  else
	return BS_ERROR;
}

static uint8_t *rlbn_request(bufstream_tt *bs, uint32_t numbytes)
{
  if (bs->Buf_IO_struct->main_bs)
    return bs->Buf_IO_struct->main_bs->request (bs->Buf_IO_struct->main_bs, numbytes);
  else
	return NULL;
}

static uint32_t rlbn_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  if (bs->Buf_IO_struct->main_bs)
    return bs->Buf_IO_struct->main_bs->confirm (bs->Buf_IO_struct->main_bs, numbytes);
  else
	return BS_ERROR;
}

static uint32_t rlbn_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  if (bs->Buf_IO_struct->main_bs)
    return bs->Buf_IO_struct->main_bs->copybytes (bs->Buf_IO_struct->main_bs, ptr, numbytes);
  else
	return BS_ERROR;
}

static uint32_t rlbn_split(bufstream_tt *bs)
{
  if (bs->Buf_IO_struct->main_bs)
    return bs->Buf_IO_struct->main_bs->split (bs->Buf_IO_struct->main_bs);
  else
	return BS_ERROR;
}

static uint32_t rlbn_chunksize(bufstream_tt *bs)
{
  if (bs->Buf_IO_struct->main_bs)
    return bs->Buf_IO_struct->main_bs->chunksize (bs->Buf_IO_struct->main_bs);
  else
	return BS_ERROR;
}


static uint32_t rlbn_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  switch (info_ID)
  {
    case DVD_RLBN_INFO:
	  if (info_ptr && (info_size == sizeof(struct dvd_nv_info)))
	  {
		struct dvd_nv_info *nv_info = (struct dvd_nv_info*)info_ptr;
	    if (p->rlbn_bs)
	      p->rlbn_bs->copybytes(p->rlbn_bs, (uint8_t*)info_ptr, sizeof(struct dvd_nv_info));
        p->last_vobu_rlbn = nv_info->nv_sa;
		p->rlbn_count++;
	  }
	  break;

	case DVD_GET_LAST_RLBN:
		{
		  uint32_t *i = (uint32_t*)info_ptr;
	      if (i && (info_size == sizeof(uint32_t)))
            *i = p->last_vobu_rlbn;
		}
	  break;

	case DVD_GET_RLBN_COUNT:
		{
		  uint32_t *i = (uint32_t*)info_ptr;
	      if (i && (info_size == sizeof(uint32_t)))
            *i = p->rlbn_count;
		}
		return BS_OK; // do not call the auxinfo call below as it may not be valid anymore!!
	  break;
  }

  // pass on everything to main_bs
  if (p->main_bs)
    p->main_bs->auxinfo(p->main_bs, offs, info_ID, info_ptr, info_size);
  return BS_OK;
}


static void rlbn_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (Abort){};  // remove compile warning
  free(p);
  bs->Buf_IO_struct = NULL;
}


static void rlbn_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


void switch_main_bs(bufstream_tt *bs, bufstream_tt *main_bs)
{
  bs->Buf_IO_struct->main_bs = main_bs;
}


int32_t init_bufstream_write_with_rlbn_index(bufstream_tt *bs,
                                             bufstream_tt *main_bs,
                                             bufstream_tt *rlbn_bs,
                                             void (*DisplayError)(char *txt))
{
  if (DisplayError){};  // remove compile warning

  memset (bs, 0, sizeof (bufstream_tt));

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if (!bs->Buf_IO_struct)
  {
    return BS_ERROR;
  }
  memset (bs->Buf_IO_struct, 0, sizeof(struct impl_stream));

  bs->usable_bytes = rlbn_usable_bytes;
  bs->request      = rlbn_request;
  bs->confirm      = rlbn_confirm;
  bs->copybytes    = rlbn_copybytes;
  bs->split        = rlbn_split;
  bs->chunksize    = rlbn_chunksize;
  bs->free         = rlbn_free;
  bs->auxinfo      = rlbn_auxinfo;
  bs->done         = rlbn_done;
  bs->state        = NULL;
  bs->drive_ptr    = NULL;
  bs->drive        = NULL;

  bs->flags        = 0;

  if (main_bs)
    bs->Buf_IO_struct->main_bs = main_bs;

  if (rlbn_bs)
    bs->Buf_IO_struct->rlbn_bs = rlbn_bs;

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_rlbn_index(bufstream_tt *main_bs,
                                                   bufstream_tt *rlbn_bs,
                                                   void (*DisplayError)(char *txt))
{
    bufstream_tt *p;
    p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
    if(p)
    {
        if( BS_OK != init_bufstream_write_with_rlbn_index (p, main_bs, rlbn_bs, DisplayError))
        {
            free(p);
            p = NULL;
        }
    }
    return p;
}


void close_bufstream_write_with_rlbn_index(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

