/* ----------------------------------------------------------------------------
 * File: buf_mzl.c
 *
 * Desc: Buffered stream with .mzl file output
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
#include <windows.h>
#endif

#ifdef _BS_UNICODE
#define VALID_STR(x) wcslen((const wchar_t *)(x)) > 0
#else
#define VALID_STR(x) strlen((const char *)(x)) > 0
#endif

//implementation structure
struct impl_stream
{
  bufstream_tt *main_bs;        // our base bufstream
#ifdef _BS_UNICODE
  wchar_t mzlname[_BS_MAX_PATH];
#else
  char mzlname[_BS_MAX_PATH];
#endif
};


static int32_t sprintf_int64(char *str, int64_t value)
{
  uint64_t data;
  uint64_t place_mask = INT64_C(1000000000000000000);
  int32_t idx = 0;
  uint8_t data_flag = 0;
  int8_t place_value;

  if (value == 0)
  {
	str[0] = '0';
	str[1] = 0;
	return 1;
  }

  str[0] = 0;
  if (value < 0)
  {
    str[idx++] = '-';
    if (value == INT64_C(0x8000000000000000))
      data = INT64_C(0x8000000000000000);
    else
      data = (uint64_t)(-value);
  }
  else
  {
    data = (uint64_t)value;
  }

  while (place_mask > 0)
  {
    place_value = (uint8_t)(data / place_mask);
    if (place_value > 0)
    {
      data_flag = 1;
      data -= place_value * place_mask;
      str[idx++] = '0' + (char)place_value;
    }
    else if (data_flag)
    {
      str[idx++] = '0';
    }
    place_mask /= 10;
  }

  str[idx] = 0;

  return idx;
}


static void fw_mzl(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  char tmpStr[2048];
  char numStr[32];
  char fname[2048], *tmpPtr;
#ifdef _BS_UNICODE
  wchar_t fname1[_BS_MAX_PATH], *tmpPtr1;
#endif
  FILE *fp;
  int32_t filenumber;
  int64_t bytecount;

  // write the current file data to the mzl file
  if (VALID_STR(p->mzlname))
  {
    if (p->main_bs->auxinfo (p->main_bs, 0, FILENUMBER_INFO, &filenumber, sizeof(filenumber)) != BS_OK)
	  return;

	if (p->main_bs->auxinfo (p->main_bs, 0, BYTECOUNT_INFO, &bytecount, sizeof(bytecount)) != BS_OK)
	  return;

	if (!filenumber)
    {
#ifdef _BS_UNICODE
	  fp = _wfopen(p->mzlname, L"wt");
#else
 #ifndef __QNX__
	  fp = fopen(p->mzlname, "wt");
 #else
	  fp = fopen64(p->mzlname, "wt");
 #endif
#endif
	  if (fp)
	  {
		tmpStr[0] = 'D';
		tmpStr[1] = 'Z';
		tmpStr[2] = 'M';
		tmpStr[3] = 'P';
		tmpStr[4] = '\n';
		tmpStr[5] = 0;

	    if (fwrite(tmpStr, sizeof(char), strlen(tmpStr), fp) != strlen(tmpStr))
		{
		  fclose(fp);
		  fp = NULL;
		}
	  }
	}
	else
#ifdef _BS_UNICODE
	  fp = _wfopen(p->mzlname, L"at");
#else
 #ifndef __QNX__
	  fp = fopen(p->mzlname, "at");
 #else
	  fp = fopen64(p->mzlname, "at");
 #endif
#endif

	if (fp)
	{
#ifdef _BS_UNICODE
      if (p->main_bs->auxinfo (p->main_bs, 0, FILENAME_INFO, &fname1[0], _BS_MAX_PATH * 2) != BS_OK)
#else
      if (p->main_bs->auxinfo (p->main_bs, 0, FILENAME_INFO, &fname[0], _BS_MAX_PATH) != BS_OK)
#endif
	  {
		fclose(fp);
	    return;
	  }

#ifdef _BS_UNICODE
	  tmpPtr1 = wcsrchr(fname1, '\\');
	  if (!tmpPtr1)
		tmpPtr1 = &fname1[0];
	  else
		tmpPtr1++;

      WideCharToMultiByte(
        CP_ACP,
        0,
        tmpPtr1,
        (int32_t)wcslen(tmpPtr1),
        fname,
        2048,
	    NULL,
	    NULL);
	  tmpPtr = &fname[0];
#else
	  tmpPtr = strrchr(fname, '\\');
	  if (!tmpPtr)
		tmpPtr = &fname[0];
	  else
		tmpPtr++;
#endif

	  sprintf_int64(numStr, bytecount);
	  sprintf(tmpStr, "%s; %s\n", tmpPtr, numStr);
	  fwrite(tmpStr, sizeof(char), strlen(tmpStr), fp);
      fclose(fp);
      fp = NULL;
	}
  }
}


static uint32_t mzl_usable_bytes(bufstream_tt *bs)
{
    return bs->Buf_IO_struct->main_bs->usable_bytes (bs->Buf_IO_struct->main_bs);
}

static uint8_t *mzl_request(bufstream_tt *bs, uint32_t numbytes)
{
    return bs->Buf_IO_struct->main_bs->request (bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t mzl_confirm(bufstream_tt *bs, uint32_t numbytes)
{
    return bs->Buf_IO_struct->main_bs->confirm (bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t mzl_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
    return bs->Buf_IO_struct->main_bs->copybytes (bs->Buf_IO_struct->main_bs, ptr, numbytes);
}

static uint32_t mzl_chunksize(bufstream_tt *bs)
{
    return bs->Buf_IO_struct->main_bs->chunksize (bs->Buf_IO_struct->main_bs);
}

//inform bufstream about some additional info - for example
// to inform MUXer about encoding-units
static uint32_t mzl_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  switch (info_ID)
  {
	case SPLIT_OUTPUT:
      fw_mzl(bs);
	  break;
  }
  return bs->Buf_IO_struct->main_bs->auxinfo(bs->Buf_IO_struct->main_bs, offs, info_ID, info_ptr, info_size);
}


static uint32_t mzl_split(bufstream_tt *bs)
{
  fw_mzl(bs);
  return bs->Buf_IO_struct->main_bs->split (bs->Buf_IO_struct->main_bs);
}


static void mzl_done(bufstream_tt *bs, int32_t Abort)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  if (Abort){};  // remove compile warning

#ifdef __NO_SINGLE_FILE_MZL__
  int32_t i;
  if (p->main_bs->auxinfo (p->main_bs, 0, FILENUMBER_INFO, &i, sizeof(i)) == BS_OK)
  {
    if (i)
	  fw_mzl(bs);
  }
#else
  fw_mzl(bs);
#endif

  free(p);
  bs->Buf_IO_struct = NULL;
}


static void mzl_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


int32_t init_bufstream_write_with_mzl(bufstream_tt *bs, bufstream_tt *main_bs,
                                      void (*DisplayError)(char *txt))
{
#ifdef _BS_UNICODE
  wchar_t *tmpPtr;
#else
  char *tmpPtr;
#endif

  if (DisplayError){};  // remove compile warning

  if (!main_bs)
  {
    return BS_ERROR;
  }

  memset (bs, 0, sizeof (bufstream_tt));

  bs->Buf_IO_struct = (struct impl_stream*)malloc(sizeof(struct impl_stream));
  if (!bs->Buf_IO_struct)
  {
    return BS_ERROR;
  }
  memset (bs->Buf_IO_struct, 0, sizeof(struct impl_stream));


  bs->usable_bytes = mzl_usable_bytes;
  bs->request      = mzl_request;
  bs->confirm      = mzl_confirm;
  bs->copybytes    = mzl_copybytes;
  bs->split        = mzl_split;
  bs->chunksize    = mzl_chunksize;
  bs->free         = mzl_free;
  bs->auxinfo      = mzl_auxinfo;
  bs->done         = mzl_done;
  bs->state        = NULL;
  bs->drive_ptr    = NULL;
  bs->drive        = NULL;

  bs->flags        = 0;

  bs->Buf_IO_struct->main_bs = main_bs;

  if (bs->Buf_IO_struct->main_bs->auxinfo(bs->Buf_IO_struct->main_bs, 0, FILENAME_INFO, &bs->Buf_IO_struct->mzlname[0], 255) != BS_OK)
  {
    free(bs->Buf_IO_struct);
    bs->Buf_IO_struct = NULL;
	return BS_ERROR;
  }

#ifdef _BS_UNICODE
  tmpPtr = wcsrchr(bs->Buf_IO_struct->mzlname, '.');
  if (tmpPtr)
	  wcscpy(tmpPtr, L"");
  wcscat(bs->Buf_IO_struct->mzlname, L".mzl");
#else
  tmpPtr = strrchr(bs->Buf_IO_struct->mzlname, '.');
  if (tmpPtr)
    tmpPtr[0] = 0;
  strcat(bs->Buf_IO_struct->mzlname, ".mzl");
#endif

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_mzl(bufstream_tt *main_bs,
                                            void (*DisplayError)(char *txt))
{
    bufstream_tt *p;
    p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
    if(p)
    {
        if( BS_OK != init_bufstream_write_with_mzl (p, main_bs, DisplayError))
        {
            free(p);
            p = NULL;
        }
    }
    return p;
}



void close_bufstream_write_with_mzl(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

