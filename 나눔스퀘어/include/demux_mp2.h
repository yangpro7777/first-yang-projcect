/* ----------------------------------------------------------------------------
 * File: demux_mp2.h
 *
 * Desc: MPEG Demuxer API
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

#if !defined (__DEMUX_MP2_API_INCLUDED__)
#define __DEMUX_MP2_API_INCLUDED__

#include "mctypes.h"
#include "mcdefs.h"
#include "common_demux.h"
#include "mcapiext.h"
#include "bufstrm.h"
#include "mcmediatypes.h"
#include "crypto_ca.h"

#define ECM_STREAM                0x1F0L
#define EMM_STREAM                0x1F1L
#define ITU_T_H2220_STREAM        0x1F2L
#define ISO_IEC_13522_STREAM      0x1F3L
#define ITU_T_H2221_TYPEA_STREAM  0x1F4L
#define ITU_T_H2221_TYPEB_STREAM  0x1F5L
#define ITU_T_H2221_TYPEC_STREAM  0x1F6L
#define ITU_T_H2221_TYPED_STREAM  0x1F7L
#define ITU_T_H2221_TYPEE_STREAM  0x1F8L
#define ANCILLARY_STREAM          0x1F9L

//! @file demux_mp2.h
//! MP2 Demuxer and MPEG Demuxer interfaces

//////////////////////////////////////////////////////////////////////////////////
//
// new mp2DemuxXXX API, the old mpegDemuxXXX API follows the new
//
//////////////////////////////////////////////////////////////////////////////////

//! @name find stream options
//! @anchor MP2DMUX_FIND
//!@{
#define MP2DMUX_FIND_NONE               0       //!< do the default search (all streams within the MB limit)
#define MP2DMUX_FIND_VIDEO              1       //!< stop looking after 1 video stream is found
#define MP2DMUX_FIND_AUDIO              2       //!< stop looking after 1 audio stream is found
#define MP2DMUX_FIND_VIDAUD             3       //!< stop looking after 1 video and 1 audio are found
#define MP2DMUX_FIND_SUBPIC             4       //!< stop looking after 1 subpic stream is found
#define MP2DMUX_FIND_VIDAUDSP           7       //!< stop looking after 1 video, 1 audio and 1 subpic are found
#define MP2DMUX_FIND_ALL                8       //!< do not stop looking until the MB limit is reached

//!@}

//! @name file info flags
//! @anchor MP2DMUX_FILE
//!@{
#define MP2DMUX_FILE_AC3_TIMECODE       0x01
#define MP2DMUX_FILE_HAS_INDEX          0x02
//!@}

//! @name index info flags
//! @anchor MP2DMUX_xxx_FLAG
//! @{
#define MP2DMUX_SEQ_FLAG                0x01    //!< Sequence hdr present
#define MP2DMUX_GOP_FLAG                0x02    //!< GOP hdr present
#define MP2DMUX_IDR_FLAG                0x04    //!< h264 IDR flag
#define MP2DMUX_RFF_FLAG                0x08    //!< repeat first field flag
#define MP2DMUX_TFF_FLAG                0x10    //!< top field first flag
#define MP2DMUX_PROG_FLAG               0x20    //!< progressive frame flag if known
#define MP2DMUX_INTERLACED_FLAG         0x40    //!< interlaced frame flag if known
#define MP2DMUX_DISCON_FLAG             0x80    //!< a time discontinuity occurs in this AU

//!@}

//! @anchor STREAM_TYPE

//! @name ISO/IEC 13818-1 stream_type defines
//! @anchor STREAM_TYPE_ISO_IEC
//!@{
#define STREAM_TYPE_MPEG1_VIDEO         0x01    //!< ISO/IEC 11172-2 Video
#define STREAM_TYPE_MPEG2_CP_VIDEO      0x02    //!< ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream
#define STREAM_TYPE_MPEG1_AUDIO         0x03    //!< ISO/IEC 11172-3 Audio
#define STREAM_TYPE_MPEG2_AUDIO         0x04    //!< ISO/IEC 13818-3 Audio
#define STREAM_TYPE_PRIVATE_SECTIONS    0x05    //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections
#define STREAM_TYPE_PRIVATE_DATA        0x06    //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data
#define STREAM_TYPE_MHEG                0x07    //!< ISO/IEC 13522 MHEG
#define STREAM_TYPE_DSM_CC              0x08    //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM CC
#define STREAM_TYPE_H2221               0x09    //!< ITU-T Rec. H.222.1
#define STREAM_TYPE_TYPEA               0x0A    //!< ISO/IEC 13818-6 type A
#define STREAM_TYPE_TYPEB               0x0B    //!< ISO/IEC 13818-6 type B
#define STREAM_TYPE_TYPEC               0x0C    //!< ISO/IEC 13818-6 type C
#define STREAM_TYPE_TYPED               0x0D    //!< ISO/IEC 13818-6 type D
#define STREAM_TYPE_AUX                 0x0E    //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary
#define STREAM_TYPE_MPEG2_AAC           0x0F    //!< ISO/IEC 13818-7 Audio with ADTS transport syntax
#define STREAM_TYPE_MPEG4_VISUAL        0x10    //!< ISO/IEC 14496-2 Visual
#define STREAM_TYPE_MPEG4_AAC           0x11    //!< ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1
#define STREAM_TYPE_FLEXMUX_PES         0x12    //!< ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets
#define STREAM_TYPE_FLEXMUX_SECTIONS    0x13    //!< ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections.
#define STREAM_TYPE_SDP                 0x14    //!< ISO/IEC 13818-6 Synchronized Download Protocol
#define STREAM_TYPE_METADATA_PES        0x15    //!< Metadata carried in PES packets
#define STREAM_TYPE_METADATA_SECTIONS   0x16    //!< Metadata carried in metadata_sections
#define STREAM_TYPE_DATA_CAROUSEL       0x17    //!< Metadata carried in ISO/IEC 13818-6 Data Carousel
#define STREAM_TYPE_OBJECT_CAROUSEL     0x18    //!< Metadata carried in ISO/IEC 13818-6 Object Carousel
#define STREAM_TYPE_METADATA_SDP        0x19    //!< Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol
#define STREAM_TYPE_MPEG2_IPMP          0x1A    //!< IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP)
#define STREAM_TYPE_AVC                 0x1B    //!< AVC video stream as defined in ITU-T Rec. H.264 | ISO/IEC 14496-10 Video
#define STREAM_TYPE_MPEG4_NO_TS         0x1C    //!< ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, ALS and SLS
#define STREAM_TYPE_MPEG4_TEXT          0x1D    //!< ISO/IEC 14496-17 Text
#define STREAM_TYPE_AUX_VID             0x1E    //!< Auxiliary video stream as defined in ISO/IEC 23002-3
#define STREAM_TYPE_SVC                 0x1F    //!< SVC video sub-bitstream of an AVC video stream conforming to one or more profiles defined in
                                                //!< Annex G of ITU-T Rec. H.264 | ISO/IEC 14496-10
#define STREAM_TYPE_MVC                 0x20    //!< MVC video sub-bitstream of an AVC video stream conforming to one or more profiles defined in
                                                //!< Annex H of ITU-T Rec. H.264 | ISO/IEC 14496-10
#define STREAM_TYPE_J2K                 0x21    //!< JPEG2000 video, defined in Rec. ITU-T T.800 | ISO/IEC 15444-1
#define STREAM_TYPE_HEVC                0x24    //!< HEVC video
#define STREAM_TYPE_IPMP                0x7F    //!< IPMP stream

//!@}

//! @name defines from other formats
//! @anchor STREAM_TYPE_OTHER
//!@{
#define STREAM_TYPE_DIGICIPHER_II       0x80    //!< same as ITU-T Rec. H.262 | ISO/IEC 13818-2 Video
#define STREAM_TYPE_HDMV_LPCM           0x80    //!< HDMV LPCM, Blu-Ray/AVCHD
#define STREAM_TYPE_AC3                 0x81    //!< Dolby Digital, ATSC, Blu-Ray/AVCHD primary audio
#define STREAM_TYPE_DTS                 0x82    //!< DTS for Blu-Ray primary audio
#define STREAM_TYPE_DOLBY_LOSSLESS      0x83    //!< Dolby Lossless for Blu-Ray primary audio
#define STREAM_TYPE_DD_PLUS             0x84    //!< Dolby Digital Plus for Blu-Ray primary audio
#define STREAM_TYPE_DTS_HD              0x85    //!< DTS-HD for Blu-Ray primary audio
#define STREAM_TYPE_DTS_HD_XLL          0x86    //!< DTS-HD XLL for Blu-Ray primary audio
#define STREAM_TYPE_SCTE35              0x86    //!< SCTE 35 Splice info section
#define STREAM_TYPE_ATSC_DD_PLUS        0x87    //!< Dolby Digital Plus, ATSC
#define STREAM_TYPE_PRE_GRAPHICS        0x90    //!< Blu-Ray presentation graphics
#define STREAM_TYPE_INT_GRAPHICS        0x91    //!< Blu-Ray interactive graphics
#define STREAM_TYPE_TXT_SUBTITLE        0x92    //!< Blu-Ray text subtitle
#define STREAM_TYPE_DD_PLUS_SEC         0xA1    //!< Dolby Digital Plus for Blu-Ray secondary audio
#define STREAM_TYPE_DTS_HD_LBR_SEC      0xA2    //!< DTS-HD LBR for Blu-Ray secondary audio
//https://developer.apple.com/library/content/documentation/AudioVideo/Conceptual/HLS_Sample_Encryption/TransportStreamSignaling/TransportStreamSignaling.html
#define STREAM_TYPE_HLS_AVC_ENCRYPTED   0xdb    //!< HLS  AVC with enctyption
#define STREAM_TYPE_HLS_AAC_ENCRYPTED   0xcf    //!< HLS  AAC with enctyption
#define STREAM_TYPE_HLS_AC3_ENCRYPTED   0xc1    //!< HLS  AC3 with enctyption
#define STREAM_TYPE_HLS_DD_PLUS_ENCRYPTED  0xc2    //!< HLS  DD+ with enctyption
#define STREAM_TYPE_VC1                 0xEA    //!< VC1 video for ATSC and Blu-Ray

//!@}

//! @name external i/o function prototypes
//! 
//! @note These are depreciated, the mp2dmux_settings_t.p_external_io should be used instead.
//! 
//! @note If these are used, the mp2dmux_settings_t.file_length field MUST be filled in
//!       with the correct file size by the app.
//!@{

/*!
    @brief Called to open a file.
    The return value will be passed back to the seek, read and close functions.

     @param ext_app_ptr   the ext_app_ptr field in the mp2dmux_settings_t structure is passed back here
     @param  fileName     if a filename was passed to the demuxer, it is returned here

     @return              a representation of the file handle or NULL.
                          If an error occurs  this does not have to be the actual file handle,
                          any value the app  desires to represent the file handle can be used
*/
typedef long (*mp2_external_open_callback)(void *ext_app_ptr, char *fileName);

