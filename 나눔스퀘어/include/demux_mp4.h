/* ----------------------------------------------------------------------------
 * File: demux_mp4.h
 *
 * Desc: MP4 Demuxer API
 *
 * Copyright (c) 2014-2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

/*! @file /demux_mp4.h
 */
#if !defined (__DEMUX_MP4_API_INCLUDED__)
#define __DEMUX_MP4_API_INCLUDED__

#define __DEMUX_MP4_NEW__
#include <stddef.h>

#include "mctypes.h"
#include "common_demux.h"
#include "bufstrm.h"
#include "mcapiext.h"
#include "mcmediatypes.h"

/*! called to seek in the source file in splitter mode
*
* position - file position to seek to
* return 0 if OK, 1 if fault
*/
typedef int32_t (*mp4_splitter_seek_callback)(void* split_app_ptr, int64_t position);

/*! called to read data in splitter file mode
*
* read bufferSize bytes from file into buffer
* return number of bytes read, 0 if fault
*/
typedef int32_t (*mp4_splitter_read_callback)(void* split_app_ptr, uint8_t *buffer, int32_t bufferSize);


/*! external i/o function prototypes
*
* @note these are depreciated, the \ref mp4dmux_settings.p_external_io should be used instead
* @note if these are used, the \ref mpgdmux_settings.file_length field MUST be filled in
*       with the correct file size by the app
* called to open a file
* and the return value will be passed back to the seek, read and close functions
* ext_app_ptr - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileName - if a filename was passed to mpgdmux, it is returned here
* return the file handle as a long or -1 if an error occurs
*/
typedef long (*mp4_external_open_callback)(void *ext_app_ptr, char *fileName);
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
typedef long (*mp4_external_open_callbackW)(void *ext_app_ptr, wchar_t *fileName);
#else
typedef long (*mp4_external_open_callbackW)(void *ext_app_ptr, uint16_t *fileName);
#endif

/*! called to seek in a file
*
* ext_app_ptr - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileHandle - a file handle returned by the fn_external_open_callback function
* position - the position to seek to
* return 0 if Ok, 1 if an error occurs
*/
typedef int (*mp4_external_seek_callback)(void *ext_app_ptr, long fileHandle, int64_t position);

/*! called to read from a file
*
* appData - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileHandle - a file handle returned by the fn_external_open_callback function
* buffer - a buffer for the data
* bufferSize - the number of bytes to read
* return the number of bytes actually read or 0 if an error occurs
*/
typedef int (*mp4_external_read_callback)(void *ext_app_ptr, long fileHandle, uint8_t *buffer, int bufferSize);

/*! called to close a file
*
* appData - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileHandle - a file handle returned by the fn_external_open_callback function
*/
typedef void (*mp4_external_close_callback)(void *ext_app_ptr, long fileHandle);


//! @name MP4 Demuxer object
typedef struct mp4_demuxer mp4dmux_tt;	//!< demuxer object


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

//! @brief Used to define the \ref mp4dmux_tt object
//! @details The demultiplexer uses the mp4dmux_settings structure to hold its settings
struct mp4dmux_settings
{
//! @{
//! @brief Splitter mode parameters.
    int32_t splitter_mode;									//!< @brief Indicates that the demuxer is being used in splitter mode.
    mp4_splitter_seek_callback splitter_seek_callback;		//!< @brief Seek callback for splitter mode.
    mp4_splitter_read_callback splitter_read_callback;		//!< @brief Read callback for splitter mode.
    void *split_app_ptr;									//!< @brief App pointer for splitter mode.
//!@}

//! @{
//! @brief External IO parameters.
    int32_t use_external_io;								//!< @brief Indicates that the app will provide the I/O for the demuxer.
    mp4_external_open_callback external_open_callback;		//!< @brief External open callback pointer.
    mp4_external_open_callbackW external_open_callbackW;	//!< @brief UNICODE external open callback pointer.
    mp4_external_seek_callback external_seek_callback;		//!< @brief External seek callback pointer.
    mp4_external_read_callback external_read_callback;		//!< @brief External read callback pointer.
    mp4_external_close_callback external_close_callback;	//!< @brief External close callback pointer.
    mc_external_io_t *p_external_io;						//!< @brief If non NULL and use_external_io = 1, this will be used instead of the above
    void *ext_app_ptr;										//!< @brief App pointer for external I/O mode, it is passed to the external callbacks.
//!@}

