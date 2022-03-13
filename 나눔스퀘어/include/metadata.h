/* ----------------------------------------------------------------------------
 * File: metadata.h
 *
 * Desc: metadata storage class (used by multipass encoders)
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 *
 * ----------------------------------------------------------------------------
 */

#if !defined (__METADATA_API_INCLUDED__)
#define __METADATA_API_INCLUDED__

#include "bufstrm.h"

typedef struct metadata_storage metadata_storage_tt;

//structure-layout is the same as bufstream_struct.
//I want to be able to build metadata-storage as filter to
//bufstream-class auxinfo

struct metadata_storage
{
  uint32_t  (MC_EXPORT_API * usable_bytes)(bufstream_tt *bs);
  uint8_t * (MC_EXPORT_API * request)(bufstream_tt *bs, uint32_t numbytes);
  uint32_t  (MC_EXPORT_API * confirm)(bufstream_tt *bs, uint32_t numbytes);
  uint32_t  (MC_EXPORT_API * copybytes)(bufstream_tt *bs, uint8_t *ptr, uint32_t numSamples);
  uint32_t  (MC_EXPORT_API * chunksize)(bufstream_tt *bs);

  uint32_t  (MC_EXPORT_API * auxinfo)(metadata_storage_tt *instance, 
                                      uint32_t             stream_nr, 
                                      uint32_t             info_ID,
                                      void                *info_ptr, 
                                      uint32_t             info_size);

  uint32_t (MC_EXPORT_API * split) (bufstream_tt *bs);
  void     (MC_EXPORT_API * done)  (metadata_storage_tt *md, int32_t abort);
  void     (MC_EXPORT_API * free)  (metadata_storage_tt *md);

  struct drive_struct *drive_ptr;

  uint32_t (MC_EXPORT_API * drive) (bufstream_tt *bs, uint32_t info_ID, void *info_ptr, uint32_t info_size);

  uint32_t flags; //currently unused, set once on create/initialisation time

  uint32_t (MC_EXPORT_API * state) (bufstream_tt *bs); //currently unused

  struct impl_stream* Buf_IO_struct;
};

//choose unique names for METADATA-messages (to be able to build bufstream-filter)

#define METADATA_OPEN_WRITE  0x00100001
#define METADATA_OPEN_READ   0x00100002
#define METADATA_CLOSE       0x00100003
#define METADATA_WRITE       0x00100004
#define METADATA_READ        0x00100005
#define METADATA_REWIND      0x00100006
#define METADATA_DESTROY     0x00100007
#define METADATA_DESTROY_ALL 0x00100008
#define METADATA_GET_SIZE    0x00100009


#endif // #if !defined (__METADATA_API_INCLUDED__)