/*!
    @brief Called to open a file, UNICODE version.
    The return value will be passed back to the seek, read and close functions.

     @param ext_app_ptr  the ext_app_ptr field in the mp2dmux_settings_t structure is passed back here
     @param fileName     if a UNICODE filename was passed to the demuxer, it is returned here

     @return             a representation of the file handle or NULL.
                         If an error occurs  this does not have to be the actual file handle,
                         any value the app  desires to represent the file handle can be used
*/
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__native_client__) && !defined(__vxworks)
typedef long (*mp2_external_open_callbackW)(void *ext_app_ptr, wchar_t *fileName);
#else
typedef long (*mp2_external_open_callbackW)(void *ext_app_ptr, uint16_t *fileName);
#endif

/*!
    @brief Called to seek in a file.

     @param ext_app_ptr  the ext_app_ptr field in the mp2dmux_settings_t structure is passed back here
     @param fileHandle   a file handle returned by the external open callback function
     @param position     the position to seek to

     @return 0 if Ok, 1 if an error occurs
*/
typedef int32_t (*mp2_external_seek_callback)(void *ext_app_ptr, long fileHandle, int64_t position);

/*!
    @brief Called to read from a file.

     @param ext_app_ptr  the ext_app_ptr field in the mp2dmux_settings_t structure is passed back here
     @param fileHandle   a file handle returned by the external open callback function
     @param buffer       a buffer for the data
     @param bufferSize   the number of bytes to read
   
     @return the number of bytes actually read or 0 if an error occurs
*/
typedef int32_t (*mp2_external_read_callback)(void *ext_app_ptr, long fileHandle, uint8_t *buffer, int32_t bufferSize);

/*!
    @brief Called to close a file

     @param ext_app_ptr  the ext_app_ptr field in the mp2dmux_settings_t structure is passed back here
     @param fileHandle   a file handle returned by the external open callback function

     @return none
*/
typedef void (*mp2_external_close_callback)(void *ext_app_ptr, long fileHandle);

/*!
    @brief Called when DVD PCI packet is received

    @param parser_app_ptr  the parser_app_ptr field in the mp2dmux_parser_settings_t structure is passed back here
    @param buf             a pointer to the PCI data
    @param buf_len         the number of bytes in buf

    @return                none
*/
typedef void (*mp2_dvd_PCI_callback)(void *parser_app_ptr, void *buf, int32_t buf_len);


/*!
    @brief Called when DVD DSI packet is received

    @param parser_app_ptr - the parser_app_ptr field in the mp2dmux_parser_settings_t structure is passed back here
    @param buf       a pointer to the DSI data
    @param buf_len   the number of bytes in buf

    @return          none
*/
typedef void (*mp2_dvd_DSI_callback)(void *parser_app_ptr, void *buf, int32_t buf_len);


/*!
    @brief PSI callback definition

    @param PSI_app_ptr   the PSI_app_ptr field in the mp2dmux_PSI_callback_settings_t structure is passed back here
    @param buf           a pointer to the data
    @param buf_len       the number of bytes in buf

    @return              none
*/
typedef void (*mp2_PSI_callback)(void *PSI_app_ptr, void *buf, int32_t buf_len);


typedef struct mp2dmux_TS_packet_info_s mp2dmux_TS_packet_info_t;  // structure declared below

/*!
    @brief Extended PSI callback definition, transport streams only.
    Provides TS packet header and adaptation field information. Also allows
    the demuxing process to be aborted.

    @param PSI_app_ptr   the PSI_app_ptr field in the mp2dmux_PSI_callback_settings_t structure is passed back here
    @param pInfo         a pointer to a mp2dmux_TS_packet_info_t structure
    @return              0 = continue demuxing, non-zero = abort demuxing
*/
typedef int32_t (*mp2_PSI_callback_ex)(void *PSI_app_ptr, mp2dmux_TS_packet_info_t *pInfo);


//!@}

//! demuxer instance
typedef struct mp2_demuxer mp2dmux_tt;

#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

/*!
 @brief This structure is used to retrieve information about an elementary stream.
*/
typedef struct mp2dmux_stream_format_s
{
    uint8_t stream_type;                        //!< stream type as specified in the PMT or PSM

                                                //!< Usually one of the STREAM_TYPE_xxx defines in demux_mp2.h 
                                                //!<(See \ref STREAM_TYPE_ISO_IEC "ISO/IEC 13818-1 stream_type defines" or
                                                //!< \ref STREAM_TYPE_OTHER "defines from other formats").
                                                //!< If the PMT or PSM is not present this field may be 0.

    int32_t elementary_PID;                     //!< stream PID, transport streams only
    uint8_t ID;                                 //!< elementary stream ID, program/PVA stream only
    uint8_t sub_ID;                             //!< sub ID of a DVD private 1 stream, DVD only

    int32_t ES_info_length;                     //!< length in bytes of the descriptors that follow in the next field
    uint8_t *descriptors;                       //!< descriptors present in the PMT or PSM for this stream

    uint8_t PES_scrambled;                      //!< the value of the PES_scrambling_control field of the PES header, MPEG-2 transport/program streams only

                                                //!< A non-zero value indicates the PES packet is scrambled (encrypted).

    uint8_t ts_scrambled;                       //!< the value of the transport_scrambling_control field of the transport packet, transport streams only

                                                //!< A non-zero value indicates the transport payload is scrambled (encrypted).

    mc_stream_format_t format;                  //!< describes the elementary stream using the structure and enumerations in the mcmediatypes.h file

    int64_t first_pts;                          //!< the first PTS of the elementary stream in 27MHz units
    int64_t duration;                           //!< the duration of the stream in 27MHz units

    uint8_t contains_stills;                    //!< set to 1 if a DVD video stream contains still frames

                                                //!< The number of frames in the video stream is not equal to the duration / frame rate.

    uint32_t num_views;                         //!< number of views in a 'muxed' MVC stream
    uint32_t view_id;                           //!< view ID for MVC substream

    uint8_t scte35_cue_stream_type;             //!< SCTE-35 cue_stream_type from any cue_identifier_descriptor
                                                //!< defaults to 1 if no descriptor is present
    uint8_t scte35_component_tag;               //!< SCTE-35 component_tag from any stream_identifier_descriptor

    uint8_t reserved[53];                       //!< reserved for future use

} mp2dmux_stream_format_t;

/*!
 @brief This structure is used to retrieve information about a program (title).
*/
typedef struct mp2dmux_program_info_s
{
    int32_t program_number;                     //!< program number as specified in the PMT, transport streams only
    int32_t PID;                                //!< network PID if program_number is 0, else program map PID, transport stream only
    int32_t PCR_PID;                            //!< PID that carries the PCR for this program, transport stream only
    int32_t stream_count;                       //!< number of elementary streams in this program
    int32_t dvd_num_angles;                     //!< number of angles in a VTS title, only used for .IFO files
    int32_t dvd_lu_num;                         //!< depreciated, use title number

                                                //!< The DVD title number is the language unit number.

    uint8_t section_syntax_indicator;           //!< section syntax indicator field of the PMT, transport streams only
    uint8_t private_indicator;                  //!< private indicator field of the PMT, transport streams only

    int32_t program_info_length;                //!< length in bytes of the descriptors that follow in the next field
    uint8_t *descriptors;                       //!< program descriptors present in the PMT or PSM

    int64_t duration;                           //!< the duration of the program in 27MHz units

    uint8_t reserved[64];                       //!< reserved for future use

} mp2dmux_program_info_t;

/*!
 @brief This structure is used to retrieve information about the file in file mode.
*/
typedef struct mp2dmux_file_info_s
{
    int32_t program_count;                      //!< number of programs (titles) in the input stream

                                                //!< For program and PVA streams it will always be 1.
                                                //!< For IFO files and transport streams it will be >= 1.

    int32_t system_stream_flag;                 //!< input stream type, one of the STREAM_xxx defines in mcdefs.h
    int64_t file_size;                          //!< total size of the input file(s)

    int32_t transport_stream_id;                //!< transport stream ID from the PAT, transport stream only

    int32_t ts_packet_size;                     //!< size of transport stream packets, usually 188, transport stream only
    uint32_t flags;                             //!< one of the \ref MP2DMUX_FILE "MP2DMUX_FILE_xxx flags"

    uint8_t reserved[64];                       //!< reserved for future use

} mp2dmux_file_info_t;


/*!
 @brief This structure is used to retrieve an index entry.
*/
typedef struct mp2dmux_index_info_s
{
    int64_t file_pos;                           //!< index file position of the entry
    int64_t PTS;                                //!< index PTS in 27MHz units
    int64_t DTS;                                //!< deprecated, no longer used!
    uint8_t pic_type;                           //!< picture type of the entry (I = 1, P = 2, B = 3)
    int16_t temp_ref;                           //!< temporal reference of the entry
    uint8_t flags;                              //!< any of the \ref MP2DMUX_xxx_FLAG

} mp2dmux_index_info_t;