    int64_t file_length;						//!< @brief Length of the input file, used for splitter and external I/O modes only.

    uint8_t stop_parsing_after_mdat;			//!< @brief This field tells the parser to stop parsing after it reaches the MDAT atom.
												/*!< @details This can be useful with external I/O to simulate streaming decoding.
												* If this is enabled, all of the other atoms needed to demux the stream must be present before the MDAT atom.
												* If this is not the case, the file will be rejected as an unknown file.
												*/

    int32_t initial_buffer_size;				//!< @brief Size of buffer for file detection
												/*!< @details This is equivalent to the mp4dmux_parser_settings.requested_buffer_size setting.
												* When a file is opened, the demuxer creates an internal parser to do the initial read/detection.
												* This field specifies the file buffer size the demuxer uses to do this initial read/detection.
												*/

    int32_t frame_index_length;                 //!< @brief Number of samples to parse in the stream to determine whether the stream is Fields per Sample or Frames per Sample
    int32_t read_cache_size;                    //!< @brief Defines read (IO) operation buffer size in bytes

    uint8_t detect_discontinuty;                //!<@brief Detect broken sequences and skip garbage in bitstream
    int32_t use_progress_mode;                  //!<@brief Enable progressive download mode. Should be use for fragmented mp4 with external io. If whole file is not available and is downloaded during playback. In this case demuxer will send OUTPUT_SEGMENT_INFO with  estimated duration currently available for user.
    uint8_t reserved[42];						//!< Reserved
};

//! @brief Used to define a parser object
struct mp4dmux_parser_settings
{
    int32_t requested_buffer_size;			//!< @brief Size in bytes the app wished the parser to use for bitstream buffers.

	uint8_t pull_mode;						//!< @brief Set to 1 if the app will call auxinfo to get data, otherwise the app will call mp4DemuxPush

    uint8_t add_adts_headers;				//!< @brief If this value equal 1, demultiplexer forming special headers before every AAC sample (ADTS header).

    uint8_t annexb_output;					//!< @brief If this value equal 1, demultiplexer replace sizes of H.264/AVC slices on special start code 0x0001.

    uint8_t use_field_rate;					//!< @brief If this value equal 1, demultiplexer used field rate for H.264/AVC time scale calculation.

    uint8_t detect_frames_types;			//!< @brief If this value equal 1, demultiplexer detect frame types (I,P,B etc.) for every video frame while parse file.

    uint8_t add_amr_header;					//!< @brief If this value equal 1, demultiplexer forming special headers before every AMR sample.

	uint8_t format_pcm_output_flag;			//!< @brief If this value is 1, the demuxer will output any PCM type streams as standard PCM format for the platform.

    uint8_t reserved[62];					//!< Reserved
};

//!@brief Protection System Information, matches PSSH (Protection System Specific Header Box) defined by Common Encryption Specification
typedef struct protection_system_info_s
{
    uint8_t system_id[16];                  //!< @brief specifies a UUID that uniquely identifies the content protection system that this header belongs to.

    uint32_t data_size;                     //!< @brief  specifies the size in bytes of the Data member.

    uint8_t * data;                         //!< @brief holds the content protection system specific data.

    uint8_t reserved[64];		            //!< Reserved

} protection_system_info_tt;

//!@brief describes single subsample of encrypted sample (if Subsample Encryption is used)
typedef struct sub_sample_encryption_info_s
{
    uint32_t bytes_of_clear_data;           //!< @brief the number of bytes of clear data in this subsample.

    uint32_t bytes_of_encrypted_data;       //!< @brief the number of bytes of encrypted data in this subsample.

    uint8_t reserved[64];		            //!< Reserved

} sub_sample_encryption_info_tt;

