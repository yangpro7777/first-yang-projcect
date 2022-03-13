 /**
@file: bufstrm.h
@brief Stream base class (used by encoders, decoders and muxers)

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/

#if !defined (__BUFSTREAM_API_INCLUDED__)
#define __BUFSTREAM_API_INCLUDED__

#include "mctypes.h"

// comment this out to use non-UNICODE functions
// in a UNICODE project. Meant for Windows platforms
// if commented out, leave _BS_MAX_PATH defined as 2048
#ifdef _UNICODE
#define _BS_UNICODE
#define _BS_MAX_PATH  32768
#else
#define _BS_MAX_PATH  2048
#endif


#define BS_OK       (0)                 /**< Success return code @hideinitializer*/
#define BS_IO_ERROR (1)                 /**< I/O error return code @hideinitializer*/
#define BS_ERROR    (-1)                /**< General error return code @hideinitializer*/


#define BS_FLAGS_DST 0x00000001         /**< Owns AuxInfo channel, currently unused @hideinitializer*/
#define BS_FLAGS_ACT 0x00000002         /**< Currently unused @hideinitializer*/

/** 
 @brief Represents the bufstream object
*/
typedef struct bufstream bufstream_tt;

/**
 @brief Used to send data structures and information to a component or bufstream object
 
 For example a video encoder could send video access unit information to an attached muxer. Some messages are
 used in bufstream objects as well.
 
 @param[in] bs             - Pointer to bufstream object
 @param[in] offs           - Values depends on info_ID message
 @param[in] info_ID        - Auxinfo message ID, some ID's are defined in auxinfo.h (aux_ID enum), some are defined in component headers
 @param[in] info_ptr       - Pointer to passed data, data depends on info_ID message
 @param[in] info_size      - Size of passed data, data size depends on info_ID message
 @return BS_OK if successful else an error code
*/
typedef uint32_t(MC_EXPORT_API * auxinfo_t) (bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size);

/**
 @brief Put (or get) numbytes bytes into (or from) from bufsteam object.
 
 @param[in] bs                   - Pointer to bufstream object
 @param[in] ptr                  - Pointer to source buffer
 @param[in] numSamples           - Number of samples
 @return Number of bytes copied
*/
typedef uint32_t(MC_EXPORT_API * copybytes_t) (bufstream_tt *bs, uint8_t *ptr, uint32_t numSamples);

/**
 @brief Call to close a bufstream object.
 
 Recommended way to close a bufstream object is to use the close_bufstream() macro below.

 @param[in] bs             - Pointer to bufstream object
 @param[in] Abort          - Set to 1 to just close bufstream without any pending processing
*/
typedef void (MC_EXPORT_API * done_t) (bufstream_tt *bs, int32_t Abort);

/**
 @brief Reports how many bytes can be put into or read from buffer.

 Mainly used if a call to request(chunk size or less) fails to see how many bytes remain in the bufstream object.

 @param[in] bs             - Pointer to bufstream object
 @return Number of usable bytes
*/
typedef uint32_t(MC_EXPORT_API * usable_bytes_t) (bufstream_tt *bs);

/**
 @brief Requests a buffer with at least numbytes available bytes.

 The number of bytes requested should be <= the bufstream object chunk size. This call may fail if the number of
 bytes requested is larger than the chunk size. If the number of bytes requested is <= the chunk size the call
 is guaranteed to succeed unless the buffer does not have chunk size space or bytes available.
 Once a number of bytes (can be less than requested) are put into or read from the requested buffer, they can be
 finalized using the confirm() call.

 @param[in] bs                   - Pointer to bufstream object
 @param[in] numbytes             - The number of bytes requested
 @return Pointer to the buffer or NULL if request fails.
*/
typedef uint8_t* (MC_EXPORT_API * request_t) (bufstream_tt *bs, uint32_t numbytes);

