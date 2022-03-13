/* ----------------------------------------------------------------------------
 * File: demux_mxf.h
 *
 * Desc: MXF Demuxer API
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */


/*! @file /demux_mxf.h
 */

#if !defined (__DEMUX_MXF_API_INCLUDED__)
#define __DEMUX_MXF_API_INCLUDED__

#include "mctypes.h"
#include "mcdefs.h"
#include "common_demux.h"
#include "bufstrm.h"
#include "mcapiext.h"
#include "mcmediatypes.h"


//! @name MXF Demuxer object
typedef struct mxf_demuxer mxfdmux_tt;	//!< demuxer object


//! @name Frame layout
//! @brief Specifies the type of frame layout
//! @{
#define LAYOUT_FULL_FRAME                            0x00		//! @brief Progressive lattice from top to bottom, stored in progressive line order 1,2,3,4,5,6,...

#define LAYOUT_SEPARATE_FIELDS                       0x01		//! @brief Interlaced lattice divided into two fields, stored as two fields 1,3,5,... and 2,4,6,... 
                                                                /*! @details Field 1 scans alternate lines from top to bottom, field 2 scans the intervening lines.
                                                                The second field is scanned at a later time than the first field (one field later).*/

#define LAYOUT_SINGLE_FIELD                          0x02		//! @brief Interlaced lattice with single field
                                                                /*! @details Interlaced lattice as for LAYOUT_SEPARATE_FIELDS above,
                                                                except that only one field is scanned and retained in the stored data, as 1,3,5,... or 2,4,6,...
                                                                or (1+2),(3+4),(5+6),... For display, the second field is derived by line replication or interpolation.*/

#define LAYOUT_MIXED_FIELDS                          0x03		//! @brief Interlaced lattice with mixed fields
                                                                /*! @details Interlaced lattice as for LAYOUT_SEPARATE_FIELDS, stored as a single matrix of interleaved lines
                                                                1,2,3,4,5,6 ... It is not common to use LAYOUT_MIXED_FIELDS in broadcast;
                                                                however, intermediate in-memory data structures sometimes use this format.*/

#define LAYOUT_SEGMENTED_FRAME                       0x04		//! @brief Interlaced lattice divided into two fields. 
                                                                /*! @details Field 1 scans alternate lines from top to bottom,
                                                                field 2 scans the intervening lines. The lines are stored as two fields 1,3,5,... 2,4,6,...
                                                                The two fields are taken from a single scan of the incoming image –	i.e.,
                                                                they are coincident in time, except for the effects of shutter angle. */
                                                                //!@}


                                                                //! @name Essence coding
                                                                //! @brief Specifies the type of picture essence_coding
                                                                //! Used with \ref  mxfdmux_stream_format.picture_essence_coding field
                                                                //! @{

                                                                //! @{
                                                                //! @brief Specifies the type of DV video essence_coding
#define ESSENCE_IEC_DV25                             0x0010
#define ESSENCE_DVCPRO25                             0x0011
#define ESSENCE_DVCPRO50                             0x0012
#define ESSENCE_DV100_720P                           0x0013
#define ESSENCE_DV100_1080I                          0x0014
//!@}

//! @{
//! @brief Specifies the type of JPEG2000 video essence_coding
#define ESSENCE_JPEG2000                             0x0100
#define ESSENCE_JPEG2000_DCI2K                       0x0101
#define ESSENCE_JPEG2000_DCI4K                       0x0102
//!@}

//! @{
//! @brief Specifies the type of MPEG2 video essence_coding
#define ESSENCE_MPEG2_422P_HL_LONG_GOP               0x0200
#define ESSENCE_MPEG2_MP_HL_LONG_GOP                 0x0201
#define ESSENCE_MPEG2_MP_H14_LONG_GOP                0x0202
#define ESSENCE_MPEG2_422P_HL_LONG_GOP_OVERCRANK     0x0203
#define ESSENCE_MPEG2_MP_HL_LONG_GOP_OVERCRANK       0x0204
#define ESSENCE_MPEG2_MP_H14_LONG_GOP_OVERCRANK      0x0205
#define ESSENCE_MPEG2_D10_30                         0x0206
#define ESSENCE_MPEG2_D10_40                         0x0207
#define ESSENCE_MPEG2_D10_50                         0x0208
//!@}

//! @{
//! @brief Specifies the type of AVC-Intra video essence_coding
#define ESSENCE_AVCI_50_720                          0x0300
#define ESSENCE_AVCI_50_1080                         0x0301
#define ESSENCE_AVCI_100_720                         0x0302
#define ESSENCE_AVCI_100_1080                        0x0303
#define ESSENCE_AVCI_200_720                         0x0304
#define ESSENCE_AVCI_200_1080                        0x0305
//
#define ESSENCE_AVCI_HIGH10_UNCONSTRAINED            0x0340
#define ESSENCE_AVCI_HIGH422_UNCONSTRAINED           0x0341
//!@}

//! @{
//! @brief Specifies the type of AVC Long GOP video essence_coding
#define ESSENCE_AVC_LONG_GOP_HIGH_UNCONSTRAINED      0x0350
#define ESSENCE_AVC_LONG_GOP_HIGH422_UNCONSTRAINED   0x0351
//!@}