//!@brief Sample Encryption Information, matches SEIG (Sample Encryption Information) defined by Common Encryption Specification
typedef struct sample_encryption_info_s
{
    uint8_t is_encrypted;                        //!< @brief If this value equal 1, sample is encrypted, otherwise encryption isn't applied.

    uint8_t IV_size;                             //!< @brief Size of initialization vector, in bytes, either 8 or 16.

    uint8_t IV[16];                              //!< @brief Initialization vector.

    uint8_t KID[16];                             //!< @brief Is a key identifier that uniquely identifies the key needed to decrypt the associated samples. This allows the identification of multiple encryption keys per file or track.

    uint32_t sub_sample_count;                   //!< @brief the count of subsamples for this sample, zero if sub-sampling is not used (entire sample is encrypted).

    sub_sample_encryption_info_tt * sub_samples; //!< @brief array of subsamples for this samples, field is not used if sub_sample_count is zero.

    uint8_t reserved[64];		                 //!< Reserved

} sample_encryption_info_tt;

//! @brief Used to define the stream info
typedef struct mp4dmux_stream_format_s
{
    uint32_t ID;                            //!< @brief This value equal for internal track ID. Unique for every track.

    mc_stream_format_t format;              //!< @brief This field describes the stream in a standard mc_stream_format_t structure defined in mcmediatypes.h

    int32_t ES_info_length;                 //!< @brief Equal elementary stream decoder configuration size in bytes. This information is necessary for decoders.

    uint8_t *descriptors;					//!< @brief Array of bytes which contain decoder configuration.

    uint64_t sample_count;                  //!< @brief Number of video frames for a video stream

    uint32_t samples_per_frame;             //!< @brief Number of audio samples in 1 sample_count above

    double duration;                        //!< @brief The duration of the stream in seconds.

    int32_t raw_decoder_config_len;         //!< @brief The size in bytes of the raw_decoder_config field.

    uint8_t *raw_decoder_config;            //!< @brief The decoder config (if any) as present in the container for this stream.

    uint32_t track_width;                   //!< @brief Width indicated in the track header

	uint32_t track_height;                  //!< @brief Height indicated in the track header

    int64_t first_timestamp;                //!< @brief First timestamp in track

    uint32_t scheme_type;                   //!< @brief Encryption scheme type (e.g. 'cenc' for Common Encryption), zero if track is not encrypted

    uint16_t scheme_version_major;          //!< @brief Encryption scheme major version

    uint16_t scheme_version_minor;          //!< @brief Encryption scheme minor version

    uint32_t four_cc;                       //!< @brief Stream fourcc

    uint8_t reserved[28];                   //!< Reserved

} mp4dmux_stream_format_tt;

//! @brief Used to define the file info
struct mp4dmux_file_info
{
    int32_t stream_count;                   //!< @brief This value equal of total elementary streams count in file.

    double duration;                        //!< @brief Total file durations (in seconds). Equal duration of longest track.

    int32_t file_type;						//!< @brief This value equal ID for input stream type (MP4, 3GPP, QuickTime etc.).
											//!< @note At present moment, only one value available - STREAM_MP4.

    int64_t file_size;						//!< @brief Total input file size in bytes.

    int32_t file_info_length;               //!< @brief Reserved for future use.

    uint8_t *descriptors;					//!< @brief Reserved for future use.

    int32_t protection_system_info_count;   //!< @brief Number of PSSH entries associated with the given segment (\ref protection_system_info_tt data structures)

    uint8_t reserved[60];					//!< Reserved
};

//! @brief Used to hold the stream settings
struct mp4dmux_stream_settings
{
    int32_t stream_num;			//!< @brief Internal stream number which. Used in MC API.
    bufstream_tt *bs;			//!< @brief Pointer to a bufstream_tt instance for the streams output

    uint8_t reserved[64];		//!< Reserved
};

//! @brief Used to define the seek info
struct mp4dmux_seek_info
{
    int32_t parser_num;			//!< @brief Parser number for the seek.
    int32_t stream_num;			//!< @brief Stream number for the seek.
    int64_t seek_sample;		//!< @brief Sample number to get.

    int32_t ref_do;			//!< @brief Demuxing will start at this point after a seek if return_exact_sample = 0.
								/*!< @details For video streams this will be the reference frame display order where decoding must start to decode
								* the target frame. For audio streams, it will be the starting sample number of the compressed audio frame or uncompressed chunk.
								*/

        int32_t ref_co;			//!< @brief Demuxing will start at this point after a seek if return_exact_sample = 0.
								    /*!< For video streams this will be the reference frame coding order where decoding must start to decode the target frame.
								    * For audio streams, this will be the starting sample number of the compressed audio frame or uncompressed chunk.
								    */

