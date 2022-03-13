/* ----------------------------------------------------------------------------
 * File: buf_xml.c
 *
 * Desc: Buffered stream splitter for XML index output of a program stream
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
#include "buf_xml.h"
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
  int32_t len;
  int32_t size;
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
  int64_t             firstVPTS;
  int64_t             lastVPTS;
  int64_t             firstAPTS;
  int64_t             lastAPTS;
  int64_t             gopDuration;
  int32_t             gopSize;
  int32_t             rffFlag;
  struct header       headerData;
  struct picture_data frames[MAXN];
};

//implementation structure
struct impl_stream
{
  bufstream_tt      *main_bs;        // main bufstream
  bufstream_tt      *v_xml_bs;       // video xml bufstream
  bufstream_tt      *a_xml_bs;       // audio xml bufstream
  int64_t            vByteCount;
  int64_t            aByteCount;
  int64_t            vDuration;
  int64_t            aDuration;
  struct header      curGOPHeader;
  struct header      curPICHeader;
  int64_t            curPICPos;
  int32_t            vInitOutput;
  int32_t            aInitOutput;
  int64_t            clocks_per_frame;
  int64_t            clocks_per_field;
  int64_t            clocks_per_audio_frame;
  int32_t            curRFFFlag;
  int32_t            vID;
  int32_t            aID;
  struct access_unit aunit;            // buffer for video au info
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
  char tmpStr[2048];

  for(i = 0; i < indent; i++)
    tmpStr[i] = ' ';
  tmpStr[i] = 0;
  strcat(tmpStr, "<HeaderData>");

  j = (int32_t)strlen(tmpStr);
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
        tmpStr[j++] = out[i];
      blocksout++;
    }
  }

  tmpStr[j] = 0;
  strcat(tmpStr, "</HeaderData>\n");
  p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
}

#else

static void ascii_encode(bufstream_tt *bs, uint8_t *ptr, int32_t length, int32_t indent)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  int32_t i, j;
  static const char hexChars[] = "0123456789ABCDEF";
  char tmpStr[1024];

  for(i = 0; i < indent; i++)
    tmpStr[i] = ' ';
  tmpStr[i] = 0;
  strcat(tmpStr, "<HeaderData>\n");
  p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], strlen(tmpStr));

  for(i = 0; i <= indent; i++)
    tmpStr[i] = ' ';
  tmpStr[i] = 0;

  j = i;

  for(i = 0; i < length; i++)
  {
    tmpStr[j++] = hexChars[ptr[i] >> 4];
    tmpStr[j++] = hexChars[ptr[i] & 0x0F];
  }
  tmpStr[j] = 0;
  strcat(tmpStr, "\n");
  p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], strlen(tmpStr));

  for(i = 0; i < indent; i++)
    tmpStr[i] = ' ';
  tmpStr[i] = 0;
  strcat(tmpStr, "</HeaderData>\n");
  p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], strlen(tmpStr));
}

#endif


static uint32_t idx_usable_bytes(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->usable_bytes(bs->Buf_IO_struct->main_bs);
}

static uint8_t *idx_request(bufstream_tt *bs, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->request(bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t idx_confirm(bufstream_tt *bs, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->confirm(bs->Buf_IO_struct->main_bs, numbytes);
}

static uint32_t idx_copybytes(bufstream_tt *bs, uint8_t *ptr, uint32_t numbytes)
{
  return bs->Buf_IO_struct->main_bs->copybytes(bs->Buf_IO_struct->main_bs, ptr, numbytes);
}

static uint32_t idx_split(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->split(bs->Buf_IO_struct->main_bs);
}

static uint32_t idx_chunksize(bufstream_tt *bs)
{
  return bs->Buf_IO_struct->main_bs->chunksize(bs->Buf_IO_struct->main_bs);
}

static int32_t write_vau_info(bufstream_tt *bs)
{
  struct impl_stream* p = bs->Buf_IO_struct;
  char tmpStr[1024];
  char numStr[32];
  int32_t i;
  int64_t duration;

  if(p->v_xml_bs)
  {
    if(!p->vInitOutput)
    {
      strcpy(tmpStr, "<Stream>\n");
      p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
      p->vInitOutput = 1;
    }

    strcpy(tmpStr, " <AccessUnit>\n");
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    if(p->aunit.unitType & SEQHDR_FLAG)
      strcpy(tmpStr, "  <UnitType>SEQ</UnitType>\n");
    else
      strcpy(tmpStr, "  <UnitType>GOP</UnitType>\n");
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    if(bs->flags == 0)
    {
      sprintf_int64(numStr, p->aunit.gopPosition);
      sprintf(tmpStr, "  <Position>%s</Position>\n", numStr);
      p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

      sprintf_int64(numStr, p->aunit.firstVPTS / 300);
      sprintf(tmpStr, "  <PTS>%s</PTS>\n", numStr);
      p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

      p->aunit.gopDuration = p->aunit.lastVPTS - p->aunit.firstVPTS + p->clocks_per_frame;
      if(p->aunit.rffFlag)
        p->aunit.gopDuration += p->clocks_per_field;

      p->vDuration += p->aunit.gopDuration;
      sprintf_int64(numStr, p->aunit.gopDuration / 300);
      sprintf(tmpStr, "  <Duration>%s</Duration>\n", numStr);
      p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
    }

    sprintf(tmpStr, "  <Size>%d</Size>\n", p->aunit.gopSize);
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

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
          p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

#ifdef DO_BASE64
          b64_encode(bs, &p->aunit.frames[i].headerData.data[0], p->aunit.frames[i].headerData.len, 3);
#else
          ascii_encode(bs, &p->aunit.frames[i].headerData.data[0], p->aunit.frames[i].headerData.len, 3);
#endif

          sprintf_int64(numStr, p->aunit.frames[i].relPosition);
          sprintf(tmpStr, "   <Offset>%s</Offset>\n", numStr);
          p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

          strcpy(tmpStr, "  </PictureData>\n");
          p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
        }
      }
    }

    strcpy(tmpStr, " </AccessUnit>\n");
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
  }

  // audio items
  if(p->a_xml_bs && p->aunit.firstAPTS >= 0)
  {
    if(!p->aInitOutput)
    {
      strcpy(tmpStr, "<Stream>\n");
      p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
      p->aInitOutput = 1;
    }

    strcpy(tmpStr, " <AudioUnit>\n");
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    if(p->aunit.unitType & SEQHDR_FLAG)
      strcpy(tmpStr, "  <UnitType>SEQ</UnitType>\n");
    else
      strcpy(tmpStr, "  <UnitType>GOP</UnitType>\n");
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf_int64(numStr, p->aunit.gopPosition);
    sprintf(tmpStr, "  <Position>%s</Position>\n", numStr);
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf_int64(numStr, p->aunit.firstAPTS / 300);
    sprintf(tmpStr, "  <PTS>%s</PTS>\n", numStr);
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    duration = p->aunit.lastAPTS - p->aunit.firstAPTS + p->clocks_per_audio_frame;
    p->aDuration += duration;

    sprintf_int64(numStr, duration / 300);
    sprintf(tmpStr, "  <Duration>%s</Duration>\n", numStr);
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    strcpy(tmpStr, " </AudioUnit>\n");
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
  }

  return BS_OK;
}


static uint32_t idx_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  struct impl_stream* p = bs->Buf_IO_struct;

  switch(info_ID)
  {
    case FLUSH_BUFFER:
      if(p->v_xml_bs)
        p->v_xml_bs->auxinfo(p->v_xml_bs, 0, info_ID, NULL, 0);
      if(p->a_xml_bs)
        p->a_xml_bs->auxinfo(p->a_xml_bs, 0, info_ID, NULL, 0);
      break;

    case VIDEO_STREAM_INFO:
      if(p->v_xml_bs && (info_size == sizeof(struct video_stream_info)))
      {
        struct video_stream_info *video_info = (struct video_stream_info*)info_ptr;
        p->vID = video_info->stream_id;
      }
      break;

    case VIDEO_AU_INFO_XML:
      if(p->v_xml_bs)
      {
        struct video_au_info_xml *pau = (struct video_au_info_xml*)info_ptr;
        int32_t i, j;
        uint8_t *ptr;
        int32_t pauhdrlength = pau->hdr_length;

        p->curRFFFlag = pau->repeat_first_field;
        ptr = &pau->hdr[0];
        if((pau->flags & SEQHDR_FLAG) || (pau->flags & GOPHDR_FLAG))
        {
          // send out previous GOP info
          if(p->aunit.firstVPTS >= 0)
            write_vau_info(bs);

          // clear out all the existing picture present flags
          for(i = 0; i < MAXN; i++)
            p->aunit.frames[i].present = 0;

          p->aunit.unitType = pau->flags;
          p->aunit.gopPosition = pau->filePos; // filePos really points to the gop position here
          p->aunit.gopSize = 0;
          p->aunit.firstVPTS = -1;
          p->aunit.firstAPTS = -1;

          p->curPICPos = 0;

          i = 0;
          while((i < pau->hdr_length-3) &&
                (BS_INT32(*(uint32_t*)&(ptr[i])) != 0x00000100))
            i++;

          if((i > 0) && (i < pauhdrlength-3))
          {
            // found some headers before the picture header, save them to curGOPHeader
            if(i > p->curGOPHeader.size)
            {
              if(p->curGOPHeader.size)
                free(p->curGOPHeader.data);
              p->curGOPHeader.data = (uint8_t*)malloc(i);
              p->curGOPHeader.size = i;
            }

            memcpy(p->curGOPHeader.data, pau->hdr, i);
            p->curGOPHeader.len = i;

            // set ptr to point to the picture header
            ptr += i;
            pauhdrlength -= i;
            p->curPICPos = i;
            i = 0;
          }

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
        }
        else
          p->curPICPos = pau->filePos;

        i = 0;
        while((i < pauhdrlength-3) &&
              (((BS_INT32(*(uint32_t*)&(ptr[i])) & 0xFFFFFF00) != 0x00000100) ||
               (ptr[i + 3] < 0x01) || (ptr[i + 3] > 0xAF)))
          i++;

        if((i > 0) && (i + 3 < pauhdrlength))
        {
          // found the picture headers, save them to the curPICHeader
          if(i > p->curPICHeader.size)
          {
            if(p->curPICHeader.size)
              free(p->curPICHeader.data);
            p->curPICHeader.data = (uint8_t*)malloc(i);
            p->curPICHeader.size = i;
          }

          memcpy(p->curPICHeader.data, ptr, i);
          p->curPICHeader.len = i;
        }

        if ((p->curPICHeader.data != NULL) && (p->curPICHeader.len > 6))
        {
          // get the temp ref of the current picture
          i = ((p->curPICHeader.data[4] << 8) | p->curPICHeader.data[5]) >> 6;
          if(i < MAXN)
          {
            // we got a valid picture
            p->aunit.frames[i].present = 1;
            if((p->aunit.firstVPTS < 0) ||
               (pau->PTS < p->aunit.firstVPTS))
              p->aunit.firstVPTS = pau->PTS;

            if(pau->PTS > p->aunit.lastVPTS)
            {
              p->aunit.lastVPTS = pau->PTS;
              p->aunit.rffFlag = p->curRFFFlag;
            }

            p->aunit.frames[i].relPosition = p->curPICPos; // - p->aunit.gopPosition;

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
        p->vByteCount += pau->length;
      }
      break;

    case VIDEO_SEQ_INFO_XML:
      if(!p->clocks_per_frame)
      {
        struct video_seq_info_xml *psh = (struct video_seq_info_xml*)info_ptr;

        if((psh->pulldown_flag == PULLDOWN_32) ||
           (psh->pulldown_flag == PULLDOWN_23))
          p->clocks_per_frame = (int64_t)picture_rates[FRAMERATE29];
        else
          p->clocks_per_frame = (int64_t)picture_rates[psh->frame_rate_code];
        p->clocks_per_field = p->clocks_per_frame / 2;
      }
      break;

    case AUDIO_STREAM_INFO:
      if(p->a_xml_bs && (info_size == sizeof(struct audio_stream_info)))
      {
        struct audio_stream_info *audio_info = (struct audio_stream_info*)info_ptr;
        p->aID = audio_info->stream_id;
      }
      break;

    case AUDIO_AU_INFO_XML:
      if(p->a_xml_bs)
      {
        struct audio_au_info_xml *pau = (struct audio_au_info_xml*)info_ptr;

        if(p->aunit.firstAPTS < 0)
          p->aunit.firstAPTS = pau->PTS;
        else
          p->aunit.lastAPTS = pau->PTS;
        p->aByteCount += pau->length;
      }
      break;

    case AUDIO_SEQ_INFO_XML:
      if(!p->clocks_per_audio_frame)
      {
        struct audio_seq_info_xml *psh = (struct audio_seq_info_xml*)info_ptr;
        p->clocks_per_audio_frame = psh->clocks_per_audio_frame;
      }
      break;

    case SESSION_USER_DATA:
      if(p->v_xml_bs && (info_size == sizeof(struct tag_session_user_data_tt)))
      {
        struct tag_session_user_data_tt *ud = (struct tag_session_user_data_tt*)info_ptr;
        char tmpStr[1024];

        if(!p->vInitOutput)
        {
          strcpy(tmpStr, "<Stream>\n");
          p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
          p->vInitOutput = 1;
        }

        strcpy(tmpStr, "<UserData>");
        p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
        p->v_xml_bs->copybytes(p->v_xml_bs, ud->udata, ud->udata_len);
        strcpy(tmpStr, "</UserData>\n");
        p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
      }
      break;

    case SESSION_USER_DATA_AUDIO:
      if(p->a_xml_bs && (info_size == sizeof(struct tag_session_user_data_tt)))
      {
        struct tag_session_user_data_tt *ud = (struct tag_session_user_data_tt*)info_ptr;
        char tmpStr[1024];

        if(!p->aInitOutput)
        {
          strcpy(tmpStr, "<Stream>\n");
          p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
          p->aInitOutput = 1;
        }

        strcpy(tmpStr, "<UserData>");
        p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
        p->a_xml_bs->copybytes(p->a_xml_bs, ud->udata, ud->udata_len);
        strcpy(tmpStr, "</UserData>\n");
        p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
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

  if(p->v_xml_bs)
  {
    if(!Abort)
    {
      if(p->aunit.firstVPTS >= 0)
        write_vau_info(bs);
    }

    sprintf_int64(numStr, p->vDuration / 300);
    sprintf(tmpStr, " <StreamDuration>%s</StreamDuration>\n", numStr);
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf_int64(numStr, p->vByteCount);
    sprintf(tmpStr, " <StreamSize>%s</StreamSize>\n", numStr);
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf(tmpStr, " <StreamId>%d</StreamId>\n", p->vID);
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    strcpy(tmpStr, "</Stream>\n");
    p->v_xml_bs->copybytes(p->v_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
  }

  if(p->a_xml_bs)
  {
    sprintf_int64(numStr, p->aDuration / 300);
    sprintf(tmpStr, " <StreamDuration>%s</StreamDuration>\n", numStr);
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf_int64(numStr, p->aByteCount);
    sprintf(tmpStr, " <StreamSize>%s</StreamSize>\n", numStr);
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    sprintf(tmpStr, " <StreamId>%d</StreamId>\n", p->aID);
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));

    strcpy(tmpStr, "</Stream>\n");
    p->a_xml_bs->copybytes(p->a_xml_bs, (uint8_t*)&tmpStr[0], (uint32_t)strlen(tmpStr));
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

  free(p);
  bs->Buf_IO_struct = NULL;
}


static void idx_free(bufstream_tt *bs)
{
  if(bs->Buf_IO_struct)
    bs->done(bs,0);

  free(bs);
}


int32_t init_bufstream_write_with_xml_index(bufstream_tt *bs,
                                            bufstream_tt *main_bs,
                                            bufstream_tt *v_xml_bs,
                                            bufstream_tt *a_xml_bs,
                                            uint32_t flag,
                                            void (*DisplayError)(char *txt))
{
  if (DisplayError){};  // remove compile warning

  if(!main_bs || (!v_xml_bs && !a_xml_bs))
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


  if(v_xml_bs)
    bs->Buf_IO_struct->v_xml_bs = v_xml_bs;

  if(a_xml_bs)
  {
    bs->Buf_IO_struct->a_xml_bs = a_xml_bs;
    memset(&bs->Buf_IO_struct->aunit, 0, sizeof(struct access_unit));
    bs->Buf_IO_struct->aunit.firstVPTS = -1;
    bs->Buf_IO_struct->aunit.firstAPTS = -1;
  }

  return BS_OK;
}


bufstream_tt *open_bufstream_write_with_xml_index(bufstream_tt *main_bs,
                                                  bufstream_tt *v_xml_bs,
                                                  bufstream_tt *a_xml_bs,
                                                  uint32_t flag,
                                                  void (*DisplayError)(char *txt))
{
  bufstream_tt *p;
  p=(bufstream_tt*)malloc(sizeof(bufstream_tt));
  if(p)
  {
    if(BS_OK != init_bufstream_write_with_xml_index (p, main_bs, v_xml_bs, a_xml_bs, flag, DisplayError))
    {
      free(p);
      p = NULL;
    }
  }
  return p;
}



void close_bufstream_write_with_xml_index(bufstream_tt* bs, int32_t Abort)
{
  bs->done(bs, Abort);
  bs->free(bs);
}