//! @{
//! @brief Specifies the type of generic AVC video essence_coding
#define ESSENCE_AVC                                  0x0360
#define ESSENCE_AVC_LONG_GOP                         0x0361
#define ESSENCE_AVC_INTRA                            0x0362
//!@}


//! @brief Specifies the type of MPEG4 video essence_coding
#define ESSENCE_MPEG4                                0x0400


//! @brief Specifies the type of VC-3 video essence_coding (CID)
#define ESSENCE_VC3                                  0x0500
#define ESSENCE_VC3_1235                             0x0535
#define ESSENCE_VC3_1237                             0x0537
#define ESSENCE_VC3_1238                             0x0538
#define ESSENCE_VC3_1241                             0x0541
#define ESSENCE_VC3_1242                             0x0542
#define ESSENCE_VC3_1243                             0x0543
#define ESSENCE_VC3_1244                             0x0544
#define ESSENCE_VC3_1250                             0x0550
#define ESSENCE_VC3_1251                             0x0551
#define ESSENCE_VC3_1252                             0x0552
#define ESSENCE_VC3_1253                             0x0553
#define ESSENCE_VC3_1256                             0x0556
#define ESSENCE_VC3_1258                             0x0558
#define ESSENCE_VC3_1259                             0x0559
#define ESSENCE_VC3_1260                             0x0560
#define ESSENCE_VC3_1270                             0x0570
#define ESSENCE_VC3_1271                             0x0571
#define ESSENCE_VC3_1272                             0x0572
#define ESSENCE_VC3_1273                             0x0573
#define ESSENCE_VC3_1274                             0x0574
//!@}

//! @brief Specifies the type of Pro Res video essence_coding
#define ESSENCE_ProRes422Proxy                       0x0600
#define ESSENCE_ProRes422LT                          0x0601
#define ESSENCE_ProRes422                            0x0602
#define ESSENCE_ProRes422HQ                          0x0603
#define ESSENCE_ProRes4444                           0x0604
#define ESSENCE_ProRes4444XQ                         0x0605
//!@}

//! @brief Specifies the type of HEVC video essence_coding
#define ESSENCE_HEVC_MAIN                            0x0700
#define ESSENCE_HEVC_MAIN_10                         0x0701
#define ESSENCE_HEVC_MAIN_422_10                     0x0702
#define ESSENCE_HEVC_MAIN_422_12                     0x0703
#define ESSENCE_HEVC_MAIN_INTRA                      0x0704
#define ESSENCE_HEVC_MAIN_10_INTRA                   0x0705
#define ESSENCE_HEVC_MAIN_422_10_INTRA               0x0706
#define ESSENCE_HEVC_MAIN_422_12_INTRA               0x0707
//!@}

//! @name Seeking flags
//! @brief Specifies flags for seeking process
//! Used with \ref  mxfdmux_seek_settings_s.seek_flags field
//! @{
#define SEEKING_FLAG_BY_FRAME                        0x00000001        //!< seeking by number of frame
#define SEEKING_FLAG_BY_TIME                         0x00000002        //!< seeking by times in 27Mhz units
#define SEEKING_FLAG_BY_ABSOLUTE_OFFSET              0x00000004        //!< seeking by absolute offset in file
#define SEEKING_FLAG_RETURN_REF_TIME                 0x00000100        //!< need to return the time of where the parser is positioned
#define SEEKING_FLAG_RETURN_REF_OFFSET               0x00000200        //!< need to return the offset of where the parser is positioned
#define SEEKING_FLAG_RETURN_REF_FRAME                0x00000400        //!< need to return the frame number of where the parser is positioned
#define SEEKING_FLAG_RETURN_TIME                     0x00000800        //!< converts the position value to a reference time returns it in the start_pos or stop_pos variable. This flag is useful if you are using another time format, such as frames.
//!@}


//! @name Operational patterns
//! @brief Used with \ref  mxfdmux_file_info.operational_pattern field
//! @{
#define MXF_OP_PATTERN_UNKNOWN                       0x00
#define MXF_OP_PATTERN_ATOM                          0x01
#define MXF_OP_PATTERN_1A                            0x02
#define MXF_OP_PATTERN_1B                            0x03
#define MXF_OP_PATTERN_1C                            0x04
#define MXF_OP_PATTERN_2A                            0x05
#define MXF_OP_PATTERN_2B                            0x06
#define MXF_OP_PATTERN_2C                            0x07
#define MXF_OP_PATTERN_3A                            0x08
#define MXF_OP_PATTERN_3B                            0x09
#define MXF_OP_PATTERN_3C                            0x0A
#define MXF_OP_PATTERN_XDCAM_PROXY                   0x0B
//!@}


//! @name MXF Track Types
//! @brief Used with \ref  mxfdmux_dm_track_info_s.track_type field
//! @{
#define MXF_TRACK_TYPE_TIMLINE                       0x00
#define MXF_TRACK_TYPE_STATIC                        0x01
#define MXF_TRACK_TYPE_EVENT                         0x02
//!@}

//! @name MXF Descriptive Metadat Framework Types
//! @brief Used with \ref  mxfdmux_dm_segment_info_s.framework_type field
//! @{
#define MXF_DM_FRAMEWORK_TYPE_DARK                        0   //!< Dark Meta data, DM Scheme and Framework type not known to the demuxer.