        int64_t ref_PTS;			//!< @brief The reference frame timestamp (27MHz units).

        int32_t seek_do;			//!< @brief For video streams this will be the desired frame display order. For audio streams, this will be the desired sample number.
        int32_t seek_co;			//!< @brief For video streams this will be the desired frame coding order. For audio streams, this will be the desired sample number.
        int64_t seek_PTS;			//!< @brief The desired samples timestamp (27MHz units).

    uint8_t return_exact_sample;	//!< @brief Set this to have the demuxer position the seek point to the desired sample instead of the reference frame needed to decode the desired sample.

	//!< @brief These do not change the file position, the merely return the appropriate time:
    uint8_t seek_next_key_frame;                //!< @brief return the time of the next key frame based on seek_sample
    uint8_t seek_prev_key_frame;                //!< @brief return the time of the previous key frame based on seek_sample

    double seek_start_time;						//!< @brief Seek start time in seconds
    double seek_stop_time;						//!< @brief Seek stop time in seconds
    uint8_t reserved[45];						//!< Reserved
};

//! @brief Used to define the sample info in a sample map
struct demuxer_sample_info {
    /*!
    /--------/--------/--------/
    a--------b--------c--------d

    start = b;
    duration = c - b;
    start_cutoff =  b - a; // used for recostruct physical sample(actual sample duration based on real data amount(pcm)) start time
    stop_cutoff = d - c; // used for recostruct physical sample stop time
    */

    int64_t timestamp;			//!< @brief Sample timestamp (27MHz units).
    int64_t duration;           //!< @brief Sample duration (27MHz units).
    int64_t start_cutoff;       //!< @brief Start cutoff (27MHz units).
    int64_t stop_cutoff;        //!< @brief Stop cutoff (27MHz units).

    uint32_t size;              //!< @brief Sample size.

    bool preroll;               //!< @brief Sample preroll flag.
    bool sync;                  //!< @brief Sample sync flag.
};

#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif


//! call to create and initialize an demuxer instance
//!
//! @param[in]    callbacks		pointer to a structure with resource callbacks
//! @param[in]    set			pointer to an mp4dmux_settings structure
//!
//! @return a pointer to a demuxer instance if successful, else NULL

mp4dmux_tt * MC_EXPORT_API mp4DemuxNew(const callbacks_t * callbacks, void * reserved, struct mp4dmux_settings *set);


//! call to free a demuxer instance
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//!
//! @return    none

void MC_EXPORT_API mp4DemuxFree(mp4dmux_tt *demuxer);


//! call to open a file for demuxing
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    reserved		reserved
//! @param[in]    filename		pointer to a filename to open
//!
//! @return    0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxOpen(mp4dmux_tt *demuxer, void *reserved, char *filename);

//! Windows UTF-16 wchar_t
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
int32_t MC_EXPORT_API mp4DemuxOpenW(mp4dmux_tt *demuxer, void *reserved, wchar_t *filename);
#endif


//! call to close file
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//!
//! @return    none

void MC_EXPORT_API mp4DemuxClose(mp4dmux_tt *demuxer);


//! call to get information about the input file
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    file_info		pointer to a file_info_struct
//!
//! @return    0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxGetFileInfo(mp4dmux_tt *demuxer, struct mp4dmux_file_info *file_info);


//! call to get a description of a stream
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    stream_info	pointer to a mp4dmux_stream_format_tt
//! @param[in]    stream_num	zero-based stream number
//!
//! @return    0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxGetStreamFormat(mp4dmux_tt *demuxer, mp4dmux_stream_format_tt *stream_format, int32_t stream_num);


//! call to create and initialize a parser instance
//!
//! @param[in]    demuxer	pointer to a demuxer instance
//! @param[in]    set		pointer to a mp4dmux_parser_settings structure
//!
//! @return    a non-zero parser number if successful, else 0

int32_t MC_EXPORT_API mp4DemuxNewParser(mp4dmux_tt *demuxer, struct mp4dmux_parser_settings *set);


//! call to free a parser instance
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//!
//! @return   none

