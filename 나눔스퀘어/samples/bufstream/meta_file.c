/* ----------------------------------------------------------------------------
 * File: meta_file.c
 *
 * Desc: Stream Buffer implementation for file output. 
 *       Buffers data and proccesses file access asynchronously.
 *       Buffers seek/write requests until the bufstream is closed (implements same functionality as buf_seek).
 *
 *       Use where multiple files are to written in parallel to avoid performance degradation because of 
 *       concurent disk access, e.g P2 mxf multiplexing or multiple encoding sessions
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (defined(__APPLE__) ||  defined(__linux__))
#include <unistd.h>
#endif
#include "meta_file.h"

#define MAX_METADATA_NUM (12)

#ifndef LOCAL
#define LOCAL static
#endif

struct impl_stream
{
  bufstream_tt *filtered;
  uint32_t metadata_state[MAX_METADATA_NUM];
  FILE *metadata_file[MAX_METADATA_NUM];
#ifdef _BS_UNICODE
  wchar_t base_filename[_BS_MAX_PATH];
#else
  char base_filename[_BS_MAX_PATH];
#endif
  int32_t destroy_when_closed;
};


LOCAL uint32_t auxinfo_metadata_file(metadata_storage_tt *md, 
                      uint32_t stream_nr, uint32_t info_ID, 
                      void *info_ptr, uint32_t info_size)
{
#ifdef _BS_UNICODE
  wchar_t filename[_BS_MAX_PATH + 4];
#else
  char filename[_BS_MAX_PATH + 4];
#endif
  struct impl_stream *instance;

  instance = md->Buf_IO_struct;

  if(stream_nr >= MAX_METADATA_NUM)
    return ((uint32_t)(~0));

  switch(info_ID)
  {
    case METADATA_OPEN_WRITE:

      if(instance->metadata_file[stream_nr] == NULL)
      {
#ifdef _BS_UNICODE
        swprintf(filename,_BS_MAX_PATH + 4, L"%s.md%d", instance->base_filename, (int32_t)stream_nr);
#else
        sprintf(filename,"%s.md%d", instance->base_filename, (int32_t)stream_nr);
#endif

#ifdef _BS_UNICODE
        instance->metadata_file[stream_nr] = _wfopen(filename,L"wb");
#else
#ifndef __QNX__
        instance->metadata_file[stream_nr] = fopen(filename,"wb");
#else
        instance->metadata_file[stream_nr] = fopen64(filename,"wb");
#endif
#endif

        if( instance->metadata_file[stream_nr] == NULL)
          return ((uint32_t)(~0));

        instance->metadata_state[stream_nr]= 0;
      }
      else
        return ((uint32_t)(~0));

      break;
      
    case METADATA_OPEN_READ:

      if(instance->metadata_file[stream_nr] == NULL)
      {
#ifdef _BS_UNICODE
        swprintf(filename,_BS_MAX_PATH + 4, L"%s.md%d", instance->base_filename, (int32_t)stream_nr);
#else
        sprintf(filename,"%s.md%d", instance->base_filename, (int32_t)stream_nr);
#endif

#ifdef _BS_UNICODE
        instance->metadata_file[stream_nr] = _wfopen(filename,L"rb");
#else
#ifndef __QNX__
        instance->metadata_file[stream_nr] = fopen(filename,"rb");
#else
        instance->metadata_file[stream_nr] = fopen64(filename,"rb");
#endif
#endif
        if( instance->metadata_file[stream_nr] == NULL)
          return ((uint32_t)(~0));

        instance->metadata_state[stream_nr]= 0;
      }
      else
        return ((uint32_t)(~0));

      break;

    case METADATA_CLOSE:

      if(instance->metadata_file[stream_nr])
      {
        fclose(instance->metadata_file[stream_nr]);
        instance->metadata_file[stream_nr] = NULL;
        instance->metadata_state[stream_nr]= 0;
      }
      break;

    case METADATA_WRITE:

      if(instance->metadata_file[stream_nr])
      {
        size_t n = fwrite(info_ptr, info_size, 1, instance->metadata_file[stream_nr]);

        if( n != 1)
          return ((uint32_t)(~0));
      }
      else
        return ((uint32_t)(~0));

      break;

    case METADATA_READ:

      if(instance->metadata_file[stream_nr])
      {
        size_t n = fread(info_ptr, info_size, 1, instance->metadata_file[stream_nr]);

        if( n != 1)
          return ((uint32_t)(~0));
      }
      else
        return ((uint32_t)(~0));

      break;

    case METADATA_REWIND:

      if(instance->metadata_file[stream_nr])
      {
        rewind(instance->metadata_file[stream_nr]);
      }
      else
        return ((uint32_t)(~0));

      break;

    case METADATA_GET_SIZE:

      if(instance->metadata_file[stream_nr] && info_ptr)
      {
        // store current position
        int32_t pos = ftell(instance->metadata_file[stream_nr]);

        fseek(instance->metadata_file[stream_nr], 0, SEEK_END);
        *((uint32_t *)info_ptr) = (uint32_t)ftell(instance->metadata_file[stream_nr]);

        fseek(instance->metadata_file[stream_nr], pos, SEEK_SET);
      }
      else
        return ((uint32_t)(~0));

      break;

    case METADATA_DESTROY:

      if(instance->metadata_file[stream_nr] == NULL)
      {
#if (!defined(__APPLE__) && !defined(__linux__))
        FILE * fil;
#endif
          
#ifdef _BS_UNICODE
        swprintf(filename,_BS_MAX_PATH + 4, L"%s.md%d", instance->base_filename, (int32_t)stream_nr);
#else
        sprintf(filename,"%s.md%d", instance->base_filename, (int32_t)stream_nr);
#endif

#if (defined(__APPLE__) ||  defined(__linux__))
        unlink(filename);
#else
#ifdef _BS_UNICODE
        fil = _wfopen(filename,L"wD");
#else
#ifndef __linux__
        fil = fopen(filename,"wD");
#else
        fil = fopen64(filename,"wD");
#endif
#endif

        if(fil)
          fclose(fil);
#endif          
		break;
      }
      else
        return ((uint32_t)(~0));

    case METADATA_DESTROY_ALL:
      instance->destroy_when_closed = 1;
	  break;

    default: 
      if(instance->filtered)
      {
        return instance->filtered->auxinfo(instance->filtered, stream_nr, info_ID, info_ptr, info_size);
      }
      else
        return ((uint32_t)(~0));
  }
  return (0);
}

  
LOCAL void done_metadata_file(metadata_storage_tt *md, int32_t abort)
{
  int32_t i;
  struct impl_stream *instance;

  instance = md->Buf_IO_struct;

//we don't cache anything in metadata-instance. nothing to do with "abort"-parameter
  for(i = 0; i<MAX_METADATA_NUM; i++)
  {
    if(instance->metadata_file[i])
    {
      md->auxinfo(md, i, METADATA_CLOSE, NULL, 0);
    }
	if(instance->destroy_when_closed)
	{
      md->auxinfo(md, i, METADATA_DESTROY, NULL, 0);
	}
  }

  if(instance->filtered)
  {
    instance->filtered->done(instance->filtered, abort);
  }
}


LOCAL void free_metadata_file(metadata_storage_tt *md)
{
  int32_t i;
  struct impl_stream *instance;

  instance = md->Buf_IO_struct;

  for(i = 0; i<MAX_METADATA_NUM; i++)
  {
    if(instance->metadata_file[i])
    {
      md->auxinfo(md, i, METADATA_CLOSE, NULL, 0);
    }
	if(instance->destroy_when_closed)
	{
      md->auxinfo(md, i, METADATA_DESTROY, NULL, 0);
	}
  }

  if(instance->filtered)
  {
    instance->filtered->free(instance->filtered);
  }

  free(md);
}


#ifdef _BS_UNICODE
metadata_storage_tt* new_metadata_file(wchar_t *name)
#else
metadata_storage_tt* new_metadata_file(char *name)
#endif
{
  struct metadata_storage_impl
  {
    metadata_storage_tt metadata;
    struct impl_stream metadata_storage;
  } *instance;
  
#ifdef _BS_UNICODE
  if(name && wcslen(name) && (wcslen(name) < _BS_MAX_PATH))
#else
  if(name && strlen(name) && (strlen(name) < _BS_MAX_PATH))
#endif
  {
    instance = (struct metadata_storage_impl *) malloc(sizeof(struct metadata_storage_impl));

    if(instance)
    {
      int32_t i;

      memset(instance, 0, sizeof(*instance));

      instance->metadata.done    = done_metadata_file;
      instance->metadata.free    = free_metadata_file;
      instance->metadata.auxinfo = auxinfo_metadata_file;
      instance->metadata.Buf_IO_struct = &instance->metadata_storage;
  
      for(i = 0; i<MAX_METADATA_NUM; i++)
      {
        instance->metadata_storage.metadata_file[i] = NULL;
        instance->metadata_storage.metadata_state[i] = 0;
      }
#ifdef _BS_UNICODE
      wcscpy(instance->metadata_storage.base_filename, name);
#else
      strcpy(instance->metadata_storage.base_filename, name);
#endif
      return &instance->metadata;
    }
  }

  return NULL;
}



LOCAL uint32_t filter_drive(bufstream_tt *bs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
  metadata_storage_tt *md;
  
  md = (metadata_storage_tt *)bs->drive_ptr;
  if(md->drive)
    return md->drive((bufstream_tt *)md, info_ID, info_ptr, info_size);
  else
    return 0;
}


LOCAL uint32_t filt_usable_bytes (bufstream_tt *bs)
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->usable_bytes(fbs);
}


LOCAL uint8_t *filt_request (bufstream_tt *bs, uint32_t numbytes)
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->request(fbs, numbytes);
}


LOCAL uint32_t filt_confirm (bufstream_tt *bs, uint32_t numbytes)
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->confirm(fbs, numbytes);
}


LOCAL uint32_t filt_copybytes (bufstream_tt *bs, uint8_t *ptr, uint32_t numSamples)
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->copybytes(fbs, ptr, numSamples);
}


LOCAL uint32_t filt_chunksize (bufstream_tt *bs)
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->chunksize(fbs);
}


LOCAL uint32_t filt_split (bufstream_tt *bs)
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->split(fbs);
}


LOCAL uint32_t filt_state (bufstream_tt *bs) //currently unused
{
  bufstream_tt *fbs = bs->Buf_IO_struct->filtered;
  return fbs->state(fbs);
}




bufstream_tt* init_metadata_filter(metadata_storage_tt *md, bufstream_tt *bs)
{
  struct impl_stream *instance;

  instance = md->Buf_IO_struct;

  instance->filtered = bs;

  md->usable_bytes = filt_usable_bytes;
  md->request      = filt_request;
  md->confirm      = filt_confirm;
  md->copybytes    = filt_copybytes;
  md->chunksize    = filt_chunksize;
  md->split        = filt_split;

  md->drive_ptr    = instance->filtered->drive_ptr;
  md->drive        = instance->filtered->drive;
  instance->filtered->drive_ptr    = (struct drive_struct *)md;
  instance->filtered->drive        = filter_drive;

  md->flags = instance->filtered->flags;
  md->state = filt_state;
 
  return (bufstream_tt*) md;
}

