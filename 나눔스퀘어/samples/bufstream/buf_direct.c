/* ----------------------------------------------------------------------------
 * File: buf_direct.c
 *
 * Desc: direct bufstream, user must override internal functions
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */


#include "buf_direct.h"
#include <stdlib.h>
#include <string.h>


//implementation structure
struct impl_stream
{
	uint32_t dummy;	// C requires that a struct or union has at least one member
};


static uint32_t  direct_usable_bytes(bufstream_tt *bs)
{
    if (!bs){};  // remove compile warning
	return 0;
}


static uint8_t *direct_request(bufstream_tt *bs, uint32_t numbytes)
{
    if (!bs || !numbytes){}; // remove compile warning
	return NULL;
}


static uint32_t direct_confirm(bufstream_tt *bs, uint32_t numbytes)
{
    if (!bs || !numbytes){}; // remove compile warning
	return 0;
}


static uint32_t direct_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
    if (!bs || !ptr || !numbytes){}; // remove compile warning
	return 0;
}


static uint32_t direct_split (bufstream_tt *bs)
{
    if (!bs){};  // remove compile warning
	return BS_ERROR;
}


static uint32_t direct_chunksize(bufstream_tt *bs)
{
    if (!bs){};  // remove compile warning
	return 0;
}


static uint32_t direct_auxinfo (bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
    if (!bs || !offs || !info_ID || !info_size){};  // remove compile warning
	return BS_ERROR;
}


static void direct_done(bufstream_tt *bs, int32_t Abort)
{
    if (Abort){}; // remove compile warning
	if (bs->Buf_IO_struct)
	{
		free(bs->Buf_IO_struct);
		bs->Buf_IO_struct = NULL;
	}
}


static void direct_free(bufstream_tt *bs)
{
	if (bs->Buf_IO_struct)
		bs->done(bs, 0);

	free(bs);
}


static int32_t init_bufstream_direct(bufstream_tt *bs, void (*DisplayError)(char *txt))
{
	if (DisplayError){};  // remove compile warning

	memset (bs, 0, sizeof (bufstream_tt));

	bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
	if (!bs->Buf_IO_struct)
	{
		return BS_ERROR;
	}
	memset (bs->Buf_IO_struct, 0, sizeof(struct impl_stream));

	bs->usable_bytes = direct_usable_bytes;
	bs->request      = direct_request;
	bs->confirm      = direct_confirm;
	bs->copybytes    = direct_copybytes;
	bs->split        = direct_split;
	bs->chunksize    = direct_chunksize;
	bs->free         = direct_free;
	bs->auxinfo      = direct_auxinfo;
	bs->done         = direct_done;
	bs->state        = NULL;
	bs->drive_ptr    = NULL;
	bs->drive        = NULL;
	
	bs->flags        = 0;

	return BS_OK;
}


bufstream_tt *open_bufstream_direct(void (*DisplayError)(char *txt))
{
    bufstream_tt *p;

    p = (bufstream_tt*)malloc(sizeof(bufstream_tt));
    if (p)
    {
        if (BS_OK != init_bufstream_direct(p, DisplayError))
        {
            free(p);
            p = NULL;
        }
    }
    return p;
}


void close_bufstream_direct(bufstream_tt* bs, int32_t Abort)
{
	bs->done(bs, Abort);
	bs->free(bs);
}