/**
 @brief Informs the bufstream object of bytes put into (or read from) the requested buffer.
 
 Confirms that numbytes bytes were put into (or read from) the buffer returned by the last request() call.

 @param[in] bs                   - Pointer to bufstream object
 @param[in] numbytes             - The number of bytes put into (or read from) the buffer
 @return Number of bytes confirmed
*/
typedef uint32_t(MC_EXPORT_API * confirm_t) (bufstream_tt *bs, uint32_t numbytes);

/**
 @brief Gets the chunk-size of the bufstream object in buffer mode.
 
 Returns the chunk size used when the buffered bufstream object was created.

 @param[in] bs                   - Pointer to bufstream object
 @return Chunk size used to setup the bufstream object
*/
typedef uint32_t(MC_EXPORT_API * chunksize_t) (bufstream_tt *bs);

/**
 @brief Call to switch output to a new file.

 If available, currently only implemented in buf_file.c in output mode, else this field is NULL

 @param[in] bs             - Pointer to bufstream object
 @return BS_OK if successful else an error code
*/
typedef uint32_t(MC_EXPORT_API * split_t) (bufstream_tt *bs);

/**
 @brief Call to free a bufstream object.
 
 Recommended way to free a bufstream object is to use the close_bufstream() macro below.

 @param[in] bs             - Pointer to bufstream object
*/
typedef void (MC_EXPORT_API * bufstream_free_t) (bufstream_tt *bs);

struct bufstream
{
  usable_bytes_t   usable_bytes;             /**< @brief Reports how many bytes can be put into or read from buffer. See @ref usable_bytes_t for details. */
  request_t        request;                  /**< @brief @brief Requests a buffer with at least numbytes available bytes. See @ref request_t for details. */
  confirm_t        confirm;                  /**< @brief Informs the bufstream object of bytes put into (or read from) the requested buffer. See @ref confirm_t for details. */
  copybytes_t      copybytes;                /**< @brief Put (or get) numbytes bytes into (or from) from bufsteam object. See @ref copybytes_t for details. */
  chunksize_t      chunksize;                /**< @brief Gets the chunk-size of the bufstream object in buffer mode. See @ref chunksize_t for details. */
  auxinfo_t        auxinfo;                  /**< @brief Used to send data structures and information to a component or bufstream object. See @ref auxinfo_t for details. */
  split_t          split;                    /**< @brief Call to switch output to a new file. See @ref split_t for details. */
  done_t           done;                     /**< @brief Call to close a bufstream object. See @ref done_t for details. */
  bufstream_free_t free;                     /**< @brief Call to free a bufstream object. See @ref free_t for details. */

  struct drive_struct *drive_ptr;           /**< @brief Used by the auxinfo receiver to store information, a muxer might store a stream object pointer in this field */
  struct impl_stream* Buf_IO_struct;        /**< @brief Information storage pointer defined by each bufstream implementation */

/** @cond */
  uint32_t (MC_EXPORT_API * drive) (bufstream_tt *bs, uint32_t info_ID, void *info_ptr, uint32_t info_size);    /**< @brief Currently unused */
  uint32_t flags;                                                                                               /**< @brief Currently unused, set once during creation/initialization, but unused after */
  uint32_t (MC_EXPORT_API * state) (bufstream_tt *bs);                                                          /**< @brief Currently unused */
/** @endcond */
};

/**
 @brief Recommended way to close and free a bufstream object

 bs = NULL; is very important!
 It forces usage of close_bufstream()-macro on pointer-variables and produce Compiler-error
 on static/stack/part_of_struct allocated objects

 @param[in] bs             - Pointer to bufstream object
 @param[in] abort          - Set to 1 to just close bufstream without any pending processing
*/
#define close_bufstream(bs, abort) \
do { bs->done(bs, abort); bs->free(bs); bs = NULL; } while(0)


#endif // #if !defined (__BUFSTREAM_API_INCLUDED__)