#define MXF_DM_FRAMEWORK_TYPE_DMS_1_PRODUCTION            10  //!< SMPTE ST 380 DMS-1 Production Framework
#define MXF_DM_FRAMEWORK_TYPE_DMS_1_CLIP                  11  //!< SMPTE ST 380 DMS-1 Clip Framework
#define MXF_DM_FRAMEWORK_TYPE_DMS_1_SCENE                 12  //!< SMPTE ST 380 DMS-1 Scene Framework

#define MXF_DM_FRAMEWORK_TYPE_TEXT_BASED                  20  //!< SMPTE RP 2057 Text based Metadata Framework

#define MXF_DM_FRAMEWORK_TYPE_AMWA_AS03                   30  //!< AMWA AS-03 Descriptive Metadata framework

#define MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_CORE              40  //!< AMWA AS-11 Core Descriptive Metadata framework
#define MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_SEGMENTATION      41  //!< AMWA AS-11 Segmentation Descriptive Metadata framework
#define MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_UKDDP             42  //!< AMWA AS-11 UKDDP Descriptive Metadata framework

#define MXF_DM_FRAMEWORK_TYPE_P2_STATIC                   50  //!< Panasonic P2 Static Descriptive Metadata framework (carries Clip XML metadata)
#define MXF_DM_FRAMEWORK_TYPE_P2_EVENT                    51  //!< Panasonic P2 Event Descriptive Metadata framework (carries Thumbnail )
//!@}

//! @name MXF Demuxer external index mode
//! @brief Used with \ref mxfdmux_settings.index_mode field to specify how to deal with container index.
//! @{
#define MXF_INDEX_MODE_DEFAULT          0 //!< Build index without saving it to file (default).
#define MXF_INDEX_MODE_WRITE_IDX        1 //!< Try to use external index, build it and save to file otherwise.
#define MXF_INDEX_MODE_EXT_IDX_ONLY     2 //!< Use external index only, refuse to open container without index file.
//!@}


/*! Creates a type name for pointer to the function */
/*!
* Called to seek in the source file in splitter mode to the \a position,
* where position - file position to seek to
* return 0 if OK, 1 if fault
*/
typedef int32_t (*mxf_splitter_seek_callback)(void* split_app_ptr, int64_t position);


/*! Creates a type name for pointer to the function */
/*!
* Called to read \a bufferSize bytes of data from file to the \a buffer in splitter file mode,
* return number of bytes read, 0 if fault
*/
typedef int32_t (*mxf_splitter_read_callback)(void* split_app_ptr, uint8_t *buffer, int32_t bufferSize);


//! @name External functions
//! @brief External i/o function prototypes
/*!	@details These are depreciated, the mxfdmux_settings.p_external_io should be used instead
    If these are used, the mxfdmux_settings.file_length field MUST be filled in with the correct file size by the app*/
//! @{

/*!
* Called to open a file
* and the return value will be passed back to the seek, read and close functions
* ext_app_ptr - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileName - if a filename was passed to the demultiplexer, it is returned here
* return the file handle as a long or -1 if an error occurs
*/
typedef long (*mxf_external_open_callback)(void *ext_app_ptr, char *fileName);
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__vxworks)
typedef long (*mxf_external_open_callbackW)(void *ext_app_ptr, wchar_t *fileName);
#else
typedef long (*mxf_external_open_callbackW)(void *ext_app_ptr, uint16_t *fileName);
#endif

/*!
* Called to seek in a file
* ext_app_ptr - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileHandle - a file handle returned by the fn_external_open_callback function
* position - the position to seek to
* return 0 if Ok, 1 if an error occurs
*/
typedef int32_t (*mxf_external_seek_callback)(void *ext_app_ptr, long fileHandle, int64_t position);

/*!
* called to read from a file
* appData - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileHandle - a file handle returned by the fn_external_open_callback function
* buffer - a buffer for the data
* bufferSize - the number of bytes to read
* return the number of bytes actually read or 0 if an error occurs
*/
typedef int32_t (*mxf_external_read_callback)(void *ext_app_ptr, long fileHandle, uint8_t *buffer, int32_t bufferSize);

/*
* called to close a file
* appData - the ext_app_ptr field in the mpgdmux_settings structure is passed back here
* fileHandle - a file handle returned by the fn_external_open_callback function
*/
typedef void (*mxf_external_close_callback)(void *ext_app_ptr, long fileHandle);

//!@}


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif


//! @brief Used to define the \ref mxfdmux_tt object
//! @details The demultiplexer uses the mxfdmux_settings structure to hold its settings
struct mxfdmux_settings
{
    uint32_t growing_files_open_timeout;		              //!< @brief Time out in milliseconds for opening growing files. Has no effect when external IO is specified.
                                                                            //!< @details If > 0 and mxfDemuxOpen detects an EOF, it will repeatedly check for updated file size and retry opening and parsing header up the specified timeout.
//! @{
//! @brief Splitter mode parameters. Used by mfimport.
    int32_t splitter_mode;									              //!< @brief Indicates that the demuxer is being used in splitter mode. 
                                                                                        //!< @details This mode is sort of a hybrid between file and stream mode.
    mxf_splitter_seek_callback splitter_seek_callback;		//!< @brief Seek callback for splitter mode.
    mxf_splitter_read_callback splitter_read_callback;		//!< @brief Read callback for splitter mode.
    void *split_app_ptr;									                //!< @brief App pointer for splitter mode.
//!@}

//! @{
//! @brief External IO parameters. Used by mfimport.
    int32_t use_external_io;								              //!< @brief Indicates that the app will provide the I/O for the demuxer.
    mxf_external_open_callback external_open_callback;		//!< @brief External open callback pointer.
    mxf_external_open_callbackW external_open_callbackW;	//!< @brief UNICODE external open callback pointer.
    mxf_external_seek_callback external_seek_callback;		//!< @brief External seek callback pointer.
    mxf_external_read_callback external_read_callback;		//!< @brief External read callback pointer.
    mxf_external_close_callback external_close_callback;	//!< @brief External close callback pointer.
    void *ext_app_ptr;										                //!< @brief App pointer for external I/O mode, it is passed to the external callbacks.
//!@}

    int64_t file_length;				            //!< @brief Length of the input file, used for splitter and external I/O modes only.

    uint8_t parse_external_streams;		      //!< @brief Parameter for detecting and parsing external streams of OP-Atom files in same directory or Panasonic P2, Ikegami files within their specified directory structure.

    uint8_t format_pcm_output_flag;		      //!< @brief Set this field to 1 to output AES-3 audio streams as basic PCM samples, else output in the native format.

    mc_external_io_t *p_external_io;	      //!< @brief If non NULL and use_external_io = 1, this will be used instead of the above

    uint8_t is_growing_file;				        //!< @brief Treat incomplete files as 'growing file'. For more information on growing files support see the documentation.

    uint8_t deliver_aes3_331_headers;       //!< @brief Set this field to 1 to output AES-3 331 audio units including element headers. By default headers are stripped off and sent as DEMUX_CHUNK_INFO 'info' data.

    uint32_t growing_files_parse_timeout;   //!< @brief Time out in milliseconds for parsing growing files. Has no effect when external IO is specified.
                                            //!< @details If > 0 and mxfDemuxPush detects an EOF, it will repeatedly check for updated file size and retry parsing up the specified timeout.

    uint16_t preview_range;                 //!< @brief If > 0 the demuxer will only index the essence streams up to specified number of edit units or up to a range including this number depending on the files index implementation.
                                            /*!< @details Use this when only a brief file information and access to the first access units for quick preview is desired. It avoids time consuming index building on huge files with
                                                 multiple body partitions and index segments. The demultiplexer will stop advancing to body partitions and reading index segments once the specified range is within the total index range
                                                 that was read so far. The maximum range that can be requested is 65535. The resulting seek range may be larger, depending on the file and index structure. */

    uint8_t parse_on_file_size_update;      //!< @brief On mxfDemuxUpdateFilesize calls parse ahead up to EOF to further index the file. Growing file mode only! 

    uint8_t index_mode;                     //!< @brief Parameter which controls indexing mode behavior.
                                            //!< @details One of the MXF_INDEX_MODE_xxx values.

    char *index_file_path;                  //!< @brief UTF8 string which specifies path to index file. Can be NULL: in this case default value will be used (input file path with '.idx' suffix).

    uint8_t reserved[40];                   //!< Reserved
};




//! @brief Information about a stream
/*! @details This structure is used to get information about an elementary stream.
It uses the standard mc_stream_format_t structure defined in mcmediatypes.h. */
typedef struct mxfdmux_stream_format_s
{
    uint32_t ID;							            //!< @brief ID of the stream

    mc_stream_format_t format;				    //!< @brief Structure containing information about stream format, defined in mcmediatypes.h.

    uint8_t frame_layout;                 //!< @brief Frame layout of a video stream (according to SMPTE 377M E.2.2). One of LAYOUT_xxx_xxx

    uint32_t picture_essence_coding;	    //!< @brief Picture essence coding value of a video stream. One of ESSENCE_xxx... defined above

    int64_t duration;                     //!< @brief Stream duration in edit units, -1 is unknown value. On growing file this is the number of edit units parsed so far.

    uint32_t index_num_entries;				    //!< @brief Number of entries in the index_table for current stream.

    uint32_t index_entry_len;				      //!< @brief Length in bytes of each index entry for current stream.

    uint8_t *index_table;					        //!< @brief Pointer to the index table for current stream.

    int32_t ES_info_length;               //!< @brief Length of the \ref descriptors.

    uint8_t *descriptors;					        //!< @brief Contains any stream descriptors present. Currently not used.

    uint8_t external;                     //!< @brief Flag for external stream.

    uint8_t edit_rate_code;               //!< @brief Code for edit rate, one of FRAMERATExx.

    uint64_t origin;                      //!< @brief Timeline track origin - 1st valid frame on the timeline segment of this file, relative to the segments start - default 0. 

    uint64_t sample_count;                //!< @brief Actual number of samples in stream.

    uint8_t field_dominance;              //!< @brief Field dominance for interlaced frame layout. 1 = Top Field First, 2 = Bottom Field First.

    uint32_t track_id;                    //!< @brief MXF Track ID, not identical with the 0 based stream ID above.

    int64_t positionable_duration;        //!< @brief Duration in edit units in which the stream is positionable. 
                                          /*!< @details On a growing file this can be less than 'duration' and means seeks are possible up to this range up to now.
                                               If the file was opened for preview this is the maximum seek range for a preview. See mxfdmux_settings.preview_range parameter*/

    int64_t positionable_sample_count;    //!< @brief Duration in samples in which the stream is positionable. 
                                          /*!< @details On growing file this can be less than 'sample_count' and means seeks are possible up to this range up to now.
                                              If the file was opened for preview this is the maximum seek range for a preview. See mxfdmux_settings.preview_range parameter*/

    uint8_t active_format_descriptor;     //!< @brief Active Format Descriptor, ST 377-1. MXF version 1.3
                                          /*!< @details Bits 6..3 are AFD code data bits a3..a0 (SMPTE ST 2016-1 Table 1). Bit 2 is Aspect Ratio bit, 0 = 4:3, 1 = 16:9.*/

    uint8_t reserved[4];				          //!< Reserved

} mxfdmux_stream_format_tt;