void MC_EXPORT_API mp4DemuxFreeParser(mp4dmux_tt *demuxer, int32_t parser_num);


//! call to add a stream to a parser instance
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//! @param[in]    set			pointer to an mp4dmux_stream_settings structure
//!
//! @return    0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxAddStream(mp4dmux_tt *demuxer, int32_t parser_num, struct mp4dmux_stream_settings *set);


//! call to remove an output bufstream
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//! @param[in]    set			pointer to an mp4dmux_stream_settings structure
//!
//! @return    0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxRemoveStream(mp4dmux_tt *demuxer, int32_t parser_num, struct mp4dmux_stream_settings *set);


//! call to seek to the specified time
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    info			pointer to a mp4dmux_seek_info structure
//!
//! @return    0 if successful, 2 if EOF, 1 if an error occurs

int32_t MC_EXPORT_API mp4DemuxSeek(mp4dmux_tt *demuxer, struct mp4dmux_seek_info* info);


//! call to push a packet through the parser
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//!
//! @return   \-1 if an error occurs, 0 if EOF else the number of bytes consumed from the source

int32_t MC_EXPORT_API mp4DemuxPush(mp4dmux_tt *demuxer, int32_t parser_num);


//! call to re-open a parser
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxWakeParser(mp4dmux_tt *demuxer, int32_t parser_num);


//! call to close the actual file of a parser, the parser is left intact and can be re-opened with the wake function
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxSleepParser(mp4dmux_tt *demuxer, int32_t parser_num);


//! call to have the file size updated if it is getting bigger
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//! @param[in]    file_size		the new file size if splitter or external i/o mode, else unused
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxUpdateFilesize(mp4dmux_tt *demuxer, int32_t parser_num, int64_t file_size);


//! call to have the parser perform a seek before the next read and optionally update the internal app_data ptr
//!
//! @param[in]    demuxer		pointer to a demuxer instance
//! @param[in]    parser_num	parser number
//! @param[in]    split_app_ptr	can be used to updated the split_app_ptr from the mp4dmux_settings structure
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxSetFirstSplitRead(mp4dmux_tt *demuxer, int32_t parser_num, void *split_app_ptr);


//! call to get extend API functions
//!
//! Currently supported values include:
//!   \li \ref MCAPI_GetParam
//!   \li \ref MCAPI_GetParamInfo
//!   \li \ref MCAPI_GetModuleInfo
//!
//! @param[in]   func	ID for function a pointer to get
//!
//! @return  function pointer or NULL

APIEXTFUNC MC_EXPORT_API mp4DemuxGetAPIExt(uint32_t func);

//! call to retrieve UV metadata
//!
//! @param[in]   demuxer	pointer to a demuxer instance
//! @param[in]   pBuff	buffer to the metadata; at the first call of the function, when API contributor doesn't know exact required data size this field should be NULL; at second call this pointer should be valid and allocated with the required size
//! @param[in]   iBuffSize	allocated buffer size; at the first call this argument should be 0; at the second call should be equal to the buffer allocation size
//! @param[in]   piRequiredBuffSize	this argument contains metadata size in bytes
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxGetUVMetaData(mp4dmux_tt *demuxer, int8_t *pBuff, uint32_t iBuffSize, uint32_t *piRequiredBuffSize);

//! @brief Used to carry metadata info
typedef struct {
    char        path[128];  //!< @brief [out] return location of metadata item: '' - root (or file), 'moov', 'moov/trak', ...
    char*       xml;        //!< @brief [in/out] buffer to the xml part of metadata. Set to 0 if you would not to use a XML part
    uint32_t    xml_size;   //!< @brief [in/out] required[out]/allocated[in] size for xml buffer.
    uint8_t*    blob;       //!< @brief [in/out] buffer to the whole 'meta/xml' Atom (for MP4) or whole 'meta' Atom (for MOV). Set to 0 if you would not to use.
    uint32_t    blob_size;  //!< @brief [in/out] required[out]/allocated[in] size for blob buffer.
} mp4dmux_metadata_info_tt;

//! call to retrieve metadata
//!
//! @param[in]      demuxer	    pointer to a demuxer instance
//! @param[in/out]  pMeta	    pointer to a metadata structure array of size nMetaCount
//! @param[in]      nMetaCount  size of metadata array
//!
//! @return   0 if successful, > 0 a required metadata array size, or < 0 in case of error