/*!
 @brief These settings are used to create a demuxer instance and are used in both file and streaming mode.
*/
typedef struct mp2dmux_settings_s
{
    int32_t find_stream_limit;                  //!< number of megabytes of the input file to search for elementary streams.

                                                //!< A value of 0 means use the default built into the demuxer

    int32_t find_stream_options;                //!< indicates what streams to look for when searching the input file for stream information

                                                //!< Can be one of the \ref MP2DMUX_FIND "MP2DMUX_FIND_xxx defines above"
    //! @name external IO stuff
    //!@{
    int32_t use_external_io;                    //!< indicates that the app will provide the I/O for the demuxer, set to 1 to enable external I/O
    mp2_external_open_callback   external_open_callback;  //!< external open callback pointer
    mp2_external_open_callbackW  external_open_callbackW; //!< UNICODE external open callback pointer
    mp2_external_seek_callback   external_seek_callback;  //!< external seek callback pointer
    mp2_external_read_callback   external_read_callback;  //!< external read callback pointer
    mp2_external_close_callback  external_close_callback; //!< external close callback pointer
    void *ext_app_ptr;                          //!< an app pointer for external I/O mode, it is passed to the external callbacks
    int64_t file_length;                        //!< the length of the input file, used for splitter and external I/O modes only

    //!@}

    uint8_t use_all_frames_flag;                //!< set to 1 to use all frames/samples in the file, including those that are missing, should be used with caution

                                                //!< If there is a gap in the timestamps of a stream, frames/samples will be generated to fill the gap.

    uint8_t info_only_flag;                     //!< set to 1 to determine the number and type of streams available only, the durations of the streams are not determined and will be 0

                                                //!< In some files this can be much quicker than a normal open.

    uint8_t user_no_seeking_flag;               //!< set to 1 if the duration and seeking (except to frame/sample 0) is not required

                                                //!< Use this if you just want to play file from the start,
                                                //!< opening files is quicker and problem files may be opened that
                                                //!< cannot be opened in normal mode.
                                                //!< The duration for all streams will be 0 and seek functions will return an error
                                                //!< if the seek position is not 0.

    uint8_t fixed_frame_size_flag;              //!< set to 1 to inform the demuxer the input video elementary stream is D10/AVCIntra type video

                                                //!< Used if the GOP timecodes in the stream are all zeros.

    uint8_t index_mode_required_action;         //!< this flag specifies the action to take if the demuxer requires index mode to open a file and index_mode_flag = 0:

                                                //!< 0 = use index mode to open the file<br>
                                                //!< 1 = return an error<br>
                                                //!< 2 = call the prompt_index_mode callback
                                                 
    uint8_t index_mode_flag;                    //!< open the file in index mode

    uint8_t additional_GOP_search;              //!< search additional GOP's for pulldown type video

                                                //!< Some files may have non-pulldown content for several GOP's before the pulldown content starts.

    //! @name options when opening a DVD VMG/VTS IFO fileset
    //!@{
    int32_t dvd_get_vts_menu;                   //!< indicates the VTS menu is to be opened (1) instead of the VTS contents (0)

                                                //!<Only used for IFO files.

    //!@}

    //! @name the next three fields are depreciated
    //!@{
    int32_t dvd_title_num;                      //!< depreciated, use mp2dmux_stream_settings_t.program_idx to select the DVD title
    int32_t dvd_language;                       //!< depreciated, use mp2dmux_stream_settings_t.program_idx to select the DVD language (LU number)
    int32_t dvd_angle;                          //!< depreciated, use mp2dmux_seek_settings_t.dvd_angle to specify the DVD angle

    //!@}

    mc_external_io_t *p_external_io;            //!< can be used for external i/o instead of the above fields

                                                //!< If non NULL and use_external_io = 1, this will be used instead of the above.
                                                //!< It uses the mc_external_io_t structure defined in mcdefs.h and is common among the demuxers.
                                                //!< This method overcomes a problem with the old method,
                                                //!< it uses a void* pointer to represent the file handle instead of a long.

    bufstream_tt *p_sec_idx_bs;                 //!< secondary index bufstream, for compatibility

    uint8_t es_index_mode_flag;                 //!< open elementary streams in index mode

	uint8_t is_growing_file;                    //!< @brief Treat file as a 'growing file'. For more information on growing files support see the documentation.

    uint8_t reserved[54];                       //!< reserved for future use

} mp2dmux_settings_t;


/*!
 @brief These settings are used to create a parser in file mode.
*/
typedef struct mp2dmux_parser_settings_s
{
    int32_t requested_buffer_size;              //!< the requested size in bytes the app wishes the parser to use for bitstream buffers

    mp2_dvd_PCI_callback   dvd_PCI_callback;    //!< a pointer to a PCI callback function

                                                //!< When a DVD PCI packet is demuxed it is sent to the app via this callback.

    mp2_dvd_DSI_callback   dvd_DSI_callback;    //!< a pointer to a DSI callback function

                                                //!< When a DVD DSI packet is demuxed it is sent to the app via this callback.

    void *parser_app_ptr;                       //!< an app pointer for the DVD PCI and DSI callbacks, it is passed to the callbacks

    uint8_t remove_headers_flag;                //!< set to 1 to remove AnnexB AVC headers or ADTS AAC headers if present from the output elementary stream
                                                //!< NOTE: this setting is ignored if mp2dmux_stream_settings_t.no_parsing_flag is enabled
    uint8_t format_pcm_output_flag;             //!< set to 1 to output DVD LPCM, HDMV LPCM and AES3 302M LPCM audio streams as basic PCM samples, else output in the native format
                                                //!< NOTE: this setting is ignored if mp2dmux_stream_settings_t.no_parsing_flag is enabled
    uint8_t output_native_aes3_337m;            //!< set to 1 to output native AES3 337M data streams, else output unwrapped data
                                                //!< NOTE: if mp2dmux_stream_settings_t.no_parsing_flag is enabled, the output is always the native AES3 337M data stream

    uint8_t reserved[63];                       //!< reserved for future use

} mp2dmux_parser_settings_t;

/*!
  @brief Stream settings. This structure is used when adding a stream to a parser.
*/
typedef struct mp2dmux_stream_settings_s
{
    int32_t program_idx;                        //!< the program idx of the desired stream (a zero based program index requested by the app)

                                                //!< May not be the same as the program number!
                                                //!< The range is 0 .. mp2dmux_file_info_t.program_count - 1.
                                                //!< For program and PVA streams, this will always be 0.

    int32_t stream_idx;                         //!< the stream idx of the desired stream (a zero based stream index requested by the app)

                                                //!< The range is 0 .. mp2dmux_program_info_t.stream_count – 1.

    bufstream_tt *bs;                           //!< a pointer to the output bufstream for this elementary stream

    uint8_t pull_mode_flag;                     //!< set to 1 if the app will call auxinfo to get data, otherwise the app will call mp2DemuxPush
    uint8_t mvc_reassemble_flag;                //!< reassemble MVC base and substreams and output as a single stream
                                                //!< NOTE: this setting is ignored if no_parsing_flag is enabled
    uint8_t output_pes_flag;                    //!< set to 1 to output the PES packetized stream instead of the elementary stream, only used
                                                //!< for MPEG-2 program or transport streams
    uint8_t no_parsing_flag;                    //!< set to 1 to demux this stream without parsing. This mode is faster
                                                //!< but output data chunks are not complete frames, and less info is
                                                //!< available for the data chunk (video frame type, etc)

    uint8_t reserved[61];                       //!< reserved for future use

} mp2dmux_stream_settings_t;


//! @anchor MP2DMUX_PSI_FLAG
#define MP2DMUX_PSI_FLAG_ENTIRE_PACKET          0x00000001    //!< TS only, send the entire packet to the callback, including any TS headers
                                                              //!< Default is to send only the data after TS headers.

/*!
  @brief This structure is used to initialize a PSI callback.
  PSI callback settings can actually be used for any PID/ID, not just PSI packets.
*/
typedef struct mp2dmux_PSI_callback_settings_s
{
    int32_t parser_num;                         //!< the parser number for the callback, not used in streaming mode
    int32_t id;                                 //!< stream pid or id for the callback

    mp2_PSI_callback PSI_callback;              //!< pointer to a callback routine, set to NULL to use PSI_callback_ex 
    void *PSI_app_ptr;                          //!< user pointer passed back to the PSI callback

    uint32_t flags;                             //!< zero or more of the \ref MP2DMUX_PSI_FLAG "MP2DMUX_PSI_FLAG_xxx flags"

    mp2_PSI_callback_ex PSI_callback_ex;        //!< pointer to an extended callback routine

    uint8_t reserved[56];                       //!< reserved for future use

} mp2dmux_PSI_callback_settings_t;