typedef uint8_t MXF_UUID[16];       //!@brief MXF Unique Identifier
typedef uint8_t MXF_UL[16];         //!@brief MXF Universal Label


//! @brief This structure is used to get information about a file.
struct mxfdmux_file_info
{
    int32_t stream_count;             //!< @brief Number of elementary streams in the input stream. Audio, video, ANC, VBI
    int32_t edit_unit_size;					  //!< @brief Size of the edit units in the MXF file.
    int64_t duration;						      //!< @brief Duration in edit units in the MXF file. -1 if unknown
    int32_t file_type;						    //!< @brief This field contains the MXF file type. One of the STREAM_XDCAM* defines in mfimport.h and STREAM_* in demux_mxf.h..
    int64_t file_size;						    //!< @brief Total size of the input file. 

    uint8_t reserved_2[8];            //!< Reserved
    uint8_t *reserved_3;              //!< Reserved

    uint8_t file_is_growing;          //!< @brief A growing file was detected. This is a temporary state, reset after the parser has hit a footer partition and completed the indexing.

    uint8_t file_is_truncated;        //!< @brief If the demuxer is not in growing file mode it signals truncated file when no footer partition is available. 
                                      /*!< @details This can either be detected when the Header Partition has Footer Partition Offset > 0 which doesn't match the file size and is signaled
                                           even in preview mode, or when the parser hits EOF without having seen a footer partition. */

    uint8_t reserved_1[3];					  //!< @brief Reserved

    int8_t operational_pattern;       //!< @brief MXF operational pattern of this file. One of MXF_OP_PATTERN_xxx defines 

    uint8_t dm_schemes_count;         //!< @brief Number of Descriptive Meta Data Scheme labels found in Preface
    MXF_UL *dm_schemes;               //!< @brief Pointer to an array of descriptive Meta Data Scheme labels found in Preface, or NULL

    uint32_t dm_tracks_count;         //!< @brief Number of Descriptive Meta Data tracks

    uint32_t dm_framework_set_count;  //!< @brief Number of Descriptive Meta Data Framework sets.
    MXF_UUID *dm_framework_sets;      //!< @brief Pointer to an array of the Descriptive Meta Data Framework Set instance UID's, or NULL

    uint8_t reserved[46];					    //!< Reserved
};



//! @brief This structure is used to get information about a descriptive meta data segment.
typedef struct mxfdmux_dm_segment_info_s
{
    int64_t duration;                       //!< @brief Duration in edit units. 
    int64_t start_position;                 //!< @brief Event Start Position in edit units. 
    uint8_t* event_comment;                 //!< @brief Event Comment, a UTF 16 string. Can be NULL.

    uint32_t num_track_ids;                 //!< @brief Number of essence track ID's this segments framework refers to. Refers to all essence tracks if 0.
    uint32_t *track_ids;                    //!< @brief Pointer to an array of essence track ID's this segments framework refers to. NUll if num_track_ids = 0.

    MXF_UUID framework_id;                  //!< @brief Descriptive Metadata Framework ID. Instance UID of a the top level framework. A framework may refer to other lover level framework sets.
    MXF_UUID plugin_id;                     //!< @brief Descriptive Metadata Plugin ID. Can be NULL.
    MXF_UL  dm_scheme_ul;                   //!< @brief Descriptive Metadata scheme label. Can be NULL

    uint8_t *application_environment_id;    //!< @brief Descriptive Metadata Application Environment ID, a UTF 16 string. Can be NULL.

    uint32_t framework_type;                //!< @brief Type of the referenced Framework. One of MXF_DM_FRAMEWORK_TYPE_xxx. An application also can check the mxfdmux_dm_framework_info_tt->key parameter to identify the type.

    uint8_t reserved[256];                  //!< @brief Reserved
}mxfdmux_dm_segment_info_tt;