int32_t MC_EXPORT_API mp4DemuxGetMetaData(mp4dmux_tt *demuxer, mp4dmux_metadata_info_tt* pMeta, uint32_t nMetaCount);


//! call to retrieve info for a specific sample
//!
//! @param[in]   demuxer	pointer to a demuxer instance
//! @param[in]   pInfo	    pointer to an mp4dmux_seek_info structure to receive the sample info
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxGetSampleInfo(mp4dmux_tt *demuxer, mp4dmux_seek_info *pInfo);

//! call to retrieve sample duration map
//!
//! @param[in]     demuxer      pointer to a demuxer instance
//! @param[in]     parser_num   Parser number for a sample map
//! @param[in]     stream_num   Stream number for a sample map
//! @param[in/out] sample_count Number of samples allocated in info array. If info is NULL actual number of samples in sample map is returned. Otherwise actual number of samples written is returned
//! @param[in/out] info         Array of samples info. Should be allocated by user. If NULL than sample count in a map is returned in sample_count
//!
//! @return   0 if successful, else non-zero

int32_t MC_EXPORT_API mp4DemuxGetSampleMap(mp4dmux_tt *demuxer, int parser_num, int stream_num, uint64_t* sample_count, demuxer_sample_info *info);

#ifdef __cplusplus
}
#endif

#define PARAM_INFO_NAME 0x00001000
#define PARAM_INFO_TYPE 0x00001001
#define PARAM_INFO_DIM  0x00001002


//! @name GetAPIExt parameter ids
//! @brief Using these, one can extract additional information from the demuxer or pass some configuration settings
//! @{
#define DEMUX_MP4_PARAM_TRANSFORM_MATRIX        0x0100      //!< Retrieves transformation matrix for the specified track, see @sa mp4dmux_transformation_matrix_tt
#define DEMUX_MP4_PARAM_TRANSFORM_DATA          0x0101      //!< Retrieves transformation data for the specified track, see @sa mp4dmux_transformation_data_tt
#define DEMUX_MP4_PARAM_EDITLIST_ENABLED        0x0201      //!< Enable or disable edit list processing, uses uint32_t
//! @}

//! @brief Structure for retrieving transformation matrix for the track
typedef struct {
    int32_t track_num;                    //!< Track number to retrieve the matrix from, track_num < 0 for movie-wide matrix
    double matrix_coeff[9];               //!< Matrix coefficients as stored in the file
} mp4dmux_transformation_matrix_tt;

//! @brief Structure for retrieving transformation details for the track
typedef struct {
    int32_t track_num;                    //!< Track number to retrieve the matrix from, track_num < 0 for movie-wide transform details
    uint32_t rotation_first;              //!< Indicates whether the transform is RST (Rotation-Scaling-Translation) or SRT
    double rotation_angle;                //!< Rotation angle
    double translate_x;                   //!< Horizontal translation offset
    double translate_y;                   //!< Vertical translation offset
    double scale_x;                       //!< Horizontal scaling factor, always positive
    double scale_y;                       //!< Vertical scaling factor
} mp4dmux_transformation_data_tt;

//--------------------------------------------------------------------------------------------------
// MP4 Push Demultiplexer
//--------------------------------------------------------------------------------------------------

//! @name MP4 Demuxer object
typedef struct mp4_demuxer_push mp4dmx_push_tt;	//!< demuxer object

//! @name Segment event types
//! @{
#define MP4PDMUX_EVENT_NEW_SEGMENT      0x01        //!< New stream segment is available for demuxing
#define MP4PDMUX_EVENT_FAILURE          0x02        //!< Input stream parser has suffered a failure it could not recover from
#define MP4PDMUX_EVENT_EOS              0x03        //!< Input stream signalled EOS, no new segments are to be expected
#define MP4PDMUX_EVENT_DISCONTINUITY    0x04        //!< Discontinuity was signalled in the input stream
//! @}

#ifndef DMXU_SAMPLE_ENCRYPTION_INFO
#define DMXU_SAMPLE_ENCRYPTION_INFO (DMUX_CHUNK_INFO_EX + 1)
#endif /* DMXU_SAMPLE_ENCRYPTION_INFO */