/*!
 @brief This structure is used to seek by time.
*/
typedef struct mp2dmux_seek_settings_s
{
    int32_t parser_num;                         //!< the parser number for the seek
    int32_t program_idx;                        //!< the program idx for the seek (a zero based program index for the seek)

                                                //!< The range is 0 .. mp2dmux_file_info_t.program_count - 1.
                                                //!< For program and PVA streams, this will always be 0.

    int32_t stream_idx;                         //!< the stream idx for the seek (a zero based stream index for the seek)

                                                //!< The range is 0 .. mp2dmux_program_info_t.stream_count – 1.

    int64_t seek_time;                          //!< input/output, the time to seek to in 27Mhz units

                                                //!< As output it is the actual time of the target frame.
                                                //!< This field is used for both input and output.
                                                //!< On input it is the time to seek to in 27Mhz units,
                                                //!< on output it is the actual time of the seek.
                                                //!< This may be different than the input if the input is not an exact timestamp.

    int64_t ref_time;                           //!< output, the resulting time in 27Mhz units

                                                //!< This field contains the reference time where the stream is positioned after a seek.
                                                //!< For video streams this will be at an I frame,
                                                //!< use the seek_time – ref_time to determine when the target frame is reached.

    //! @name these do not change the file position, the merely return the appropriate time
    //!@{
    uint8_t seek_next_key_frame;                //!< return the time of the next key frame based on seek_time

                                                //!< Set this field to 1 to get the time of the next key frame from seek_time in a video stream.
                                                //!< This will not change the position of the stream,
                                                //!< it will just report the next key frame value in the seek_time field.

    uint8_t seek_prev_key_frame;                //!< return the time of the previous key frame base on seek_time

                                                //!< Set this field to 1 to get the time of the previous key frame from seek_time in a video stream.
                                                //!< This will not change the position of the stream,
                                                //!< it will just report the previous key frame value in the seek_time field.

    //!@}

    uint8_t avc_seek_start_point;               //!< specify the method of reference frame seeking in AVC video stream

                                                //!< 0 = seek to the nearest IDR or recovery point frame before the target frame,
                                                //!< decode from IDR or recovery point to target frame (default)
                                                //!<
                                                //!< 1 = seek to the nearest frame with I slices before the target
                                                //!< frame, decode from this frame to target frame
                                                //!<
                                                //!< other values (> 1) indicate the number of frames before the target
                                                //!< frame where decoding is to start. The actual starting frame
                                                //!< will be the nearest frame with I slices before the value
                                                //!< specified.
                                                //!< 
                                                //!< For example, if avc_seek_start_point = 30 and a seek is performed to
                                                //!< frame 130, the demuxer will backup to frame 100 (130 - 30) to start decoding
                                                //!< until it reaches frame 130. If frame 100 does not contain any I slices
                                                //!< but frame 98 does, the decoding will actually start at frame 98 instead
                                                //!< of frame 100.

    uint8_t dvd_angle;                           //!< the new DVD angle to demux

                                                 //!< Set to 0 for no change, else 1 .. mp2dmux_program_info_s.dvd_num_angles for the new angle.

    uint8_t reserved[63];                        //!< reserved for future use

} mp2dmux_seek_settings_t;


//! @anchor MP2DMUX_SM_INIT_FLAG
#define MP2DMUX_SM_INIT_FLAG_WEAK_DETECTION     0x00000001    //!< TS only, do weak stream detection using only the PMT. Normal stream detection
                                                              //!< will look for the actual stream starts before completing. Use this flag
                                                              //!< to force detection using only the TS PMT.

/*!
 @brief This structure is used to initialize streaming mode.
*/
typedef struct mp2dmux_sm_init_settings_s
{
    bufstream_tt *input_bs;                     //!< a pointer to the input bufstream for the source stream

    uint32_t flags;                             //!< zero or more of the \ref MP2DMUX_SM_INIT_FLAG "MP2DMUX_SM_INIT_FLAG_xxx flags"

    uint8_t reserved[60];                       //!< reserved for future use

} mp2dmux_sm_init_settings_t;


/*!
 @brief Streaming mode stream settings.
 This structure is used to add a stream in streaming mode.
*/
typedef struct mp2dmux_sm_stream_settings_s
{
    bufstream_tt *bs;                           //!< pointer to the output bufstream for the desired elementary stream

    int32_t program_idx;                        //!< the program idx of the desired stream (a zero based program index requested by the app)

                                                //!< Not the same as the program number!
                                                //!< The range is 0 .. mp2dmux_file_info_t.program_count - 1.
                                                //!< For program and PVA streams, this will always be 0.

    int32_t stream_idx;                         //!< the stream idx of the desired stream (a zero based stream index requested by the app)

                                                //!< The range is 0 .. mp2dmux_program_info_t.stream_count – 1.

    uint8_t mvc_reassemble_flag;                //!< reassemble MVC base and substreams and output as a single stream
                                                //!< NOTE: this setting is ignored if no_parsing_flag is enabled
    uint8_t output_pes_flag;                    //!< set to 1 to output the PES packetized stream instead of the elementary stream, only used
                                                //!< for MPEG-2 program or transport streams
    uint8_t no_parsing_flag;                    //!< set to 1 to demux this stream without parsing. This mode is faster
                                                //!< but output data chunks are not complete frames, and less info is
                                                //!< available for the data chunk (video frame type, etc)

    uint8_t reserved[61];                       //!< reserved for future use

} mp2dmux_sm_stream_settings_t;


/*!
@brief This structure contains TS adaptation field values, used with PSI callbacks.
The fields contain the same named bitstream values. 
*/
typedef struct mp2dmux_adaptation_field_s
{
    uint8_t adaptation_field_length;

    //!< the rest of the structure is only valid if adaptation_field_length > 0
    uint8_t discontinuity_indicator;
    uint8_t random_access_indicator;
    uint8_t elementary_stream_priority_indicator;
    uint8_t PCR_flag;
    uint8_t OPCR_flag;
    uint8_t splicing_point_flag;
    uint8_t transport_private_data_flag;
    uint8_t adaptation_field_extension_flag;

    //!< valid if PCR_flag != 0
    uint64_t program_clock_reference_base;
    uint16_t program_clock_reference_extension;

    //!< valid if OPCR_flag != 0
    uint64_t original_program_clock_reference_base;
    uint16_t original_program_clock_reference_extension;

    //!< valid if splicing_point_flag != 0
    uint8_t splice_countdown;

    //!< valid if transport_private_data_flag != 0
    uint8_t transport_private_data_length;
    uint8_t private_data_byte[255];

    //!< valid if adaptation_field_extension_flag != 0
    uint8_t adaptation_field_extension_length;

    //!< valid if adaptation_field_extension_length > 0
    uint8_t ltw_flag;
    uint8_t piecewise_rate_flag;
    uint8_t seamless_splice_flag;

    //!< valid if ltw_flag != 0
    uint8_t ltw_valid_flag;
    uint16_t ltw_offset;

    //!< valid if piecewise_rate_flag != 0
    uint32_t piecewise_rate;

    //!< valid if seamless_splice_flag != 0
    uint8_t splice_type;
    uint8_t DTS_next_AU_upper;
    uint16_t DTS_next_AU_mid;
    uint16_t DTS_next_AU_lower;

    uint8_t reserved[64];

} mp2dmux_adaptation_field_t;


/*!
@brief This structure contains TS packet header values, used with PSI callbacks
The fields contain the same named bitstream values.
*/
typedef struct mp2dmux_TS_packet_s
{
    uint8_t transport_error_indicator;
    uint8_t payload_unit_start_indicator;
    uint8_t transport_priority;
    uint16_t PID;
    uint8_t transport_scrambling_control;
    uint8_t adaptation_field_control;
    uint8_t continuity_counter;

    //!< valid if adaptation_field_control > 1 and MP2DMUX_PSI_FLAG_ENTIRE_PACKET was NOT used for the callback
    mp2dmux_adaptation_field_t adaptation_field;

    uint8_t reserved[64];

} mp2dmux_TS_packet_t;


/*!
@brief This structure contains TS packet info, used with PSI callbacks
*/
struct mp2dmux_TS_packet_info_s
{
    mp2dmux_TS_packet_t TS_packet;      //!< TS packet header fields

    void *buf;                          //!< pointer to the TS packet data
    int32_t buf_len;                    //!< length of data in buf
    int64_t PCR;                        //!< PCR of the packet
    uint8_t PCR_synth_flag;             //!< if 0 the PCR value is an actual value from the bitstream
                                        //!< else the PCR value is a synthesized value

    uint8_t reserved[64];

};

/*!
@brief This structure is interface for external descrambler which can be used by demuxer. This is for TS scrambling.
@note This is not stable API yet.
*/
struct mp2dmux_CA_TS_Decrypt_s
{
    void * engine; //!< Pointer of implementation
    void (*decrypt)(void* engine, uint8_t* buf, uint32_t len); //!< Method which will be called
};

typedef int32_t(MC_EXPORT_API* demux_mp2_prompt_index_mode_t)(context_t);
typedef int32_t(MC_EXPORT_API* demux_mp2_yield_t)(context_t);
typedef struct mp2dmux_CA_TS_Decrypt_s* (MC_EXPORT_API *demux_mp2_get_ca_decryptor_t)(context_t context, mp2dmux_program_info_t* prog, mp2dmux_stream_format_t* stream);

struct callbacks_demux_mp2_t
{
    demux_mp2_prompt_index_mode_t prompt_index_mode;
    demux_mp2_yield_t yield;
    demux_mp2_get_ca_decryptor_t get_ca_decryptor;
    void* reserved[16];
};

#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif


//! @name crypto functions
//!@{
/*!
   @brief Call to create instance of decryptor. No dynamic keys or IV are supported.

@param [in] crypto_algorithm     AES-128 CBC mode the only supported algorithm
@param [in] block_termination_mode     SCTE52 is the only supported mode

@param [in] key_length   Size in bytes (16 bytes for AES128) of key and IVs
@param [in] key   C array with the static key
@param [in] iv1   C array with initialization vector (whitener1 according to SCTE52)
@param [in] iv2   C array with additional initialization vector (whitener2 according to SCTE52)

@return      a pointer to an mp2dmux_CA_TS_Decrypt_s instance if successful, NULL if unsuccessful
*/
mp2dmux_CA_TS_Decrypt_s * MC_EXPORT_API mp2DemuxCreateStaticTSDecryptor(enum eCA_TS_Mode crypto_algorithm, enum eCA_TS_BlockTermMode block_termination_mode,
                                                                        uint8_t *key, uint32_t key_length, uint8_t *iv1, uint32_t iv1_length, uint8_t *iv2,
                                                                        uint32_t iv2_length);

/*!
@brief Call to destroy decryptor.

@param [in] engine     a pointer to mp2dmux_CA_TS_Decrypt_s  object created with a mp2DemuxCreateStaticTSDecryptor call

@return              none
*/
void MC_EXPORT_API mp2DemuxDestroyStaticTSDecryptor(struct mp2dmux_CA_TS_Decrypt_s* engine);

//!@}

//! @name general functions
//!@{

/*!
   @brief Call to create and initialize an demuxer instance.
   The app pointer is passed back to all resource functions.
   
   @param [in] callbacks        a pointer to a structure with resource callbacks definition
   @param [in] demux_callbacks  a pointer to a structure with demultiplexer-specific callbacks definition
   @param [in] set              a pointer to a filled in mp2dmux_settings_t structure
   
   @return                  a pointer to an mp2dmux_tt instance if successful, NULL if unsuccessful
*/
mp2dmux_tt * MC_EXPORT_API mp2DemuxNew(callbacks_t * callbacks, callbacks_demux_mp2_t * demux_callbacks, mp2dmux_settings_t *set);