//! @brief This structure is used to get information about a descriptive meta data track. Used with mxfDemuxGetDMTrackInfo function call.
typedef struct mxfdmux_dm_track_info_s
{
    uint32_t track_id;                      //!< @brief MXF Track ID,
    uint32_t track_type;                    //!< @brief MXF track type. Timeline, Static or Event. One of MXF_TRACK_TYPE_xxxx

    int64_t duration;                       //!< @brief Duration in edit units. 

    uint32_t num_segments;                  //!< @brief Number of descriptive meta data segments on this track.
    mxfdmux_dm_segment_info_tt *segments;   //!< @brief Pointer to an array of descriptive meta data segments info

    uint8_t reserved[104];                  //!< @brief Reserved
}mxfdmux_dm_track_info_tt;


//! @brief This structure is used to get information about a descriptive meta data framework set package. Filled by mxfDemuxGetDMFrameworkInfo function call. Passed to 
typedef struct mxfdmux_dm_framework_info_s
{
    MXF_UUID instance_id;                     //!< @brief Instance UID. Segments and and other framework sets refer to it.
    MXF_UUID generation_uid;                  //!< @brief Generation UID.
    uint32_t framework_type;                  //!< @brief Type of the framework. One of MXF_DM_FRAMEWORK_TYPE_xxx. 
                                              //!< @details Informational only, an application also can check the key parameter to identify the type.
    // package
    MXF_UUID key;                             //!< @brief package key, 16 byte UL used to identify the type of the framework
    uint64_t length;                          //!< @brief length of package data including all tags.
    //
    uint8_t reserved[196];
}mxfdmux_dm_framework_info_tt;



//! @brief Used to create a parser
struct mxfdmux_parser_settings
{
    int32_t requested_buffer_size;	//!< @brief Size in bytes the app wished the parser to use for bitstream buffers.

    uint8_t reserved[65];				    //!< Reserved
};


//! @brief These settings are used to add a stream to a parser.
struct mxfdmux_stream_settings
{
    int32_t stream_num;						//!< @brief This field contains the zero based stream index requested by the app. 
                                                        //!< @details The range is 0 .. \ref mxfdmux_file_info.stream_count – 1.

    bufstream_tt *bs;						  //!< @brief This field contains a pointer to the output bufstream for this elementary stream.

    uint8_t reserved[64];					//!< Reserved
};

//! @brief Seek settings
/*! @details Used by \ref mxfDemuxSetPositions function
        If the SEEKING_FLAG_BY_FRAME is specified then start_pos, stop_pos are numbers of frames.
        If the SEEKING_FLAG_BY_TIME is specified then start_pos, stop_pos are times in 27Mhz units.
        If the SEEKING_FLAG_BY_ABSOLUTE_OFFSET is specified then start_pos, stop_pos are absolute offsets in file. */
typedef struct mxfdmux_seek_settings_s
{
    int64_t start_pos;              //!< @brief Input/output, specifies start position.

    int64_t stop_pos;               //!< @brief Input/output, specifies stop position

    int32_t seek_flags;             //!< @brief Combination of SEEKING_FLAG_xxx flags

  //! @{
  //! @brief these do not change the file position, they merely return the appropriate time:
    int64_t ref_time;             /*	!< @brief Output, the resulting time in 27Mhz units
                                                        !< @details This is the time of the where the parser is positioned.
                                                          Usually the reference I frame.
                                                          Need to use \ref SEEKING_FLAG_RETURN_REF_TIME flag */

    int64_t ref_offset;           /*	!< @brief Output, the resulting absolute offset
                                                        !< @details This is the offset of the where the parser is positioned.
                                                        Need to use SEEKING_FLAG_RETURN_REF_OFFSET flag */

    int64_t ref_frame;           /*	!< @brief Output, the resulting frame number
                                                        !< @details This is the frame number of the where the parser is positioned
                                                        Need to use SEEKING_FLAG_RETURN_REF_FRAME flag */
                                                        //!@}

    uint8_t seek_next_key_frame;    //!< @brief Return the time of the next key frame based on seek_time

    uint8_t seek_prev_key_frame;    //!< @brief Return the time of the previous key frame based on seek_time                                                

    uint8_t reserved[64];					  //!< Reserved

} mxfdmux_seek_settings_t;


#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif


//! Call this function to create and initialize an demuxer instance
//!
//! @param[in]      callbacks	Pointer to a structure with resource callbacks 
//! @param[in]      set			Pointer to a filled in \ref mxfdmux_settings structure
//!
//! @return  pointer to a \ref mxfdmux_tt object if successful, else NULL

mxfdmux_tt* MC_EXPORT_API mxfDemuxNew(const callbacks_t * callbacks, void * reserved, struct mxfdmux_settings *set);


//! Call to free a mxf demuxer instance
//!
//! @param[in]    demuxer		  Pointer to a demuxer instance
//!
//! @return    none

void MC_EXPORT_API mxfDemuxFree(mxfdmux_tt *demuxer);


//! Call this function to open a file for demuxing
//!
//! @param[in]      demuxer		Pointer pointer to a demuxer instance
//! @param[in]      reserved	Reserved
//! @param[in]      filename	Pointer to a filename to open
//!
//! @return  0 if successful, 1 if an error occurred, 2 if the file did not contain enough data to parse the header meta data and gather stream information (growing file).