typedef void (MC_EXPORT_API *segment_callback_func)(mp4dmx_push_tt* demuxer, uint32_t event_type, void* event_data);

#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

//! @brief Used to define the \ref mp4dmx_push_tt object
//! @details The demultiplexer uses the mp4dmux_push_settings structure to hold its settings
struct mp4dmux_push_settings
{
    uint8_t add_adts_headers;				//!< @brief If this value equal 1, demultiplexer forming special headers before every AAC sample (ADTS header).
    uint8_t annexb_output;				//!< @brief If this value equal 1, demultiplexer replace sizes of H.264/AVC slices on special start code 0x0001.
    uint8_t add_amr_header;				//!< @brief If this value equal 1, demultiplexer forming special headers before every AMR sample.
    segment_callback_func segment_callback;             //!< @brief Segment event callback function
    void* segment_callback_user_data;                   //!< @brief User data pointer to be passed to the callback function
    uint32_t input_buffer_size;                         //!< @brief Input buffer size. If set to zero, will default to 64Kb.
    uint8_t detect_discontinuty;                //!<@brief Detect broken sequences and skip garbage in bitstream
};

#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

//! Create and initialize an push demuxer instance
//!
//! @param[in]    callbacks     pointer to a structure with resource callbacks
//! @param[in]    set           pointer to an mp4dmux_settings structure
//!
//! @return a pointer to a demuxer instance if successful, else NULL

mp4dmx_push_tt * MC_EXPORT_API mp4DemuxPushNew(const callbacks_t * callbacks, void * reserved, struct mp4dmux_push_settings *set);


//! Release a push demuxer instance
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//!
//! @return    none

void MC_EXPORT_API mp4DemuxPushFree(mp4dmx_push_tt *demuxer);


//! Initialize push demuxer input
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//! @param[in]      input           input bufstream
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushInitStream(mp4dmx_push_tt* demuxer, bufstream_tt* input);


//! Reset the demuxer to its initial state, closing down the input and stopping all the output streams
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushCloseStream(mp4dmx_push_tt* demuxer);


//! Wait for stream end (all segments demuxed until EOS or parser failure)
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushWaitDone(mp4dmx_push_tt* demuxer);

    
//! Get current segment info (number of tracks, mostly)
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//! @param[out]     file_info       file info structure to be filled with the segment info
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushSegmentGetInfo(mp4dmx_push_tt* demuxer, struct mp4dmux_file_info*  file_info);


//! Get stream format for the specified track
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//! @param[out]     stream_format   pointer to stream format structure intended to be filled
//! @param[in]      stream_num      track number to retrieve the stream format for
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushSegmentGetStreamInfo(mp4dmx_push_tt* demuxer, mp4dmux_stream_format_tt* stream_format, int32_t stream_num);


//! Connect the segment demuxer to an output bufstream. Can only be done before the segment demuxing starts.
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//! @param[in]      set             pointer to the stream settings structure containing a pointer to the output bufstream
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushSegmentAddStream(mp4dmx_push_tt* demuxer, mp4dmux_stream_settings* set);


//! Disconnect the segment demuxer from the output bufstream. Can only be done before the segment demuxing starts.
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//! @param[in]      set             pointer to the stream settings structure containing a stream number to disconnect
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushSegmentRemoveStream(mp4dmx_push_tt* demuxer, mp4dmux_stream_settings* set);


//! Launch the segment demuxing
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushSegmentStart(mp4dmx_push_tt* demuxer);

//! Get Protection System Information (\ref protection_system_info_tt)
//!
//! @param[in]      demuxer         pointer to a demuxer instance
//! @param[out]     protection_info pointer to protection system information structure intended to be filled (\ref protection_system_info_tt)
//! @param[in]      protection_num  protection system information index, within range 0 .. \ref protection_system_info_count
//!
//! @return 0 if successful, non-zero otherwise

int32_t MC_EXPORT_API mp4DemuxPushSegmentGetProtectionSystemInfo(mp4dmx_push_tt* demuxer, protection_system_info_tt * protection_info, int32_t protection_num);

#ifdef __cplusplus
}
#endif

#endif // #if !defined (__DEMUX_MP4_API_INCLUDED__)