/*!
    @brief Call to free a demuxer instance.

    @param [in] demuxer  a pointer to a mp2dmux_tt object created with a mp2DemuxNew call

    @return              none
*/
void MC_EXPORT_API mp2DemuxFree(mp2dmux_tt *demuxer);

/*!
    @brief Call to get information about the input file or stream.
    This will return the number of programs (titles) in the file.
    For program streams and PVA files this will always be 1.
    For IFO and transport streams it may be > 1.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] file_info   a pointer to a mp2dmux_file_info_t structure

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxGetFileInfo(mp2dmux_tt *demuxer, mp2dmux_file_info_t *file_info);


/*!
    @brief Call to get information about the specified program.
    This will return the number of elementary streams in the program.
    There may be no elementary streams in a program.

    @param [in] demuxer       a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] program_info  a pointer to a mp2dmux_program_info_t structure
    @param [in] program_idx   a zero-based program index number

    @return                   0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxGetProgramInfo(mp2dmux_tt *demuxer, mp2dmux_program_info_t *program_info, int32_t program_idx);


/*!
    @brief Call to get format information about the specified stream in the specified program

    @param [in] demuxer      a pointer to a demuxer instance
    @param [in] stream_info  a pointer to a mp2dmux_stream_format_t structure
    @param [in] program_idx  a zero-based program index number
    @param [in] stream_idx   a zero-based stream index number

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxGetStreamFormat(mp2dmux_tt *demuxer, mp2dmux_stream_format_t *stream_format, int32_t program_idx, int32_t stream_idx);


/*!
    @brief Call to add a PSI callback to a parser.
    Note this can be used to get access to any TS or PS packet, not just PSI packets.

    @param [in] demuxer  a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] set      a pointer to a mp2dmux_PSI_callback_settings_t structure

    @return              0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxAddPSICallback(mp2dmux_tt *demuxer, mp2dmux_PSI_callback_settings_t *set);


/*!
    @brief Call to remove a PSI callback from a parser.

    @param [in] demuxer  a pointer to a demuxer instance
    @param [in] set      a pointer to an mp2dmux_PSI_callback_settings_t structure

    @return              0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxRemovePSICallback(mp2dmux_tt *demuxer, mp2dmux_PSI_callback_settings_t *set);

//!@}


//! @name file mode functions
//!@{

/*!
    @brief Call to open a file in file mode.
    This can be a DVD IFO as well as an MPEG or PVA file.

    @param [in] demuxer   a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] idx_bs    a pointer to an index bufstream to use when opening the file, can be NULL
    @param [in] filename  a pointer to a filename to open

    @return               0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxOpen(mp2dmux_tt *demuxer, bufstream_tt *idx_bs, char *filename);

/*!
    @brief Call to open a file in file mode, UNICODE version.
    This can be a DVD IFO as well as an MPEG or PVA file.

    @param [in] demuxer   a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] idx_bs    a pointer to an index bufstream to use when opening the file, can be NULL
    @param [in] filename  a pointer to a UNICODE filename to open

    @return               0 if successful, non-zero if unsuccessful
*/
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__native_client__) && !defined(__vxworks) && !defined(__QNX__)

int32_t MC_EXPORT_API mp2DemuxOpenW(mp2dmux_tt *demuxer, bufstream_tt *idx_bs, wchar_t *filename);

#else // !defined(__APPLE__) && !defined(__linux__) && !defined(__vxworks) && !defined(__QNX__)

// this version will currently return an error!
int32_t MC_EXPORT_API mp2DemuxOpenW(mp2dmux_tt *demuxer, bufstream_tt *idx_bs, uint16_t *filename);

#endif // !defined(__APPLE__) && !defined(__linux__) && !defined(__vxworks) && !defined(__QNX__)


/*!
    @brief Call to close a file in file mode.
    This will also free any remaining parsers.

    @param [in] demuxer a pointer to a mp2dmux_tt object created with a mp2DemuxNew call

    @return             none
*/
void MC_EXPORT_API mp2DemuxClose(mp2dmux_tt *demuxer);


/*!
    @brief Call to create and initialize a parser instance in file mode.

    @param [in] demuxer  a pointer to a demuxer instance
    @param [in] set      a pointer to a mp2dmux_parser_settings_t structure

    @ return             a non-zero parser number if successful, 0 if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxNewParser(mp2dmux_tt *demuxer, mp2dmux_parser_settings_t *set);


/*!
    @brief Call to free a parser instance inf file mode.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call

    @return                 none
*/
void MC_EXPORT_API mp2DemuxFreeParser(mp2dmux_tt *demuxer, int32_t parser_num);


/*!
    @brief Call to add an output bufstream for the specified stream in file mode.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call
    @param [in] set         a pointer to a filled mp2dmux_stream_settings_t structure

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxAddStream(mp2dmux_tt *demuxer, int32_t parser_num, mp2dmux_stream_settings_t *set);


/*!
    @brief Call to remove an output bufstream in file mode.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call
    @param [in] set         a pointer to a filled mp2dmux_stream_settings_t structure

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxRemoveStream(mp2dmux_tt *demuxer, int32_t parser_num, mp2dmux_stream_settings_t *set);


/*!
    @brief Call to position a parser at a particular file position in file mode.
    The demuxer will find the nearest start code and start demuxing there.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call
    @param [in] file_pos    the byte file position to seek to

    @return                 0 if successful, 2 if EOF, 1 if an error occurs
*/
int32_t MC_EXPORT_API mp2DemuxSeekPos(mp2dmux_tt *demuxer, int32_t parser_num, int64_t file_pos);


/*!
    @brief Call to position a parser at a particular time in file mode.
    The demuxer will find the nearest reference frame and start demuxing there.

    @param [in] demuxer  a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] set      a pointer to a filled mp2dmux_seek_settings_t structure

    @return    0 if successful, 2 if EOF, 1 if an error occurs
*/
int32_t MC_EXPORT_API mp2DemuxSeekTime(mp2dmux_tt *demuxer, mp2dmux_seek_settings_t *set);


/*!
    @brief Call to push some source stream data through the demuxer in file mode.
    The return value is the number of source stream bytes consumed by the demuxer.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call

    @return                 non-zero if successful, the number of bytes consumed from the source stream,<br>
                            0 if EOF,<br>
                            -2 if EOF in growing file mode, wants more data, call mp2DemuxUpdateFilesize. <br>
                            -1 if an error occurs
*/
int32_t MC_EXPORT_API mp2DemuxPush(mp2dmux_tt *demuxer, int32_t parser_num);


/*!
    @brief Call to re-open the parsers file handle in file mode.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxWakeParser(mp2dmux_tt *demuxer, int32_t parser_num);


/*!
    @brief Call to close the actual file of a parser, the parser
    is left intact and can be re-opened with the wake function

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] parser_num  a parser number from a mp2DemuxNewParser call

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxSleepParser(mp2dmux_tt *demuxer, int32_t parser_num);


/*!
    @brief Call to update the size of the source file when using growing file or external I/O modes 
    (if file is growing). File mode only and not available in IFO or MZL modes.
    
    For growing file mode, the file_size parameter is ignored unless it is set to -1. If
    file_size != -1, the demuxer determines the new size of the growing file itself and updates
    the stream durations. If file_size = -1, the demuxer considers the file complete and will 
    finalize the stream durations. Further calls to mp2DemuxUpdateFilesize will return an error.

    For external I/O mode the file_size parameter must be > 0.

    @param [in] demuxer     a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] reserved    reserved for future use
    @param [in] file_size   The new file size if external i/o mode. 
                            If in growing file mode set to -1 to reset growing file mode. 

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxUpdateFilesize(mp2dmux_tt *demuxer, int32_t reserved, int64_t file_size);

//!@}


//! @name streaming mode functions
//!@{

/*!
    @brief Call to initialize streaming mode.

    @param [in] demuxer   a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] set       a pointer to a filled mp2dmux_sm_init_settings_t structure

    @return               0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxStreamModeInit(mp2dmux_tt *demuxer, mp2dmux_sm_init_settings_t *set);


/*!
    @brief Call to add a stream in streaming mode.

    @param [in] demuxer  a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] set      a pointer to a filled mp2dmux_sm_stream_settings_t structure

    @return              0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxStreamModeAddStream(mp2dmux_tt *demuxer, mp2dmux_sm_stream_settings_t *set);


/*!
    @brief Call to flush a streaming demuxing session.

    @param [in] demuxer  a pointer to a demuxer instance

    @return              0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxStreamModeFlush(mp2dmux_tt *demuxer);


/*!
    @brief Call to finish a streaming demuxing session (close streaming mode).

    @param [in] demuxer  a pointer to a demuxer instance
    @param [in] abort    set to 1 to abort demuxing or 0 to finish normally

    @return              0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxStreamModeDone(mp2dmux_tt *demuxer, int32_t abort);

//! @}

//! @name index functions
//!@{

/*!
    @brief Call to create an index of the input stream.

    @param [in] demuxer       a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] get_pic_info  set to a 1 to determine picture type and temp ref, mpeg and AVC only

    @return                   0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxCreateIndex(mp2dmux_tt *demuxer, int32_t get_pic_info);


/*!
    @brief Call to save an index

    @param [in] demuxer  a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] idx_bs   a pointer to a bufstream to receive the index

    @return              0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxSaveIndex(mp2dmux_tt *demuxer, bufstream_tt *idx_bs);


/*!
    @brief Call to get the number of index entries for a stream.

    @param [in] demuxer      a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] program_idx  a zero based program index
    @param [in] stream_idx   a zero based stream index

    @return                  number of entries if successful, 0 if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxGetIndexCount(mp2dmux_tt *demuxer, int32_t program_idx, int32_t stream_idx);


/*!
    @brief Call to get an index entry for a stream.

    @param [in] demuxer      a pointer to a mp2dmux_tt object created with a mp2DemuxNew call
    @param [in] program_idx  a zero based program index
    @param [in] stream_idx   a zero based stream index
    @param [in] info         a pointer to a mpgdmux_index_info_t to receive the index info
    @param [in] idx          the index entry number to fetch

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mp2DemuxGetIndexEntry(mp2dmux_tt *demuxer, int32_t program_idx, int32_t stream_idx, mp2dmux_index_info_t *info, uint32_t idx);


/*!
    @brief Call to free a created index.

    @param [in] demuxer  a pointer to a mp2dmux_tt object created with a mp2DemuxNew call

    @return     none
*/
void MC_EXPORT_API mp2DemuxFreeIndex(mp2dmux_tt *demuxer);

