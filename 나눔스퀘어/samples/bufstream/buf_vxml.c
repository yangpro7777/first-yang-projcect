/* ----------------------------------------------------------------------------
 * File: buf_vxml.c
 *
 * Desc: Buffered stream splitter for XML index output of a video stream
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

#include "buf_vxml.h"
#include "auxinfo.h"
#include "mcdefs.h"

#define DO_BASE64   // represent the header data in base64, else ASCII

#ifdef MC_LITTLEENDIAN
#define BS_INT32(x) ((((x)&0x000000FF)<<24)|(((x)&0x0000FF00)<<8)|(((x)&0x00FF0000)>>8)|(((x)&0xFF000000)>>24))
#else
#define BS_INT32(x) (x)
#endif

static int32_t picture_rates[16] =
{
  0,
  1126125,
  1125000,
  1080000,
  900900,
  900000,
  540000,
  450450,
  450000,
  0,0,0,0,0,0,0
};

struct header
{
  int32_t  len;
  int32_t  size;
  uint8_t *data;
};


struct picture_data
{
  int32_t       present;
  struct header headerData;
  int64_t       relPosition;
};


struct access_unit
{
  int32_t             unitType;
  int64_t             gopPosition;
  int64_t             firstPTS;
  int64_t             lastPTS;
  int64_t             gopDuration;
  int32_t             gopSize;
  int32_t             rffFlag;
  struct header       headerData;
  struct picture_data frames[MAXN];
};

//implementation structure
struct impl_stream
{
  bufstream_tt      *main_bs;        // bufstream to pass video info to
  bufstream_tt      *video_bs;       // bufstream to pass video info to
  uint8_t           *picBuffer;      // pointer to the buffer the picture is written to
  int64_t            byteCount;      // current byte count
  int64_t            duration;
  struct header      curGOPHeader;
  struct header      curPICHeader;
  int64_t            curGOPPos;
  int64_t            curPICPos;
  int32_t            initOutput;
  int64_t            clocks_per_frame;
  int64_t            clocks_per_field;
  int32_t            curRFFFlag;
  uint8_t            bPicStartFlag;
  uint8_t            bPicGotHdrFlag;
  char              *workBuffer;     // for encoding and formatting
  uint32_t           workBufferSize; // size of the work buffer
  struct access_unit aunit;          // buffer for video au info
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


#ifdef DO_BASE64

// base64 encoding stuff
static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// encode 3 8-bit binary bytes as 4 '6-bit' characters
static void encode_block(uint8_t in[3], uint8_t out[4], int32_t len)
{
  out[0] = b64[ in[0] >> 2 ];
  out[1] = b64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (uint8_t) (len > 1 ? b64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (uint8_t) (len > 2 ? b64[ in[2] & 0x3f ] : '=');
}

// base64 encode a stream adding padding and line breaks
static void b64_encode(bufstream_tt *bs, uint8_t *ptr, int32_t length, int32_t indent)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  uint8_t in[3], out[4];
  int32_t i, j, len, blocksout = 0, iLen = 0;
  uint32_t encoded_len = length / 3 * 4 + 64;

  if(encoded_len > p->workBufferSize)
  {
    if(p->workBufferSize)
      free(p->workBuffer);
    p->workBuffer = (char*)malloc(encoded_len);
    p->workBufferSize = encoded_len;
  }

  for(i = 0; i < indent; i++)
    p->workBuffer[i] = ' ';
  p->workBuffer[i] = 0;

  strcat(p->workBuffer, "<HeaderData>");

  j = (int32_t)strlen(p->workBuffer);
  while(iLen < length)
  {
    len = 0;
    for(i = 0; i < 3; i++)
    {
      if(iLen < length)
      {
        in[i] = ptr[iLen];
        len++;
      }
      else
        in[i] = 0;
      iLen++;
    }

    if(len)
    {
      encode_block(in, out, len);
      for(i = 0; i < 4; i++)
        p->workBuffer[j++] = out[i];
      blocksout++;
    }
  }

  p->workBuffer[j] = 0;
  strcat(p->workBuffer, "</HeaderData>\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&p->workBuffer[0], (uint32_t)strlen(p->workBuffer));
}

#else

static void ascii_encode(bufstream_tt *bs, uint8_t *ptr, int32_t length, int32_t indent)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  int32_t i, j;
  static const char hexChars[] = "0123456789ABCDEF";
  uint32_t encoded_len = length * 2 + 64;

  if(encoded_len > p->workBufferSize)
  {
    if(p->workBufferSize)
      free(p->workBuffer);
    p->workBuffer = (uint8_t*)malloc(encoded_len);
    p->workBufferSize = encoded_len;
  }

  for(i = 0; i < indent; i++)
    p->workBuffer[i] = ' ';
  p->workBuffer[i] = 0;
  strcat(p->workBuffer, "<HeaderData>\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&p->workBuffer[0], strlen(p->workBuffer));

  for(i = 0; i <= indent; i++)
    p->workBuffer[i] = ' ';
  p->workBuffer[i] = 0;

  j = i;

  for(i = 0; i < length; i++)
  {
    p->workBuffer[j++] = hexChars[ptr[i] >> 4];
    p->workBuffer[j++] = hexChars[ptr[i] & 0x0F];
  }
  p->workBuffer[j] = 0;
  strcat(p->workBuffer, "\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&p->workBuffer[0], strlen(p->workBuffer));

  for(i = 0; i < indent; i++)
    p->workBuffer[i] = ' ';
  p->workBuffer[i] = 0;
  strcat(p->workBuffer, "</HeaderData>\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&p->workBuffer[0], strlen(p->workBuffer));
}

#endif


static uint32_t idx_usable_bytes(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->usable_bytes (bs->Buf_IO_struct->main_bs);
}

static uint8_t *idx_request(bufstream_tt *bs, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  p->picBuffer = p->main_bs->request (p->main_bs, numbytes);
  return p->picBuffer;
}

static int32_t save_header_bytes(struct impl_stream* p, uint8_t *ptr, int32_t numbytes)
{
    if(p->curGOPHeader.len + numbytes > p->curGOPHeader.size)
    {
      uint8_t *pPtr = (uint8_t*)malloc(p->curGOPHeader.len + numbytes);
      if (!pPtr)
          return 1;
      if (p->curGOPHeader.data && (p->curGOPHeader.len > 0))
        memcpy(pPtr, p->curGOPHeader.data, p->curGOPHeader.len);
      if(p->curGOPHeader.data)
        free(p->curGOPHeader.data);
      p->curGOPHeader.data = pPtr;
      p->curGOPHeader.size = p->curGOPHeader.len + numbytes;
    }

    memcpy(p->curGOPHeader.data + p->curGOPHeader.len, ptr, numbytes);
    if (p->curGOPHeader.len == 0)
      p->curGOPPos = p->byteCount;
    p->curGOPHeader.len += numbytes;
    return 0;
}

static uint32_t idx_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  uint8_t *ptr = bs->Buf_IO_struct->picBuffer;
  int32_t leftnumbytes = numbytes;

  if(!ptr || !numbytes)
    return 0;

  if (p->bPicStartFlag)
  {
    if (!p->bPicGotHdrFlag)
    {
      int32_t i = 0;

      while((i < leftnumbytes-3) &&
            (BS_INT32(*(uint32_t*)&(ptr[i])) != 0x00000100))
        i++;

      if (i > 0)
      {
        if (save_header_bytes(p, ptr, i))
          return 0;

        // set ptr to point to the picture header
        ptr += i;
        leftnumbytes -= i;
        i = 0;
      }

      while((i + 3 < leftnumbytes) &&
            (((BS_INT32(*(uint32_t*)&(ptr[i])) & 0xFFFFFF00) != 0x00000100) ||
             (ptr[i + 3] < 0x01) || (ptr[i + 3] > 0xAF)))
        i++;

      if(i > 0)
      {
        // found the picture headers, save them to the curPICHeader
        if (i + 3 == leftnumbytes)
          i = leftnumbytes;    // no slice header was found so just use to the end of the buffer

        if(i > p->curPICHeader.size)
        {
          if(p->curPICHeader.size)
            free(p->curPICHeader.data);
          p->curPICHeader.data = (uint8_t*)malloc(i);
          p->curPICHeader.size = i;
        }

        memcpy(p->curPICHeader.data, ptr, i);
        p->curPICHeader.len = i;
        p->curPICPos = p->byteCount;
      }

      p->bPicGotHdrFlag = 1;
    }
  }
  else
  {
    if (save_header_bytes(p, ptr, numbytes))
      return 0;
  }

  p->byteCount += numbytes;

  return p->main_bs->confirm (p->main_bs, numbytes);
}

static uint32_t idx_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->copybytes (bs->Buf_IO_struct->main_bs, ptr, numbytes);
}

static uint32_t idx_split (bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->split (bs->Buf_IO_struct->main_bs);
}

static uint32_t idx_chunksize(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->chunksize (bs->Buf_IO_struct->main_bs);
}

static int32_t write_vau_info(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  char tmpStr[1024];
  char numStr[32];
  int32_t i;

  if(!p->initOutput)
  {
    strcpy(tmpStr, "<Stream>\n");
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
    p->initOutput = 1;
  }

  strcpy(tmpStr, " <AccessUnit>\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

  if(p->aunit.unitType & SEQHDR_FLAG)
    strcpy(tmpStr, "  <UnitType>SEQ</UnitType>\n");
  else
    strcpy(tmpStr, "  <UnitType>GOP</UnitType>\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

  if(bs->flags == 0)
  {
    sprintf_int64(numStr, p->aunit.gopPosition);
    sprintf(tmpStr, "  <Position>%s</Position>\n", numStr);
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf_int64(numStr, p->aunit.firstPTS / 300);
    sprintf(tmpStr, "  <PTS>%s</PTS>\n", numStr);
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    p->aunit.gopDuration = p->aunit.lastPTS - p->aunit.firstPTS + p->clocks_per_frame;
    if(p->aunit.rffFlag)
      p->aunit.gopDuration += p->clocks_per_field;

    p->duration += p->aunit.gopDuration;
    sprintf_int64(numStr, p->aunit.gopDuration / 300);
    sprintf(tmpStr, "  <Duration>%s</Duration>\n", numStr);
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
  }

  sprintf(tmpStr, "  <Size>%d</Size>\n", p->aunit.gopSize);
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

  if(bs->flags == 0)
  {
#ifdef DO_BASE64
    b64_encode(bs, &p->aunit.headerData.data[0], p->aunit.headerData.len, 2);
#else
    ascii_encode(bs, &p->aunit.headerData.data[0], p->aunit.headerData.len, 2);
#endif

    for(i = 0; i < MAXN; i++)
    {
      if(p->aunit.frames[i].present)
      {
        strcpy(tmpStr, "  <PictureData>\n");
        p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

#ifdef DO_BASE64
        b64_encode(bs, &p->aunit.frames[i].headerData.data[0], p->aunit.frames[i].headerData.len, 3);
#else
        ascii_encode(bs, &p->aunit.frames[i].headerData.data[0], p->aunit.frames[i].headerData.len, 3);
#endif

        sprintf(tmpStr, "   <Offset>%d</Offset>\n", (int32_t)p->aunit.frames[i].relPosition);
        p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

        strcpy(tmpStr, "  </PictureData>\n");
        p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
      }
    }
  }

  strcpy(tmpStr, " </AccessUnit>\n");
  p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

  return BS_OK;
}



static uint32_t idx_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  switch(info_ID)
  {
    case FLUSH_BUFFER:
      if(p->video_bs)
        p->video_bs->auxinfo(p->video_bs, 0, info_ID, NULL, 0);
      break;

    case ID_PICTURE_START_CODE:
      {
        struct pic_start_info *pph = (struct pic_start_info*)info_ptr;

        p->curRFFFlag = pph->repeat_first_field;
        p->bPicStartFlag = 1;
      }
      break;

    case VIDEO_AU_CODE:
      if(p->video_bs)
      {
        struct v_au_struct *pau = (struct v_au_struct*)info_ptr;
        int32_t i, j;

        if((pau->flags & SEQHDR_FLAG) || (pau->flags & GOPHDR_FLAG))
        {
          // send out previous GOP info
          if(p->aunit.firstPTS >= 0)
            write_vau_info(bs);

          // clear out all the existing picture present flags
          for(i = 0; i < MAXN; i++)
            p->aunit.frames[i].present = 0;

          p->aunit.unitType = pau->flags;
          p->aunit.gopPosition = p->curGOPPos;
          p->aunit.gopSize = 0;
          p->aunit.firstPTS = -1;

          i = p->curGOPHeader.len;
          if(i > p->aunit.headerData.size)
          {
            if(p->aunit.headerData.size)
              free(p->aunit.headerData.data);
            p->aunit.headerData.data = (uint8_t*)malloc(i);
            p->aunit.headerData.size = i;
          }

          memcpy(p->aunit.headerData.data, p->curGOPHeader.data, i);
          p->aunit.headerData.len = i;
          p->curGOPHeader.len = 0;
        }

        if ((p->curPICHeader.data != NULL) && (p->curPICHeader.len > 6))
        {
          // get the temp ref of the current picture
          i = ((p->curPICHeader.data[4] << 8) | p->curPICHeader.data[5]) >> 6;
          if(i < MAXN)
          {
            // we got a valid picture
            p->aunit.frames[i].present = 1;
            if((p->aunit.firstPTS < 0) ||
               (pau->PTS < p->aunit.firstPTS))
              p->aunit.firstPTS = pau->PTS;

            if(pau->PTS > p->aunit.lastPTS)
            {
              p->aunit.lastPTS = pau->PTS;
              p->aunit.rffFlag = p->curRFFFlag;
            }

            p->aunit.frames[i].relPosition = p->curPICPos - p->aunit.gopPosition;

            j = p->curPICHeader.len;
            if(j > p->aunit.frames[i].headerData.size)
            {
              if(p->aunit.frames[i].headerData.size)
                free(p->aunit.frames[i].headerData.data);
              p->aunit.frames[i].headerData.data = (uint8_t*)malloc(j);
              p->aunit.frames[i].headerData.size = j;
            }
            memcpy(p->aunit.frames[i].headerData.data, p->curPICHeader.data, j);
            p->aunit.frames[i].headerData.len = j;

            p->aunit.gopSize += pau->length;
          }
        }
        p->bPicStartFlag = 0;
        p->bPicGotHdrFlag = 0;
      }
      break;

    case ID_SEQ_START_CODE:
      if(!p->clocks_per_frame)
      {
        struct seq_start_info *psh = (struct seq_start_info*)info_ptr;

        if((psh->pulldown_flag == PULLDOWN_32) ||
           (psh->pulldown_flag == PULLDOWN_23))
          p->clocks_per_frame = (int64_t)picture_rates[FRAMERATE29];
        else
          p->clocks_per_frame = (int64_t)picture_rates[psh->frame_rate_code];
        p->clocks_per_field = p->clocks_per_frame / 2;
      }
      p->bPicStartFlag = 0;
      p->bPicGotHdrFlag = 0;
      break;

    case ID_GOP_START_CODE:
      p->bPicStartFlag = 0;
      p->bPicGotHdrFlag = 0;
      break;

    case SESSION_USER_DATA:
      if(p->video_bs && (info_size == sizeof(struct tag_session_user_data_tt)))
      {
        struct tag_session_user_data_tt *ud = (struct tag_session_user_data_tt*)info_ptr;
        char tmpStr[1024];

        if(!p->initOutput)
        {
          strcpy(tmpStr, "<Stream>\n");
          p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
          p->initOutput = 1;
        }

        strcpy(tmpStr, "<UserData>");
        p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
        p->video_bs->copybytes(p->video_bs, ud->udata, ud->udata_len);
        strcpy(tmpStr, "</UserData>\n");
        p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
      }
      break;
  }
  // pass on everything to main_bs
  if(p->main_bs)
    p->main_bs->auxinfo(p->main_bs, offs, info_ID, info_ptr, info_size);
  return BS_OK;
}


static void idx_done(bufstream_tt *bs, int32_t Abort)
{
  int32_t i;
  struct impl_stream* p = bs->Buf_IO_struct;
  char tmpStr[256];
  char numStr[32];

  if(p->video_bs)
  {
    if(!Abort)
    {
      if(p->aunit.firstPTS >= 0)
        write_vau_info(bs);
    }
    sprintf_int64(numStr, p->duration / 300);
    sprintf(tmpStr, " <StreamDuration>%s</StreamDuration>\n", numStr);
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf_int64(numStr, p->byteCount);
    sprintf(tmpStr, " <StreamSize>%s</StreamSize>\n", numStr);
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    strcpy(tmpStr, "</Stream>\n");
    p->video_bs->copybytes(p->video_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
  }

  for(i = 0; i < MAXN; i++)
  {
    if(p->aunit.frames[i].headerData.size)
      free(p->aunit.frames[i].headerData.data);
  }

  if(p->aunit.headerData.size)
    free(p->aunit.headerData.data);

  if(p->curGOPHeader.size)
    free(p->curGOPHeader.data);

  if(p->curPICHeader.size)
    free(p->curPICHeader.data);

  if(p->workBuffer)
    free(p->workBuffer);

  free(p);
  bs->Buf_IO_struct = NULL;
}


static void idx_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


int32_t init_bufstream_write_with_vxml_index(
                                            bufstream_tt *bs,
                                            bufstream_tt *main_bs,
                                            bufstream_tt *video_bs,
                                            uint32_t flag,
                                            void (*DisplayError)(char *txt))
{
  if (DisplayError){};  // remove compile warning

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

  bs->flags        = flag;

  bs->Buf_IO_struct->main_bs = main_bs;


  if(video_bs)
  {
    bs->Buf_IO_struct->video_bs = video_bs;
    memset(&bs->Buf_IO_struct->aunit, 0, sizeof(struct access_unit));
    bs->Buf_IO_struct->aunit.firstPTS = -1;
  }

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_vxml_index(bufstream_tt *main_bs,
                                                   bufstream_tt *video_idx,
                                                   uint32_t flag,
                                                   void (*DisplayError)(char *txt))
{
  bufstream_tt *p;
  p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
  if(p)
  {
    if(BS_OK != init_bufstream_write_with_vxml_index (p, main_bs, video_idx, flag, DisplayError))
    {
      free(p);
      p = NULL;
    }
  }
  return p;
}



void close_bufstream_write_with_vxml_index(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