int32_t MC_EXPORT_API mxfDemuxOpen(mxfdmux_tt *demuxer, void *reserved, char *filename);

#if !defined(__APPLE__) && !defined(__linux__) && !defined(__vxworks)

//! Unicode version.
int32_t MC_EXPORT_API mxfDemuxOpenW(mxfdmux_tt *demuxer, void *reserved, wchar_t *filename);

#else

//! This version will currently return an error!
int32_t mxfDemuxOpenW(mxfdmux_tt *demuxer, void *reserved, unsigned short *filename);

#endif


//! Call to close file
//!
//! @param[in]      demuxer		Pointer to a demuxer instance 
//! @return  none

void MC_EXPORT_API mxfDemuxClose(mxfdmux_tt *demuxer);


//! Call to get information about the input file
//!
//! @param[in]      demuxer		        Pointer to a demuxer instance
//! @param[out]     file_info	        Pointer to the filled \ref mxfdmux_file_info structure
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxGetFileInfo(mxfdmux_tt *demuxer, struct mxfdmux_file_info *file_info);


//! Call to get format information about the specified stream in the file
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
//! @param[out]     stream_format	    Pointer to the filled \ref mxfdmux_stream_format_tt structure
//! @param[in]      stream_num		    Number of the stream
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxGetStreamFormat(mxfdmux_tt *demuxer, mxfdmux_stream_format_tt *stream_format, int32_t stream_num);


//! Call to get start timecode information
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
//! @param[out]     start_timecode	  Pointer to the filled \ref mc_time_code_t structure
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxGetStartTimecode(mxfdmux_tt *demuxer, mc_time_code_t *start_timecode);


//! Call to create and initialize a parser instance
//!
//! @param[in]     demuxer			Pointer to a demuxer instance
//! @param[out]    set				  Pointer to the filled \ref mxfdmux_parser_settings structure
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxNewParser(mxfdmux_tt *demuxer,struct mxfdmux_parser_settings *set);


//! Call to free a parser instance
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//!
//! @return  none

void MC_EXPORT_API mxfDemuxFreeParser(mxfdmux_tt *demuxer, int32_t parser_num);


//! Call to add an output bufstream for the specified stream
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//! @param[in]    set				    Pointer to an \ref mxfdmux_stream_settings structure
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxAddStream(mxfdmux_tt *demuxer, int32_t parser_num, struct mxfdmux_stream_settings *set);


//! Call to add an output bufstream for the specified stream
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num	  Parser number
//! @param[in]    set				    Pointer to an \ref mxfdmux_stream_settings structure
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxRemoveStream(mxfdmux_tt *demuxer, int32_t parser_num, struct mxfdmux_stream_settings *set);


//! Call to seek to the specified file position
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//! @param[in]    file_pos			The position to seek to
//!
//! @return  0 if successful, 2 if EOF, 1 if an error occurs

int32_t MC_EXPORT_API mxfDemuxSeek(mxfdmux_tt *demuxer, int32_t parser_num, int64_t file_pos);


//! Call to seek to the specified time
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//! @param[in]    set				    Pointer to a \ref mxfdmux_seek_settings_t structure
//!
//! @return  0 if successful, 2 if EOF, 1 if an error occurs

int32_t MC_EXPORT_API mxfDemuxSetPositions(mxfdmux_tt *demuxer, int32_t parser_num, mxfdmux_seek_settings_t *set);


//! Call to push a packet through the parser
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//!
//! @return  non-zero if successful, the number of bytes consumed from the source stream, 0 if EOF, -1 if an error occurred.

int32_t MC_EXPORT_API mxfDemuxPush(mxfdmux_tt *demuxer, int32_t parser_num);


//! Call to re-open a parser
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxWakeParser(mxfdmux_tt *demuxer, int32_t parser_num);


//! Call to close the actual file of a parser, the parser is left intact and can be re-opened with the wake function
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxSleepParser(mxfdmux_tt *demuxer, int32_t parser_num);


//! Call to have the file size updated if it is getting bigger
//!
//! @param[in]    demuxer			  Pointer to a demuxer instance
//! @param[in]    parser_num		Parser number
//! @param[in]    file_size			New file size if splitter or external i/o mode, else unused
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxUpdateFilesize(mxfdmux_tt *demuxer, int32_t parser_num, int64_t file_size);


//! Call to have the parser perform a seek before the next read
//!
//! @param[in]    demuxer			    Pointer to a demuxer instance
//! @param[in]    parser_num		  Parser number
//! @param[in]    split_app_ptr		Can be used to update the split_app_ptr from the \ref mxfdmux_settings structure
//!
//! @return  0 if successful, else non-zero

int32_t MC_EXPORT_API mxfDemuxSetFirstSplitRead(mxfdmux_tt *demuxer, int32_t parser_num, void *split_app_ptr);


//! Call to get extend API functions
//!
//! @param[in]	func	ID	for function a pointer to get
//!
//! @return function pointer or NULL

APIEXTFUNC MC_EXPORT_API mxfDemuxGetAPIExt(uint32_t func);



//////////////////////// Descriptive Metadata functions ////////////////////////////////////////////////////