//!@}

/*!
    @brief Call to get extended API function.

    @param [in] func  function ID

    @return           function pointer or NULL
*/
APIEXTFUNC MC_EXPORT_API mp2DemuxGetAPIExt(uint32_t func);

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////
//
// old mpegDemuxXXX API, this will eventually be depreciated in favor of the above API
//
/////////////////////////////////////////////////////////////////////////////////////////


//! @name find stream options (old API)
//! @anchor MPGDMUX_FIND
//!@{
#define MPGDMUX_FIND_NONE               0       //!< do the default search (all streams within the MB limit)
#define MPGDMUX_FIND_VIDEO              1       //!< stop looking after 1 video stream is found
#define MPGDMUX_FIND_AUDIO              2       //!< stop looking after 1 audio stream is found
#define MPGDMUX_FIND_VIDAUD             3       //!< stop looking after 1 video and 1 audio are found
#define MPGDMUX_FIND_SUBPIC             4       //!< stop looking after 1 subpic stream is found
#define MPGDMUX_FIND_VIDAUDSP           7       //!< stop looking after 1 video, 1 audio and 1 subpic are found
#define MPGDMUX_FIND_ALL                8       //!< do not stop looking until the MB limit is reached

//!@}

//! @name sdk_stream_type defines, general stream types reported
//! @anchor MPGDMUX
//!@{
#define MPGDMUX_UNKNOWN                 0x00    //!< unknown type to this SDK
#define MPGDMUX_MPEG_VIDEO              0x01    //!< MPEG or H.264 video
#define MPGDMUX_MPEG_AUDIO              0x02    //!< MPEG audio including AAC
#define MPGDMUX_AC3_AUDIO               0x03    //!< AC3 audio
#define MPGDMUX_AES_302M_AUDIO          0x05    //!< AES 302M PCM audio
#define MPGDMUX_DVD_LPCM_AUDIO          0x04    //!< LPCM audio, DVD style
#define MPGDMUX_DTS_AUDIO               0x06    //!< DTS audio
#define MPGDMUX_SDDS_AUDIO              0x07    //!< SDDS audio
#define MPGDMUX_DVD_SUBPIC              0x08    //!< DVD style subpic
#define MPGDMUX_HDMV_LPCM_AUDIO         0x09    //!< HDMV LPCM audio
#define MPGDMUX_DVD_PCI                 0x0A    //!< DVD PCI
#define MPGDMUX_DVD_DSI                 0x0B    //!< DVD DSI
#define MPGDMUX_EAC3_AUDIO              0x0C    //!< Enhanced AC3 audio
#define MPGDMUX_VC1_VIDEO               0x0D    //!< VC1 video
#define MPGDMUX_AES3_337M_DATA          0x0E    //!< AES3 SMPTE 337M data
#define MPGDMUX_HEVC_VIDEO              0x0F    //!< HEVC video

//!@}


//! maximum number of programs handled by the demuxer
#define MPGDMUX_MAX_PROGRAMS            32
//! maximum number of streams per program handled by the demuxer
#define MPGDMUX_MAX_STREAMS             64

//! @name file info flags (old API)
//! @anchor MPGDMUX_FILE
//!@{
#define MPGDMUX_FILE_AC3_TIMECODE       0x01

//!@}

//! @name index info flags (old API)
//! @anchor MPGDMUX_xxx_FLAG
//!@{
#define MPGDMUX_SEQ_FLAG                0x01    //!< Sequence hdr present
#define MPGDMUX_GOP_FLAG                0x02    //!< GOP hdr present
#define MPGDMUX_IDR_FLAG                0x04    //!< h264 IDR flag

//!@}

/*!
    @brief Called to seek in the source file in splitter mode.

    @param split_app_ptr  an application supplied pointer that will be passed back
    @param position       file position to seek to

    @return           0 if OK, 1 if fault
*/
typedef int (*mpg_splitter_seek_callback)(void* split_app_ptr, int64_t position);

/*!
    @brief Called to read data in splitter file mode.
    Read bufferSize bytes from file into buffer.

    @param split_app_ptr  an application supplied pointer that will be passed back
    @param buffer         buffer to be used for data
    @param bufferSize     buffer size

     @return number of bytes read, 0 if fault
*/
typedef int (*mpg_splitter_read_callback)(void* split_app_ptr, uint8_t *buffer, int bufferSize);


//! @name external i/o function prototypes
//! @note If these are used, the mpgdmux_settings.file_length field MUST be filled in
//!        with the correct file size by the app.

//! @{

/*!
    @brief Called to open a file.
    The return value will be passed back to the seek, read and close functions.

    @param ext_app_ptr  the ext_app_ptr field in the mpgdmux_settings structure is passed back here
    @param fileName     if a filename was passed to mpgdmux, it is returned here

    @return             the file handle as a long or -1 if an error occurs
*/
#ifndef UNDER_CE
typedef long (*mpg_external_open_callback)(void *ext_app_ptr, char *fileName);
#else
typedef FILE* (*mpg_external_open_callback)(void *ext_app_ptr, char *fileName);
#endif

/*!
    @brief Called to open a file, UNICODE version.
    The return value will be passed back to the seek, read and close functions.

    @param ext_app_ptr  the ext_app_ptr field in the mpgdmux_settings structure is passed back here
    @param fileName     if a UNICODE filename was passed to mpgdmux, it is returned here

    @return             the file handle as a long or -1 if an error occurs
*/
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__native_client__) && !defined(__vxworks)
    #ifndef UNDER_CE
typedef long (*mpg_external_open_callbackW)(void *ext_app_ptr, wchar_t *fileName);
    #else
typedef FILE* (*mpg_external_open_callbackW)(void *ext_app_ptr, wchar_t *fileName);
    #endif
#else
typedef long (*mpg_external_open_callbackW)(void *ext_app_ptr, uint16_t *fileName);
#endif

/*!
    @brief Called to seek in a file.

    @param ext_app_ptr  the ext_app_ptr field in the mpgdmux_settings structure is passed back here
    @param fileHandle   a file handle returned by the mpg_external_open_callback(W) function
    @param position     the position to seek to

    @return             0 if Ok, 1 if an error occurs
*/
#ifndef UNDER_CE
typedef int32_t (*mpg_external_seek_callback)(void *ext_app_ptr, long fileHandle, int64_t position);
#else
typedef int32_t (*mpg_external_seek_callback)(void *ext_app_ptr, FILE* pfile, int64_t position);
#endif

/*!
    @brief Called to read from a file.

    @param appData      the ext_app_ptr field in the mpgdmux_settings structure is passed back here
    @param fileHandle   a file handle returned by the mpg_external_open_callback(W) function
    @param buffer       a buffer for the data
    @param bufferSize   the number of bytes to read

    @return             the number of bytes actually read or 0 if an error occurs
*/
#ifndef UNDER_CE
    typedef int32_t (*mpg_external_read_callback)(void *ext_app_ptr, long fileHandle, uint8_t *buffer, int32_t bufferSize);
#else
    typedef int32_t (*mpg_external_read_callback)(void *ext_app_ptr, FILE* pfile, uint8_t *buffer, int32_t bufferSize);
#endif

/*!
    @brief Called to close a file.

    @param appData     the ext_app_ptr field in the mpgdmux_settings structure is passed back here
    @param fileHandle  a file handle returned by the mpg_external_open_callback(W) function

    @return            none
*/
#ifndef UNDER_CE
typedef void (*mpg_external_close_callback)(void *ext_app_ptr, long fileHandle);
#else
typedef void (*mpg_external_close_callback)(void *ext_app_ptr, FILE* pfile);
#endif

/*!
    @brief Called when DVD PCI chunk is received.

    @param parser_app_ptr  the parser_app_ptr field in the mpgdmux_parser_settings structure is passed back here
    @param buf             a pointer to the PCI data
    @param buf_len         the number of bytes in buf

    @return                none
*/
typedef void (*mpg_dvd_PCI_callback)(void *parser_app_ptr, void *buf, int32_t buf_len);

/*!
    @brief Called when DVD DSI chunk is received.

    @param parser_app_ptr  the parser_app_ptr field in the mpgdmux_parser_settings structure is passed back here
    @param buf             a pointer to the DSI data
    @param buf_len         the number of bytes in buf

    @return                none
*/
typedef void (*mpg_dvd_DSI_callback)(void *parser_app_ptr, void *buf, int32_t buf_len);

//!@}

//! demuxer instance
typedef struct mpeg_demuxer mpgdmux_tt;


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

/*!
 @brief These settings are used to create a demuxer instance and are used in both file and streaming mode. 
*/
struct mpgdmux_settings
{
    int32_t dvd_get_vts_menu;             //!< indicates the VTS menu is to be opened (1) instead of the VTS contents (0)

                                          //!< Only used for IFO files.

    int32_t dvd_title_num;                //!< indicates which VTS title to open

                                          //!< Range is 1 .. mpgdmux_file_info.dvd_num_titles.
                                          //!< Only used for IFO files.

    int32_t dvd_language;                 //!< indicates which language to use when opening a menu from an IFO file.

                                          //!< Only used for IFO files.

    int32_t dvd_angle;                    //!< indicates which angle to use when opening a VTS

                                          //!< Range is 1 .. mpgdmux_file_info.dvd_num_angles.
                                          //!< Only used for IFO files.