//! Call to get information about the specified Descriptive Meta Data track in the file
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
//! @param[out]     dm_track_info	    Pointer to the filled \ref mxfdmux_dm_track_info_tt structure
//! @param[in]      track_idx		      Index of the track
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxGetDMTrackInfo(mxfdmux_tt *demuxer, mxfdmux_dm_track_info_tt *dm_track_info, uint32_t track_idx);


//! Call to get information about the specified Descriptive Meta Data Framework set in the file
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
//! @param[out]     dm_framework_info	Pointer to the filled \ref mxfdmux_dm_framework_info_tt structure
//! @param[in]      framework_id		  Instance UID of the framework set to look up (alternative to framework_idx. framework_id is prefered if both parameters are set)
//! @param[in]      framework_idx		  Index of the framework set to look up (alternative to framework_idx. framework_id is prefered if both parameters are set)
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxGetDMFrameworkInfo(mxfdmux_tt *demuxer, mxfdmux_dm_framework_info_tt *dm_framework_info, MXF_UUID framework_id, uint32_t framework_idx);


//! Call to open an internal parser for reading of DM framework data.
//! Call this once before calling mxfDemuxGetDMFrameworkData multiple times.
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
int32_t MC_EXPORT_API mxfDemuxOpenDMParser(mxfdmux_tt *demuxer);


//! Call to close the internal parser for reading of DM framework data.
//! Call this when done with reading descriptive meta data frameworks to save resources.
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
int32_t MC_EXPORT_API mxfDemuxCloseDMParser(mxfdmux_tt *demuxer);


//! Call to read a raw opaque Descriptive Meta Data framework into a buffer - 
//! The Buffer will be filled with the KLV wrapped framework data (V), except key (K) and length (L) values, starting from the byte of the value field
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
//! @param[in]      framework_id		  Instance UID of the framework to read
//! @param[out]     framework_buffer  User allocated buffer of size mxfdmux_dm_framework_info_tt->length to receive the data
//! @param[in]      buffer_size       Size in Bytes of the buffer
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxGetDMFrameworkData(mxfdmux_tt *demuxer, MXF_UUID framework_id, uint8_t* framework_buffer, uint64_t buffer_size);


//! Call to lookup a local tag in the files primer pack and retrieve the 16 byte identifier UUID this tag is an alias for.
//! Used when parsing descriptive meta data frameworks.
//!
//! @param[in]      demuxer			      Pointer to a demuxer instance
//! @param[in]      tag 		          The 2 byte tag to lookup.  
//! @param[out]     tag 		          16 byte UID to be filled.
//!
//! @return  0 if successful, else non-zero
int32_t MC_EXPORT_API mxfDemuxTranslateLocalTag(mxfdmux_tt *demuxer, uint16_t tag, MXF_UUID identifier);





#ifdef __cplusplus
}
#endif


//! @name Depreciated Items
//! @brief Outdated - do not use - for compatibility only
//! @{

//! use STREAM_MXF_xxx from mcdefs.h instead
#define STREAM_DV                         0x10        
#define STREAM_JPEG2000                   0x11
#define STREAM_XDCAM_DV                   0x09    
#define STREAM_P2_DVCPRO                  0x12    
#define STREAM_P2_AVCI                    0x13    
#define STREAM_MXF                        0x14    
#define STREAM_IKEGAMI                    0x15

//! use enums in mcmediatypes.h instead
#define MXFDMUX_MPEG_VIDEO                0x01    // MPEG video
#define MXFDMUX_AES_331M_AUDIO            0x02    // AES 331M PCM audio
#define MXFDMUX_AES_382M_AUDIO            0x03    // AES 382M PCM audio
#define MXFDMUX_DV_VIDEO                  0x04    // DV video
#define MXFDMUX_JPEG2000_VIDEO            0x05    // JPEG2000 video
#define MXFDMUX_AVCI_VIDEO                0x06    // AVC-intra video
#define MXFDMUX_PCM_AUDIO                 0x07    // PCM audio
#define MXFDMUX_MPEG4_VIDEO               0x08    // MPEG-4 video
#define MXFDMUX_ALAW_AUDIO                0x09    // ALAW audio
#define MXFDMUX_HEVC_VIDEO                0x0A    // HEVC video


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

//! use mxfdmux_stream_format_tt instead
struct mxfdmux_stream_info
{
    int32_t sdk_stream_type;                //! stream type using SDK defines
    uint32_t ID;

    //! audio specific info
    int32_t aes3_sample_size;
    int32_t aes3_sample_rate;
    int32_t aes3_channels;
    uint8_t audio_rate_code;

    //! video specific info
    uint32_t width;
    uint32_t height;
    uint8_t frame_rate_code;
    uint8_t frame_layout;                   //! interlace or progressive layout
    uint32_t picture_essence_coding;

    int32_t ES_info_length;                 //! length of the descriptors below
    uint8_t *descriptors;
    uint8_t field_dominance;

    uint8_t reserved[48];
};

#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

//! use mxfDemuxGetStreamFormat instead
int32_t MC_EXPORT_API mxfDemuxGetStreamInfo(mxfdmux_tt *demuxer, struct mxfdmux_stream_info *stream_info, int32_t stream_num);

#ifdef __cplusplus
}
#endif

//! @}

#endif // #if !defined (__DEMUX_MXF_API_INCLUDED__)