    int32_t find_stream_limit;            //!< indicates the number of megabytes of the input file to search for elementary streams

                                          //!< A value of 0 means use the default built into the demuxer.

    int32_t find_stream_options;          //!< indicates what streams to look for when searching the input file for stream information

                                          //!< See \ref MPGDMUX_FIND "MPGDMUX_FIND_xxx flags"<br>
                                          //!< FIND_NONE do the default search (all streams within the MB limit)<br>
                                          //!< FIND_VIDEO stop searching after 1 video stream is found<br>
                                          //!< FIND_AUDIO stop searching after 1 audio stream is found<br>
                                          //!< FIND_VIDAUD stop searching after 1 video and 1 audio streams are found<br>
                                          //!< FIND_SUBPIC stop searching after 1 subpic stream is found<br>
                                          //!< FIND_VIDAUDSP stop searching after 1 video, 1 audio and 1 subpic streams are found<br>
                                          //!< FIND_ALL find all streams within the MB search limit<br>

    //! @name splitter usage
    //!@{
    int32_t splitter_mode;                //!< indicates that the demuxer is being used in splitter mode

                                          //!< This mode is sort of a hybrid between file and stream mode, used by mfimport.

    mpg_splitter_seek_callback splitter_seek_callback;   //!< seek callback for splitter mode, used by mfimport
    mpg_splitter_read_callback splitter_read_callback;   //!< read callback for splitter mode, used by mfimport
    void *split_app_ptr;                                 //!< an app pointer for splitter mode, used by mfimport

    //!@}

    //! @name external IO stuff
    //!@{
    int32_t use_external_io;                             //!< indicates that the app will provide the I/O for the demuxer
    mpg_external_open_callback external_open_callback;   //!< the external open callback pointer
    mpg_external_open_callbackW external_open_callbackW; //!< the UNICODE external open callback pointer
    mpg_external_seek_callback external_seek_callback;   //!< the external seek callback pointer
    mpg_external_read_callback external_read_callback;   //!< the external read callback pointer
    mpg_external_close_callback external_close_callback; //!< the external close callback pointer
    void *ext_app_ptr;                                   //!< an app pointer for external I/O mode, it is passed to the external callbacks

    //!@}

    int64_t file_length;                  //!< length of the input file, used for splitter and external I/O modes only

    uint8_t reserved[64];                 //!< reserved for future use
};

// file mode structures

/*!
 @brief These settings are used to create a parser in file mode.
*/
struct mpgdmux_parser_settings
{
    int32_t requested_buffer_size;                      //!< the size in bytes the app wished the parser to use for bitstream buffers

    mpg_dvd_PCI_callback dvd_PCI_callback;              //!< a pointer to a PCI callback function

                                                        //!< When a DVD PCI packet is demuxed it is sent to the app via this callback.

    mpg_dvd_DSI_callback dvd_DSI_callback;              //!< a pointer to a DSI callback function

                                                        //!< When a DVD DSI packet is demuxed it is sent to the app via this callback.

    void *parser_app_ptr;                               //!< an app pointer for the DVD PCI and DSI callbacks, it is passed to the callbacks

    uint8_t always_seek_flag;                           //!< indicates the demuxer should always seek instead of using buffered data

    uint8_t reserved[63];                               //!< reserved for future use
};

/*!
 @brief This structure is used to retrieve information about an elementary stream in file mode.
*/
struct mpgdmux_stream_info
{
    int32_t stream_type;               //!< stream type as specified in the PMT or PSM

                                       //!< Usually one of the \ref STREAM_TYPE "STREAM_TYPE_xxx defines" in demux_mp2.h.
                                       //!< If the PMT or PSM is not present this field may be 0.

    int32_t sdk_stream_type;           //!< a general stream type as defined by the demuxer

                                       //!< It is one of the \ref MPGDMUX "MPGDMUX_xxx defines" in demux_mp2.h.

    int32_t elementary_PID;            //!< the PID of the stream, transport streams only
    uint8_t ID;                        //!< elementary stream ID, program/PVA streams only
    uint8_t sub_ID;                    //!< sub stream ID of a DVD private 1 stream, DVD only
    int32_t ES_info_length;            //!< length in bytes of the descriptors that follow in the next field
    uint8_t *descriptors;              //!< elementary stream descriptors present in PMT or PSM for this stream

    uint8_t PES_scrambled;             //!< the value of the PES_scrambling_control field of the PES header, MPEG-2 transport/program streams only

                                       //!< A non-zero value indicates the PES packet is scrambled (encrypted).

    uint8_t ts_scrambled;              //!< the value of the transport_scrambling_control field from the transport packet header, transport streams only

                                       //!< A non-zero value indicates the transport payload is scrambled (encrypted)

    uint8_t mpeg1_flag;                //!< if the video stream is a MPEG-1 video stream

                                       //!< = 1 for MPEG-1 video streams.

    uint32_t view_id;                  //!< view ID for MVC streams
    
    uint8_t reserved[57];              //!< reserved for future use
};

/*!
 @brief This structure is used to retrieve information about a program in file mode.
*/
 struct mpgdmux_program_info
{
    int32_t program_number;            //!< the program number as specified in the PMT, transport streams only
    int32_t PID;                       //!< network PID if program_number is 0, else program map PID, transport stream only
    int32_t PCR_PID;                   //!< the PID that carries the PCR for this program, transport stream only
    int32_t stream_count;              //!< number of elementary streams in this program


    uint8_t section_syntax_indicator;  //!< the section syntax indicator field of the PMT, transport streams only
    uint8_t private_indicator;         //!< the private indicator field of the PMT (for program number 0), transport streams only

    int32_t program_info_length;       //!< length in bytes of the descriptors that follow in the next field
    uint8_t *descriptors;              //!< program descriptors present in PMT or PSM

    uint8_t reserved[64];              //!< reserved for future use
};

/*!
 @brief This structure is used to retrieve information about the file in file mode.
*/
struct mpgdmux_file_info
{
    int32_t program_count;             //!< the number of programs in the input stream

                                       //!< For program, PVA or elementary streams it will always be 1.
                                       //!< For transport streams it will be >= 1.

    int32_t system_stream_flag;        //!< input stream type, one of the STREAM_xxx defines in mcdefs.h
    int32_t dvd_num_titles;            //!< number of titles in a VTS, only used for .IFO files (DVD only)
    int32_t dvd_num_angles;            //!< number of angles in a VTS, only used for .IFO files (DVD only)
    int64_t file_size;                 //!< total size of the input file(s)

    int32_t transport_stream_id;       //!< transport_stream_id from the PAT, transport streams only

    //! @name currently not supported
    //!@{
    int32_t tsdt_info_length;          //!< length of the descriptors below
    uint8_t *tsdt_descriptors;         //!< transport stream descriptors present in TSDT

    //!@}

    //! @name currently not supported
    //!@{
    int32_t ca_info_length;            //!< length of the descriptors below
    uint8_t *ca_descriptors;           //!< conditional access descriptors

    //!@}

    int32_t ts_packet_size;            //!< size of transport stream packets, usually 188, transport stream only
    uint32_t flags;                    //!< one of the \ref MPGDMUX_FILE "MPGDMUX_FILE_xxx flags"

    int64_t data_start_offset;         //!< start of the actual data in case of zero padding at the start of the file
    int64_t data_end_offset;           //!< end of the actual data in case of zero padding at the end of the file

    uint8_t reserved[40];              //!< reserved for future use
};

/*!
 @brief This structure is used to retrieve index information.
 */
struct mpgdmux_index_info
{
    int64_t file_pos;                  //!< the index file position of the entry
    int64_t PTS;                       //!< the PTS time stamp of the entry
    int64_t DTS;                       //!< the DTS time stamp of the entry
    uint8_t pic_type;                  //!< the picture type of the entry, 1 = B, 2 = P, 3 = B (mpeg and h264 video only)
    int16_t temp_ref;                  //!< temporal reference of the entry(mpeg and h264 video only)
    uint8_t flags;                     //!< any of the \ref MPGDMUX_xxx_FLAG
};

/*!
 @brief This structure is used when adding a stream to a parser in file mode.
 */
struct mpgdmux_stream_settings
{
    int32_t program_idx;               //!< a zero based program index requested by the app

                                       //!< Not the same as the program number!
                                       //!< The range is 0 .. mpgdmux_file_info.program_count - 1.
                                       //!< For program and PVA streams, this will always be 0.

    int32_t stream_idx;                //!< a zero based stream index requested by the app

                                       //!<  The range is 0 .. mpgdmux_program_info.stream_count – 1.

    bufstream_tt *bs;                  //!< a pointer to the output bufstream for this elementary stream

    uint8_t reserved[64];              //!< reserved for future use
};

// streaming mode structures

/*!
 @brief This structure is used to initialize streaming mode.
*/
struct mpgdmux_sm_init_settings
{
    int32_t stream_type;               //!< the type of the input stream, one of the MPGDMUX_STREAM_xxx defines in mcdefs.h

                                       //!< If this is set to -1 (unknown), the input stream must start with the appropriate
                                       //!< start code so the demuxer can detect the stream type immediately.

    bufstream_tt *input_bs;            //!< a pointer to the input bufstream for the source streams

    uint8_t reserved[68];              //!< reserved for future use
};

/*!
 @brief This structure is used to add a stream in streaming mode.
*/
struct mpgdmux_sm_stream_settings
{
    int32_t PID;                       //!< the PID of the desired stream, transport and PVA streams only
    int32_t ID;                        //!< the ID of the desired stream, program/elementary streams only
    int32_t sub_ID;                    //!< the DVD sub ID of the desired stream, only used if ID = 0xBD (private stream 1)
    bufstream_tt *bs;                  //!< the output bufstream for the desired stream

    uint8_t reserved[64];              //!< reserved for future use
};


#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

//! @name general functions
//!@{

/*!
    @brief Call to create a demuxer instance.
    The app pointer is passed back to all resource functions.

    @param [in] callbacks           a pointer to a structure with resource callbacks definition
    @param [in] demux_callbacks     a pointer to a structure with demultiplexer-specific callbacks definition
    @param [in] set                 a pointer to an mpgdmux_settings structure

    @return                         a pointer to a demuxer instance if successful, NULL if unsuccessful
*/
mpgdmux_tt * MC_EXPORT_API mpegDemuxNew(callbacks_t * callbacks, callbacks_demux_mp2_t * demux_callbacks, struct mpgdmux_settings *set);

/*!
    @brief Call to free a demuxer instance.

    @param [in] demuxer  a pointer to a mpgdmux_tt object created with a mpegDemuxNew call

    @return              none
*/
void MC_EXPORT_API mpegDemuxFree(mpgdmux_tt *demuxer);

//!@}

//! @name file mode functions
//!@{

/*!
    @brief Call to open a file for demuxing
    This can be a DVD IFO as well as an MPEG or PVA file.
   
    @param [in] demuxer    a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] reserved   reserved, set to NULL
    @param [in] filename   a pointer to a filename to open
   
    @return                0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxOpen(mpgdmux_tt *demuxer, void *reserved, char *filename);

/*!
    @brief Call to open a file for demuxing, UNICODE version.
    This can be a DVD IFO as well as an MPEG or PVA file.
   
    @param [in] demuxer    a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] reserved   reserved, set to NULL
    @param [in] filename   a pointer to a UNICODE filename to open
   
    @return                0 if successful, non-zero if unsuccessful
*/
#if !defined(__APPLE__) && !defined(__linux__) && !defined(__native_client__) && !defined(__vxworks) && !defined(__QNX__)

int32_t MC_EXPORT_API mpegDemuxOpenW(mpgdmux_tt *demuxer, void *reserved, wchar_t *filename);

#else // !defined(__APPLE__) && !defined(__linux__) && !defined(__vxworks) && !defined(__QNX__)

// this version will currently return an error!
int32_t MC_EXPORT_API mpegDemuxOpenW(mpgdmux_tt *demuxer, void *reserved, uint16_t *filename);

#endif // !defined(__APPLE__) && !defined(__linux__) && !defined(__vxworks) && !defined(__QNX__)

/*!
    @brief Call to close file. 
    This will also free any remaining parsers.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call

    @return               none
*/
void MC_EXPORT_API mpegDemuxClose(mpgdmux_tt *demuxer);

/*!
    @brief Call to get information about the input file.
    This will return the number of programs in the file.
    For program streams and PVA files this will always be 1. 
    For transport streams it may be > 1.

    @param [in] demuxer     aa pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] file_info   a pointer to a mpgdmux_file_info structure

    @return                 0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxGetFileInfo(mpgdmux_tt *demuxer, struct mpgdmux_file_info *file_info);

/*!
    @brief Call to get information about the specified program.
    This will return the number of elementary streams in the program.
    There may be no elementary streams in a program.

    @param [in] demuxer        a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] program_info   a pointer to a mpgdmux_program_info structure
    @param [in] program_idx    a zero-based program index number

    @return                    0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxGetProgramInfo(mpgdmux_tt *demuxer, struct mpgdmux_program_info *program_info, int32_t program_idx);

/*!
    @brief Call to get information about the specified stream in the specified program.

    @param [in] demuxer       a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] stream_info   a pointer to a mpgdmux_stream_info structure
    @param [in] program_idx   a zero based program index
    @param [in] stream_idx    a zero based stream index
   
    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxGetStreamInfo(mpgdmux_tt *demuxer, struct mpgdmux_stream_info *stream_info, int32_t program_idx, int32_t stream_idx);

/*!
    @brief Call to create a new parser in file mode.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] set       a pointer to a filled mpgdmux_parser_settings structure

    @return               a non-zero parser number if successful, 0 if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxNewParser(mpgdmux_tt *demuxer, struct mpgdmux_parser_settings *set);

/*!
    @brief Call to free a parser instance.

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a pointer to a filled mpgdmux_parser_settings structure

    @return                  none
*/
void MC_EXPORT_API mpegDemuxFreeParser(mpgdmux_tt *demuxer, int32_t parser_num);

/*!
    @brief Call to add a stream to a parser in file mode.

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser call
    @param [in] set          a pointer to an mpgdmux_stream_settings structure

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxAddStream(mpgdmux_tt *demuxer, int32_t parser_num, struct mpgdmux_stream_settings *set);

/*!
    @brief Call to remove an output bufstream

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser call
    @param [in] set          a pointer to an mpgdmux_stream_settings structure

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxRemoveStream(mpgdmux_tt *demuxer, int32_t parser_num, struct mpgdmux_stream_settings *set);

/*!
    @brief Call to position a parser at a particular file position in file mode.
    The demuxer will find the nearest start code and start demuxing there.

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser call
    @param [in] file_pos     the byte file position to seek to
  
    @return                  0 if successful,<br>
                             2 if EOF,<br>
                             1 if an error occurs
*/
int32_t MC_EXPORT_API mpegDemuxSeek(mpgdmux_tt *demuxer, int32_t parser_num, int64_t file_pos);

/*!
    @brief Call to push some source stream data through the demuxer in file mode.
    The return value is the number of source stream bytes consumed by the demuxer.

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser callS
   
    @return
                            non-zero if successful, the number of bytes consumed,<br>
                            0 if EOF,<br>
                            -1 if an error occurs<br>
*/
int32_t MC_EXPORT_API mpegDemuxPush(mpgdmux_tt *demuxer, int32_t parser_num);

#ifndef UNDER_CE
/*!
    @brief Call to re-open the parsers file handle in file mode.

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser call

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxWakeParser(mpgdmux_tt *demuxer, int32_t parser_num);
#endif // UNDER_CE

#ifndef UNDER_CE
/*!
    @brief Call to close the actual file of a parser.
    The parser is left intact and can be re-opened with the wake function

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser call

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxSleepParser(mpgdmux_tt *demuxer, int32_t parser_num);
#endif // UNDER_CE

#ifndef UNDER_CE
/*!
    @brief Call to update the size of the source file (if it is getting bigger) when in splitter mode or when using external I/O mode.
    File mode only.

    @param [in] demuxer      a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num   a parser number from a mpegDemuxNewParser call
    @param [in] file_size    the new file size if splitter or external i/o mode, else unused

    @return                  0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxUpdateFilesize(mpgdmux_tt *demuxer, int32_t parser_num, int64_t file_size);
#endif // UNDER_CE

/*!
    @brief Call to have the parser perform a seek before the next read
    and optionally update the internal app_data ptr

    @param [in] demuxer         a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] parser_num      a parser number from a mpegDemuxNewParser call
    @param [in] split_app_ptr   can be used to updated the split_app_ptr from the mpgdmux_settings structure

    @return                     0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxSetFirstSplitRead(mpgdmux_tt *demuxer, int32_t parser_num, void *split_app_ptr);

//!@}

//! @name streaming mode functions
//!@{

/*!
    @brief Call to initialize streaming mode.
    Currently if the mpgdmux_sm_init_settings.stream_type setting is -1
    (meaning unknown stream type, 0 = elementary stream!), the source data coming into the demuxer
    must begin with the appropriate start code. If the stream_type setting is known and supplied, the
    demuxer will ignore anything before the appropriate start code.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] set       a pointer to a filled mpgdmux_sm_init_settings structure
   
    @return               0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxStreamModeInit(mpgdmux_tt *demuxer, struct mpgdmux_sm_init_settings *set);

/*!
    @brief Call to add a stream for streaming demuxing.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] set       a pointer to a filled mpgdmux_sm_stream_settings structure

    @return               0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxStreamModeAddStream(mpgdmux_tt *demuxer, struct mpgdmux_sm_stream_settings *set);

/*!
    @brief Call to flush a streaming demuxing session.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call

    @return               0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxStreamModeFlush(mpgdmux_tt *demuxer);

/*!
    @brief Call to finish a streaming demuxing session.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] abort     set to 1 to abort demuxing or 0 to finish normally

    @return               0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxStreamModeDone(mpgdmux_tt *demuxer, int32_t abort);

//!@}

//! @name index functions
//!@{

/*!
    @brief Call to create an index of the input stream.

    @param [in] demuxer        a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] get_pic_info   set to a 1 to determine picture type and temp ref, mpeg and AVC only

    @return                    0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxCreateIndex(mpgdmux_tt *demuxer, int32_t get_pic_info);

/*!
    @brief Call to get the index count for a elementary stream.

    @param [in] demuxer       a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] program_idx   a zero based program index
    @param [in] stream_idx    a zero based stream index

    @return                   number of entries if successful, else zero
*/
int32_t MC_EXPORT_API mpegDemuxGetIndexCount(mpgdmux_tt *demuxer, int32_t program_idx, int32_t stream_idx);

/*!
    @brief Call to get an index entry for a stream.

    @param [in] demuxer       a pointer to a mpgdmux_tt object created with a mpegDemuxNew call
    @param [in] program_idx   a zero based program index
    @param [in] stream_idx    a zero based stream index
    @param [in] info          a pointer to a mpgdmux_index_info to receive the index info
    @param [in] idx           the index entry number to fetchs

    @return                   0 if successful, non-zero if unsuccessful
*/
int32_t MC_EXPORT_API mpegDemuxGetIndexEntry(mpgdmux_tt *demuxer, int32_t program_idx, int32_t stream_idx, struct mpgdmux_index_info *info, uint32_t idx);

/*!
    @brief Call to free a created index.

    @param [in] demuxer   a pointer to a mpgdmux_tt object created with a mpegDemuxNew call

    @return               none
*/
void MC_EXPORT_API mpegDemuxFreeIndex(mpgdmux_tt *demuxer);

//!@}

/*!
    @brief Call to get extended API function.

    @param [in] func   function ID

    @return            function pointer or NULL
*/
APIEXTFUNC MC_EXPORT_API mpegDemuxGetAPIExt(uint32_t func);



#ifdef __cplusplus
}
#endif

#endif //#if !defined (__DEMUX_MP2_API_INCLUDED__)

