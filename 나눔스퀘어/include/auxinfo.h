/**
@file: auxinfo.h
@brief Class for exchanging auxiliary data between components

@verbatim
File: auxinfo.h
Desc: Class for exchanging auxiliary data between components

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
@endverbatim
 **/

#if !defined (__AUXINFO_INCLUDED__)
#define __AUXINFO_INCLUDED__

#include <mctypes.h>

enum aux_ID
{

/**
 * @name  AccessUnit identification
 * @{
 **/
  UNSPECIFIED_AU        = 0x000F0000L,
  VIDEO_AU_CODE         = 0x000F0001L,                      /**<@brief Used to pass video access unit information @hideinitializer*/
  AUDIO_AU_CODE         = 0x000F0002L,
  SP_AU_CODE            = 0x000F0004L,
  VIDEO_AU_START        = 0x000F0008L,
  RATECONTROL_STAT      = 0x000F0009L,
/** @} */


/**
 * @name  Use to get contents of bytecount field
 * @{
 **/
  BYTECOUNT_INFO        = 0x000F0010L,
  FLUSH_BUFFER          = 0x000F0020L,
  SPLIT_OUTPUT          = 0x000F0040L,
  OUTPUT_SEGMENT_INFO   = 0x000F0041L,
  /** @} */


/**
 * @name  Used to signal end of stream
 * @{
 **/
  STREAM_END_CODE       = 0x000F0080L,                    /**<@brief Indicates that the flushing is done and all data are sent to the output bufstream.*/
/** @} */

/**
 * @name  Used to signal stream discontinuity
 * @{
 **/
  STREAM_DISCONTINUITY_CODE = 0x000F0081L,

/**
 * @name Muxer output information codes
 * @{
 **/
  VIDEO_STREAM_INFO     = 0x000F0100L,
  VIDEO_AU_INFO         = 0x000F0200L,
  AUDIO_STREAM_INFO     = 0x000F0400L,
  AUDIO_STREAM_INFO_EX  = 0x000F0401L,
  AUDIO_AU_INFO         = 0x000F0800L,

  VIDEO_SEQ_INFO_XML    = 0x000F0810L,
  VIDEO_AU_INFO_XML     = 0x000F0811L,
  AUDIO_SEQ_INFO_XML    = 0x000F0812L,
  AUDIO_AU_INFO_XML     = 0x000F0813L,

  INDEX_CONTAINER_INFO  = 0x000F0814L,
  INDEX_STREAM_INFO     = 0x000F0815L,
  INDEX_AU_INFO         = 0x000F0816L,

  METADATA_INFO_XML     = 0x000F0817L,	/**<@brief Used to pass container metadata XML @hideinitializer*/
/** @} */

/**
 * @name DVD navigation codes
 * @{
 **/
  DVD_DO_NAV_INFO       = 0x000F1000L,
  DVD_SECTOR_NUM        = 0x000F2000L,
  DVD_PTM_TIME          = 0x000F4000L,
  DVD_ABORT_NAV_INFO    = 0x000F8000L,

  DVD_FIXUP_LAST_VOBU   = 0x000E0000L,
  DVD_SETUP_VOBIDN      = 0x000E0001L,
  DVD_RLBN_INFO         = 0x000E0002L,
  DVD_SEQ_END_CODE      = 0x000E0004L,
  DVD_GET_LAST_RLBN     = 0x000E0008L,
  DVD_GET_RLBN_COUNT    = 0x000E0009L,
  DVD_SET_HLI_INFO      = 0x000E0010L,
  DVD_DO_HLI_PTM_TIME   = 0x000E0020L,
  DVD_SETUP_CELLIDN     = 0x000E0040L,
  DVD_CELL_CHANGE       = 0x000E0080L,

  DVD_DO_ASYNCI_INFO    = 0x000E0101L,	/**<@brief Return value is the stream ID which must be passed back to DVD_ASYNCI_INFO @hideinitializer*/
  DVD_ASYNCI_INFO       = 0x000E0102L,
  DVD_DO_SPSYNCI_INFO   = 0x000E0201L,	/**<@brief Return value is the stream ID which must be passed back to DVD_SPSYNCI_INFO @hideinitializer*/
  DVD_SPSYNCI_INFO      = 0x000E0202L,
  DVD_FIXUP_SYNCI       = 0x000E0203L,
/** @} */


/**
 * @name VCD items
 * @{
 **/
  VCD_UPDATE_VBV        = 0x000D0000L,
/** @} */


/**
 * @name Demuxer messages
 * @{
 **/
  DMUX_CHUNK_INFO           = 0x000D1000L,
  DMUX_CHUNK_INFO_EX        = 0x000D1001L,
/** @} */


/**
 * @name Transport stream ID messages
 * @{
 **/
  TS_MICROMV            = 0x000C0001L,
  TS_DVB                = 0x000C0002L,
  TS_DVHS               = 0x000C0004L,
/** @} */



/**
 * @name Buffered seek messages
 * @{
 **/
  BUF_SEEK_INFO            = 0x000C0011L,
  BUF_SEEK_SAMPLE_INFO     = 0x000C0012L,
  BUF_SEEK_STREAM_INFO     = 0x000C0014L,
  BUF_SEEK_UPDATE_SETTINGS = 0x000C0015L,
  BUF_SEEK_RESET           = 0x000C0016L,
  BUF_SEEK_REMOVE_STREAM   = 0x000C0017L,
  BUF_SEEK_INIT_OUTPUT     = 0x000C0018L,
  BUF_SEEK_MSTART          = 0x000C0019L,
  BUF_SEEK_REORDER         = 0x000C0020L,
/** @} */


/**
 * @name Filename and filenumber codes
 * @{
 **/
  FILENUMBER_INFO       = 0x00F00001L,
  FILENAME_INFO         = 0x00F00002L,
  SWAP_ENDIAN           = 0x00F00004L,
  FILESIZE_INFO         = 0x00F00008L,	/**<@brief only when opened for reading!!! @hideinitializer*/
  FILESEEK_INFO         = 0x00F00010L,
  FILEWRITE_MODE        = 0x00F00020L,
  FILEREAD_MODE         = 0x00F00040L,
/** @} */


/**
 * @name Audio-sync identification
 * @{
 **/
  AUDIO_SYNC_CODE       = 0xFFF00000L,
/** @} */


/**
 * @name Video-sync (exactly the same as MPEG-Codes)
 * @{
 **/
  ID_PICTURE_START_CODE = 0x00000100L,	/**<@brief Used to pass picture start code @hideinitializer*/
  ID_SLICE_MIN_START    = 0x00000101L,
  ID_USER_START_CODE    = 0x000001B2L,
  ID_SEQ_START_CODE     = 0x000001B3L,	/**<@brief Used to pass sequence start code @hideinitializer*/
  ID_EXT_START_CODE     = 0x000001B5L,
  ID_SEQ_END_CODE       = 0x000001B7L,
  ID_GOP_START_CODE     = 0x000001B8L,	/**<@brief Used to pass GOP start code @hideinitializer*/
  ID_ISO_END_CODE       = 0x000001B9L,
  ID_PACK_START_CODE    = 0x000001BAL,
  ID_SYSTEM_START_CODE  = 0x000001BBL,

  ID_PREFIX_START_CODE  = 0x000001FFL,	/**<@brief Used to pass prefix information @hideinitializer*/
/** @} */

/**
 * @name  Auxiliary information
 * @{
 **/
  TIME_STAMP_INFO       = 0x0F000000L,	/**<@brief Used to pass actual timestamps (formerly TIMESTAMP_INFO) @hideinitializer*/
  STATISTIC_INFO        = 0x0F000001L,	/**<@brief Used to pass encoding statistics @hideinitializer*/
  CPB_FULLNESS          = 0x0F000002L,	/**<@brief Used to pass current cpb fullness @hideinitializer*/
  DELAYS_STAT           = 0x0F000003L,	/**<@brief Used to pass encoding delays @hideinitializer*/

  TIMECODE_BREAK        = 0x0F000004L,	/**<@brief Used to pass a new start timecode to MXF muxer. Used from next frame on - to be sent before the frames VIDEO_AU_CODE @hideinitializer*/
  TIMECODE_INFO		      = 0x0F000005L,	/**<@brief Used to pass time code data from demuxer @hideinitializer*/

  SAMPLE_INFO           = 0x0F001000L,	/**<@brief Used to pass a sample which is not aligned to audio / video access units' bounds. @hideinitializer*/
  MUX_STATISTIC_INFO    = 0x0F001001L,	/**<@brief Muxer statistics: padding, output bitrate, etc. @hideinitializer*/
  SUB_SAMPLE_INFO	      = 0x0F001002L,	/**<@brief Sub sample information. @hideinitializer*/

  MUX_IN_STATISTIC_INFO = 0x0F001003L,	/**<@brief Muxer input stream statistic (retrieve number of parsed access units, used for MXF Muxer) @hideinitializer*/

  AUXINFO_USER_DATA      = 0x0F001004L, /**<@brief auxinfo user data @hideinitializer*/
/** @} */


/**
 * @name  Windows Media audio information
 * @{
 */
  WMA_STREAM_INFO       = 0x1F000000L,	/**<@brief Used to pass data from WMA encoder to (ASF) muxer @hideinitializer*/
/** @} */

/**
 * @name  VC-1 format block (Sequence and entry-point headers for advanced profile, WMV format block for simple and main profiles)
 * @{
 */
  WMV_STREAM_INFO       = 0x1F000010L,	/**<@brief Used to pass data from VC-1 encoder to (ASF) muxer, intended mainly for SP/MP coding as there are no start codes for these profiles @hideinitializer*/
/** @} */


/**
 * @name  Dolby Digital audio information
 * @{
 */
  DD_STREAM_INFO        = 0x1F000100L,	/**<@brief Used to pass data from DD encoder to muxer @hideinitializer*/
/** @} */

/**
 * @name  Dolby Digital metadata information
 * @{
 */
  DD_METADATA_INFO        = 0x1F000101L,	/**<@brief Used to pass metadata from DDPP encoder in transcoding mode @hideinitializer*/
/** @} */

/**
 * @name  AAC Encoder Sync frame information
 * @{
 */
  AAC_SYNC_FRAME_INFO        = 0x1F000110L,	/**<@brief Used to pass information about sync frame @hideinitializer*/
/** @} */


/**
 * @name  H.264/AVC information
 * @{
 */
   AVC_NALU_STREAM_INFO		= 0x1F001000L,	/**<@brief Used to pass AVC raw stream information @hideinitializer*/
   SEI_PICTURE_TIMING_INFO	= 0x1F001001L,	/**<@brief Used to pass picture timing SEI @hideinitializer*/
   SEI_BUFFERING_PERIOD_INFO	= 0x1F001002L,	/**<@brief Used to pass buffering period SEI @hideinitializer*/
 /** @} */

/* */
  DMFA_STREAM_INFO      = 0x2F000000L,	/**<@brief Used to pass data from DMF audio encoder to muxer @hideinitializer*/
  DMFV_STREAM_INFO      = 0x2F000001L,	/**<@brief Used to pass data from DMF video encoder to muxer @hideinitializer*/

  MP3_UNLOCK_INFO       = 0x2F000002L,	/**<@brief Muxer and mp3 encoder handshaking @hideinitializer*/


/**
 * @name  Lock mechanism for connecting components
 * @{
 */
  ACCESS_INFO           = 0x30000000L,	/**<@brief General unlock handling @hideinitializer*/
/** @} */

/**
 * @name  General stream format information (to be sent by each encoder)
 * @{
 */
  STREAM_FORMAT_INFO    = 0xF0000000L	/**<@brief See mc_stream_format_t (mcmediatypes.h) @hideinitializer*/
/** @} */
};


/**
 * @name access_info Structure
 * @{
 **/
 /**
 * @brief These parameters are used to configure access information
 */
struct access_info
{
  const char *codec;	/**< Defines the codec's name (e.g. "MP3") */
  uint32_t    key;	/**< Can be used for handling different keys or serials */
  uint32_t    lock;	/**< 0 = unlocked */
};
/** @} */


/**
 * @name VIDEO_AU_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to configure VIDEO_AU structure
 */
struct v_au_struct
{
  uint32_t length;              /**< Length in bytes of the access unit including all headers (sequence/seqext/gop/picture/etc.) and picture data. */
  uint16_t type;                /**< One of the picture type codes (1 - I frame, 2 - P frame, 3 - B frame, 4 - B refference frame )   */
  uint16_t flags;               /**< Contains SEQHDR_FLAG flag if a sequence header is present and GOPHDR_FLAG if a GOP header is present. */
  // this can always be set to zero by the encoder
  // it is used by the muxer to add user data blocks to streams that
  // don't have them
  uint16_t svcd_offset;         /**< Number of bytes from the beginning of the access unit to the first byte of the user data block header where the SVCD offsets are to be stored. */
  uint16_t pict_hdr_offset;     /**< Number of bytes from the beginning of the access unit to the first byte of the picture header. If the picture header is the first thing in the access unit, this value will be 0.*/
  uint32_t first_frame_offset;  /**< Set to zero for all frames except the I frames following GOP headers. For these I frames, this value is the temporal reference of the I frame multiplied by
                                     the number of 27Mhz clock ticks per picture. For instance, if the frame rate is 29.97fps (30000/1001) and the I frame has a temp ref of 2, the value is set to 2 * (27000000 /
                                     (30000/1001)) or for 25fps, 2 * (27000000 / 25) */
  int64_t  DTS;                 /**< The DTS of the access unit in 27Mhz units */
  int64_t  PTS;                 /**< The PTS of the access unit in 27Mhz units */
};
/** @} */


/**
 * @name TIMESTAMP_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to configure Time stamp information structure
 */
struct sample_info_struct
{
  int64_t  rtStart;
  int64_t  rtStop;
  uint32_t flags;
  int32_t  mode;                        /**< specifies whether rtStart/rtStop are in 100 ns or 27 MHz units (H264_TIMESTAMP_ defines) */
};
/** @} */



/**
 * @name SAMPLE_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to configure Sample information structure
 */
struct sample_struct
{
    int64_t rtStart;		/**< 27 MHz clocks : valid if bTimestampValid = true */
    int64_t rtStop;		/**< 27 MHz clocks : optional field */
    int32_t cbSize;		/**< Mandatory field */
    uint8_t bSampleStart;	/**< Optional */
    uint8_t bTimestampValid;	/**< Mandatory */
    int64_t cbAbsoluteOffset;	/**< Optional. Can be used if a component supports this parameter for exact positioning of the timestamp within a sample being passed. Refer to the component's specs for the details on whether is it supported or not. */

    uint8_t bSysClockValid;	/**< Mandatory */
    int64_t sysClock;		/**< 27 MHz clocks, for PCR/SCR or other system timestamp : valid if bSysClockValid = true */

    uint64_t start_cutoff;	/**< Inidcates, in 27MHz timescale, how much should be skipped at the beginning of the sample */
    uint64_t stop_cutoff;	/**< Indicates, in 27MHz timescale, how much should be skipped at the end of the sample */

    uint8_t bSysClockSynthesized;   /**< 0 = sysClock is an actual system clock value
                                         else sysClock is a synthesized value : valid if bSysClockValid = true */

    uint8_t reserved[6];
};
/** @} */

/**
 * @name SUB_SAMPLE_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to initialize sub sample information structure. One SUB_SAMPLE_INFO/sub_sample_info_s describes a complete subtitle sample.
 */
typedef struct sub_sample_info_t
{
    int64_t rtStart;                            /**< 27 MHz clocks : start time for subtitle sample*/
    int64_t rtStop;	                        /**< 27 MHz clocks : stop time for subtitle sample*/
    uint32_t total_sub_samples_size;            /**< Total sub samples size */
    uint32_t sub_sample_count;                  /**< Count of elements in sub sample size array */
	uint32_t *sub_sample_size_array;	/**< Pointer on a sub sample size array */
	uint8_t reserved[128];			/**< Reserved for future use*/
} sub_sample_info_s;
/** @} */


/**
 * @name MUX_STATISTIC_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe muxer statistics
 */
struct mpgmux_stat_struct
{
    int64_t     padding;
    int32_t     mux_rate_computed;
	int32_t		num_units_queued_v;
	int32_t		num_units_queued_a;
	int64_t		current_pts_v;
	int64_t		current_pts_a;
	int32_t     video_underflows;
	int32_t     audio_underflows;
    uint8_t     reserved[32];
};
/** @} */



/**
 * @name MUX_IN_STATISTIC_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to retrieve a muxers input stream statistics (only supported in MXF muxer currently). 
 */
struct mux_input_stat_struct
{
  uint32_t  count_au;         /**< Number of access units parsed*/
                              /**< Note: count_au does not reflect the accurate number of access units processed 
                                  until the input had received a STREAM_END_CODE auxinfo and the piepelines got emptied*/

  uint8_t   reserved[32];     /**< Reserved for future use*/
};
/** @} */

/**
 * @name STATISTIC_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe encoder statistics
 */
struct encode_stat_struct
{
  int32_t  frames_incoming;                     /**< Number of frames sent to encoder */
  int32_t  frames_encoded;                      /**< Number of encoded frames */
  uint32_t time_ms_elapsed;                     /**< Time elapsed so far */
  double   summary_size;                        /**< Number of kilobits written by encoder */
  double   average_bitrate;                     /**< Average bitrate maintained */
  double   average_speed;                       /**< Average speed */

  double   current_psnr_y;                      /**< Current psnr for luma plane */
  double   current_psnr_u;                      /**< Current psnr for chroma plane */
  double   current_psnr_v;                      /**< Current psnr for chroma plane */
  double   current_psnr_a;                      /**< Current psnr for all three planes */

  double   overall_psnr_y;                      /**< Overall psnr for all luma planes */
  double   overall_psnr_u;                      /**< Overall psnr for all chroma planes */
  double   overall_psnr_v;                      /**< Overall psnr for all chroma planes */
  double   overall_psnr_a;                      /**< Overall psnr for all three planes */

  double   min_qp;                              /**< Minimum quantization used so far */
  double   max_qp;                              /**< Maximum quantization used so far */
  double   avg_qp;                              /**< Average quantization used so far */

  double   overall_avg_qp;                      /**< Overall average quantization */

  uint32_t    current_time_ms_encoding;         /**< Time of pure encoding of the last encoded frame*/
  uint32_t    current_time_ms_elapsed;          /**< Time elapsed of the last encoded frame*/
  
  uint32_t    reserved;
};
/** @} */


/**
 * @name CPB fullness
 * @{
 **/
 /**
 * @brief These parameters are used to describe CPB fullness statistics (obsolete)
 */
struct cpb_fullness_struct
{
    int32_t initial_fullness;                     /**< Buffer level before encoding picture */
    int32_t final_fullness;                       /**< Buffer level after encoding picture */
    int32_t removal_delay;

    int32_t fullness_units;                       /**< 0 = fullness in %, 1 = fullness in bits, 2 = fullness in 90kHz */

    int32_t buffer_size_in_bits;                  /**< Size of buffer */

    int32_t reserved[7];
};

/** @} */

/**
* @name CPB fullness data
* @{
**/
/**
* @brief These parameters are used to describe CPB fullness statistics.
*/
struct cpb_fullness_struct_ex
{
    int64_t initial_fullness;                     /**< Buffer level before encoding picture */
    int64_t final_fullness;                       /**< Buffer level after encoding picture */
    int32_t removal_delay;
    int32_t fullness_units;                       /**< 0 = fullness in %, 1 = fullness in bits, 2 = fullness in 90kHz */
    int64_t buffer_size_in_bits;                  /**< Size of buffer */

    int32_t dwFlags;                              /**< 1 = scale values in bits down by 256 to avoid overflow of int32_t */
    int32_t reserved[6];
};
/** @} */

/**
* @name DELAYS_STAT
* @{
**/
/**
* @brief These parameters are used to describe encoding delays
*/
struct encode_delays_struct
{
    uint64_t    frame_delay;                    /**< Complete frame processing delay in microseconds */
    uint64_t    frame_enc_time;                 /**< Time spent for frame encdoing in microseconds, excluding buffering and analysis */

    uint64_t    reserved[14];
};
/** @} */

/**
* @name Ratecontrol statistics
* @{
**/
/**
* @brief These parameters are used to describe ratecontrol statistics (implemented in AVC encoder) (obsolete)
*
*/
struct ratecontrol_stat_struct
{
    int32_t initial_qp;                         /**< Initial quantazer defined by ratecontrol */
    int32_t reencode_count;                     /**< Count of reencode process  */
    int32_t reentropy_count;                    /**< Count of reentropy process */
    int32_t estimated_size;                     /**< Estimated frame size (based on RD bits)*/
    int32_t vcl_size;                           /**< VCL size (after entropy process) */
    int32_t estimated_initial_fullness;         /**< Estimated initial buffer fullness */
    int32_t initial_fullness;                   /**< Initial buffer fullness */
    int32_t estimated_final_fullness;           /**< Estimated final buffer fullness */
    int32_t final_fullness;           			/**< Final buffer fullness  */
};
/** @} */

/**
* @name Ratecontrol statistics
* @{
**/
/**
* @brief These parameters are used to describe ratecontrol statistics (implemented in AVC encoder).
*
*/
struct ratecontrol_stat_struct_ex
{
    int32_t initial_qp;                         /**< Initial quantazer defined by ratecontrol */
    int32_t reencode_count;                     /**< Count of reencode process  */
    int32_t reentropy_count;                    /**< Count of reentropy process */
    int64_t estimated_size;                     /**< Estimated frame size (based on RD bits)*/
    int64_t vcl_size;                           /**< VCL size (after entropy process) */
    int64_t estimated_initial_fullness;         /**< Estimated initial buffer fullness */
    int64_t initial_fullness;                   /**< Initial buffer fullness */
    int64_t estimated_final_fullness;           /**< Estimated final buffer fullness */
    int64_t final_fullness;           			/**< Final buffer fullness  */
};
/** @} */


/**
 * @name TIMECODE_BREAK
 * @{
 **/
 /**
 * @brief These parameters are used to describe time code
 */
struct timecode_break
{
  int32_t frame_nr;                         /**< The frame no. at which the new timecode starts */
  int32_t time_code;                        /**< The timecode in frames */
  int32_t drop_frame;                       /**< Drop frame timecode */
  int32_t reserved[2];
};
/** @} */


/**
 * @name TIMECODE_BREAK
 * @{
 **/
 /**
 * @brief These parameters are used to describe audio AU
 */
struct a_au_struct
{
  uint32_t length;
  int64_t  pts;
};
/** @} */


/**
 * @name SP_AU_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to describe ...
 */
struct sp_au_struct
{
  uint32_t length;
  int64_t  pts;
};
/** @} */


/**
 * @name AUDIO_SYNC_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to describe ...
 */
struct aud_info
{
  int32_t version;
  int32_t lay;
  int32_t error_protection;
  int32_t bitrate_index;
  int32_t sampling_frequency;
  int32_t padding;
  int32_t extension;
  int32_t mode;
  int32_t mode_ext;
  int32_t copyright;
  int32_t original;
  int32_t emphasis;
};
/** @} */

/**
 * @name WMA_STREAM_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe ...
 */
struct wma_stream_info
{
  int32_t sampling_frequency;
  int32_t bits_per_sample;
  int32_t official_bitrate;
  int32_t num_channels;
  int32_t average_bytes_per_second;
  int32_t samples_per_block;
  int32_t super_block_align;
  int32_t encode_options;
  int32_t block_align;
  float   duration;

  // align structure to 64 bytes:
  uint8_t reserved[64 - 9 * sizeof(int32_t) - sizeof(float)];
};
/** @} */


/**
 * @name WMV_STREAM_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe ...
 */
struct wmv_stream_info
{
  int32_t width;
  int32_t height;

  int32_t aspect_x;
  int32_t aspect_y;

  int32_t frame_rate_num;
  int32_t frame_rate_denom;

  int32_t hdr_size;
  uint8_t hdr[1];
};
/** @} */


/**
 * @name AVC_NALU_STREAM_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe NALU data in AVC elementary stream
 */
struct avc_nalu_stream_info
{
  int32_t reserved_1[3];
  int32_t pps_offset;                           /**< offset of Picture Parameter Set NALU in bits */
  int32_t reserved_2[4];
};
/** @} */


/**
 * @name SEI_PICTURE_TIMING_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe picture timing SEI. See ITU T-REC-H.264 D.2.3
 */
struct sei_pic_timing_struct
{
  int32_t cpb_removal_delay;                    /**< cpb_removal_delay value of picture timing SEI message */
  int32_t dpb_output_delay;                     /**< dpb_output_delay value of picture timing SEI message */
  int32_t pic_struct;                           /**< pic_struct value of picture timing SEI message */
  int32_t reserved_1[5];
  int32_t counting_type;                        /**< counting_type value of picture timing SEI message */
  int32_t full_timestamp_flag;                  /**< full_timestamp_flag value of picture timing SEI message */
  int32_t discontinuity_flag;                   /**< discontinuity_flag value of picture timing SEI message */
  int32_t cnt_dropped_flag;                     /**< cnt_dropped_flag value of picture timing SEI message */
  int32_t n_frames;                             /**< n_frames value of picture timing SEI message */
  int32_t seconds_value;                        /**< seconds_value value of picture timing SEI message */
  int32_t minutes_value;                        /**< minutes_value value of picture timing SEI message */
  int32_t hours_value;                          /**< hours_value value of picture timing SEI message */
  int32_t reserved_3[2];
};
/** @} */


/**
 * @name SEI_BUFFERING_PERIOD_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe buffering period SEI
 */
struct sei_buff_period_struct
{
  int32_t seq_parameter_set_id;                 /**< seq_parameter_set_id value of buffering period SEI message message */
  int32_t initial_cpb_removal_delay;            /**< initial_cpb_removal_delay value of buffering period SEI message message */
  int32_t initial_cpb_removal_delay_offset;     /**< initial_cpb_removal_delay_offset value of buffering period SEI message message */
  int64_t reserved[5];
};
/** @} */

/**
* @name DD_STREAM_INFO
* @{
**/
/**
* @brief These parameters are used to describe ...
*/
struct dd_stream_info
{
    int32_t sampling_frequency;
    int32_t bits_per_sample;
    int32_t num_channels;
    int32_t average_bytes_per_second;
    int32_t samples_per_block;

    // Dolby digital fields
    int32_t fscod; // Deprecated, equal to 0
    int32_t bsid;
    int32_t frmsizcod; // Deprecated, equal to 0
    int32_t bsmod;
    int32_t acmod;
    int32_t data_rate;
    int32_t lfe_enabled;
    // align structure to 64 bytes:
    uint8_t reserved[64 - 12 * sizeof(int32_t)];
};
/** @} */

/**
 * @name PREFIX_START_CODE
 * @{
 **/
 /**
 * @brief These parameters are used for MVC and SVC
 */
struct prefix_start_info
{
  uint32_t id;                          /**< index/id of view, layer or what ever */
  uint32_t reserved[63];
};
/** @} */

/**
 * @name AAC_SYNC_FRAME_INFO
 * @{
 **/
 /**
 * @brief These parameters are used to describe ...
 */
struct aac_syncframe_info
{
    int32_t reserved[64];
};
/** @} */

 /**
 * @name SEQ_START_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to describe sequence start information (obsolete)
 */
struct seq_start_info
{
    uint32_t video_type;              /**< One of video type codes */
    uint32_t frame_rate_code;         /**< One of frame rate codes */
    uint32_t bit_rate;                /**< Bitrate (in bps) if CBR encoding */
    uint32_t max_bit_rate;            /**< Maximum bitrate (in bps) if VBR encoding */
    uint32_t pulldown_flag;           /**< One of the pulldown codes */
    uint32_t vbv_buffer_size;         /**< VBV buffer size */
};

/** @} */

/**
* @name SEQ_START_CODE
* @{
**/
/**
* @brief These parameters are used to describe sequence start information.
*/
struct seq_start_info_ex
{
    uint32_t video_type;              /**< One of video type codes */
    uint32_t frame_rate_code;         /**< One of frame rate codes */
    int64_t bit_rate;                /**< Bitrate (in bps) if CBR encoding */
    int64_t max_bit_rate;            /**< Maximum bitrate (in bps) if VBR encoding */
    uint32_t pulldown_flag;           /**< One of the pulldown codes */
    int64_t vbv_buffer_size;         /**< VBV buffer size */

    int64_t reserved[16];
};
/** @} */

/**
 * @name GOP_START_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to describe GOP start information
 */
struct gop_start_info
{
  uint32_t info_size;          /**< Size of the passed information */

  uint32_t max_govu_length;    /**< Max. number of frames in a GOVU */
  uint32_t reordering_delay;   /**< Max. distance between two ref frames */
  uint32_t idr_frequency;      /**< 0 = only one IDR at the start, 1 = every GOVU starts with and IDR, etc. */

  uint32_t IDR;                /**< 1 = instantaneous decoder refresh */

  uint32_t reserved[32];
};
/** @} */

//PICTURE_START_CODE,        (without size, I will correct it later, not used)
/**
 * @name PICTURE_START_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to describe Picture start information
 */
struct pic_start_info
{
  uint32_t pict_type;           /**< One of the picture type codes  (1 - I frame, 2 - P frame, 3 - B frame, 4 - B reference frame ) */
  uint32_t temp_ref;            /**< Temp ref of the frame */
  uint32_t repeat_first_field;  /**< Repeat first field value of the frame */
  uint32_t vbv_delay;           /**< 90khz based */
  uint32_t top_field_first;     /**< Top field first flag */
  uint32_t progressive_frame;   /**< Progressive frame flag */
};
/** @} */

//USER_START_CODE,            (without size, I will correct it later, not used)
/**
 * @name USER_START_CODE
 * @{
 **/
 /**
 * @brief These parameters are used to describe User data information
 */
struct userdata_info
{
  uint8_t tag_name;
  uint8_t size;
  uint8_t userdata[1];
};
/** @} */

#define SECTOR_FLAG_HAS_VIDEO  0x00000001



/**
 * @name DVD_SECTOR_NUM
 * @{
 **/
 /**
 * @brief These parameters are used to describe DVD sector information
 */
struct dvd_sector_info
{
  int32_t sector_num;     /**< Sector number */
  int32_t sector_length;  /**< Length of sector in 90000 mHz clocks */
  int32_t flags;
};
/** @} */


/**
 * @name DVD_PTM_TIME
 * @{
 **/
 /**
 * @brief These parameters are used to describe DVD PTM time
 */
struct dvd_ptm_info
{
  uint32_t PTM_time;      /**< Ending time of VOB */
};
/** @} */

/**
 * @name dvd_fixup_info
 * @{
 **/
 /**
 * @brief dvd_fixup_info
 */
struct dvd_fixup_info
{
  int32_t sec_present;          /**< A sequence end code is present */
  int32_t first_sector;         /**< First sector number of the added sectors */
  int32_t sectors_to_add;       /**< Number of sectors to add to the last vobu */
  int32_t sector_length;        /**< Length of sector in 90000 mHz clocks */
};
/** @} */

/**
 * @name dvd_vobidn_info
 * @{
 **/
 /**
 * @brief dvd_vobidn_info
 */
struct dvd_vobidn_info
{
  int32_t vob_idn;        /**< New vob idn number */
  int32_t sectors_to_add; /**< Number of sectors to add to the NV_PCK_LBN entries */
};
/** @} */

struct dvd_nv_info
{
  uint32_t nv_sa;
  uint32_t frame_num;
  int32_t  vob_num;
  int32_t  cell_num;
};


struct dvd_synci_info
{
  int32_t stream_num;
  int32_t sector_offset;
};


//GOP_START_CODE, NULL
//EXT_START_CODE, NULL
//SEQ_END_CODE, NULL
//SLICE_MIN_START+j, &j       (without size, I will correct it later, not used)
//SLICE_MIN_START+(j&127), &j (without size, I will correct it later, not used)


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

/** @note The size of these structures must be < 256 bytes or they cannot be passed to an auxinfo function! */


/**
 * @name VIDEO_STREAM_INFO
 * @{
 **/
 /**
 * @brief video_stream_info (Sent once at the start of the first video stream)
 */
struct video_stream_info
{
  uint8_t ID[4];                        /**< Contains 'VINF' */
  int32_t length;                       /**< Size of this structure */
  int32_t program_stream_flag;          /**< One of the STREAM_* codes */
  int32_t stream_id;                    /**< ID of the stream */
  int32_t PID;                          /**< The PID if a transport stream */
  int32_t pulldown;                     /**< One of the PULLDOWN_* codes */
  int64_t frame_duration;               /**< Estimated frame duration from SPS if it present in 27 MHz*/
  int32_t reserved[17];                 /**< Reserved for later use */
  int32_t seqhdr_length;                /**< The used length of the seq_hdr field */
  uint8_t seq_hdr[150];                 /**< Contains all the bytes of the sequence header and sequence extension if present */
};
/** @} */

/**
 * @name AUDIO_STREAM_INFO
 * @{
 **/
 /**
 * @brief audio_stream_info
 */
struct audio_stream_info
{
  uint8_t ID[4];                        /**< Contains 'AINF' */
  int32_t length;                       /**< Size of this structure */
  int32_t program_stream_flag;          /**< One of the STREAM_* codes */
  int32_t stream_id;                    /**< ID of the stream */
  int32_t substream_id;                 /**< The DVD substream ID */
  int32_t PID;                          /**< The PID if a transport stream */
  int32_t audio_type;                   /**< One of the *_AUDIO_* codes */
  int32_t reserved[20];                 /**< Reserved for later use */
  int32_t audhdr_length;                /**< The used length of the audhdr field */
  uint8_t audhdr[84];                   /**< The first N bytes of the audio stream */
  int32_t pvt1hdr_length;               /**< The used length of the pvt1hdr field */
  uint8_t pvt1hdr[8];                   /**< The bytes of the DVD private 1 stream header (PCM or AC3 only) */
};
/** @} */

/**
* @name AUDIO_STREAM_INFO_EX
* @{
**/
/**
* @brief audio_stream_info_ex
*/
struct audio_stream_info_ex
{
    struct audio_stream_info base;
    int32_t channel_count;          /**< Number of audio channels */
    int32_t sampling_frequency;     /**< Audio sampling rate */
    int32_t bitrate;                /**< Bitrate of audio stream */
    int32_t aot;                    /**< Audio object type. 2- AAC LC, 5- AAC HE, 29 - AAC HEv2 */
    uint8_t reserved[64];
};
/** @} */


/**
 * @name VIDEO_AU_INFO
 * @{
 **/
 /**
 * @brief video_au_info (Sent for each frame in the video stream)
 */
struct video_au_info
{
  int64_t filePos;                  /**< Byte offset of the PS/TS sector that contains the first byte of the sequence/gop/picture header */
  uint8_t num_in_sector;            /**< If a sector contains more than one picture header, this indicates which one is the target */
  uint8_t frame_type;               /**< One of the *_TYPE codes */
  int16_t temp_ref;                 /**< The temporal reference of the picture */
  int32_t forw_ref_frame;           /**< Relative index of the forward reference frame for this picture, -1 if there is no forward reference frame */
  int32_t coding_order;             /**< The coding order frame number, i.e. the order of the frame in the the stream */
  int32_t display_order;            /**< The display order of the frame, i.e. the order in which the frame is displayed */
  int64_t PTS;                      /**< The PTS of the frame in 27 MHz units */
};
/** @} */


/**
 * @name VIDEO_AU_START
 * @{
 **/
 /**
 * @brief video_au_start
 */
struct video_au_start
{
  uint32_t chapter_state;       /**< Chapter, scene-change what else? */
  uint32_t tc_frame_nr;         /**< Frame_nr used to generate GOP-timecode */
  uint32_t ts_frame_nr;         /**< Frame_nr used to generate PTS/DTS-timestamps */
  uint32_t flags;               /**< Nothing */
};
/** @} */


/**
 * @name AUDIO_AU_INFO
 * @{
 **/
 /**
 * @brief audio_au_info (Sent for each frame in the audio stream)
 */
struct audio_au_info
{
  int64_t filePos;              /**< Byte offset of the PS/TS sector that contains the first byte of the audio header */
  int32_t num_in_sector;        /**< If a sector contains more than one audio header, this indicates which one is the target */
  int64_t PTS;                  /**< The PTS of the frame in 27 MHz units */
};
/** @} */


/**
 * @name VIDEO_SEQ_INFO_XML
 * @{
 **/
 /**
 * @brief video_seq_info_xml (Sent once at the start of the first video stream)
 */
struct video_seq_info_xml
{
  uint8_t pulldown_flag;        /**< One of the PULLDOWN_* codes */
  uint8_t frame_rate_code;      /**< One of the FRAMERATE* codes */
  uint8_t reserved[62];         /**< Reserved for later use */
};
/** @} */


/**
 * @name VIDEO_AU_INFO_XML
 * @{
 **/
 /**
 * @brief video_au_info_xml (Sent for each frame in the video stream)
 */
struct video_au_info_xml
{
    int64_t  filePos;               /**< If the flags field contains SEQ_HDR or GOP_HDR, this field contains the byte
                                    position of the pack that contains the sequence/gop header. If the flags field does not contain
                                    the SEQ_HDR or GOP_HDR flags, this field contains the relative byte position of the picture
                                    header from the last sequence/GOP header, this value is relative to the last seq/gop header
                                    and it is relative to the video elementary stream, i.e. it does not count any of the
                                    program/transport stream bytes */
  uint32_t flags;                   /**< Can contain SEQ_HDR and/or GOP_HDR */
  int32_t  length;                  /**< Length of the video frame including any seq/gop/pic headers */
  uint8_t  repeat_first_field;      /**< Contains the state of the repeat first field flag of this frame */
  int64_t  PTS;                     /**< The PTS of the frame in 27 MHz units */
  int32_t  hdr_length;              /**< Contains the number of bytes in the hdr field */
  uint8_t* hdr;                     /**< Contains the first 512 bytes of the frame starting with the seq/gop header, only valid if the flags field contains SEQ_HDR or GOP_HDR. Also only valid during the auxinfo function. */
  uint8_t  reserved[31];            /**< Reserved for later use */
};
/** @} */

  /**
 * @name AUDIO_SEQ_INFO_XML
 * @{
 **/
 /**
 * @brief audio_seq_info_xml (Sent once at the start of the first audio stream)
 */
struct audio_seq_info_xml
{
  int64_t clocks_per_audio_frame;       /**< Number of 27MHz clock ticks per audio frame */
  uint8_t reserved[56];                 /**< Reserved for later use */
};
/** @} */


/**
 * @name AUDIO_AU_INFO_XML
 * @{
 **/
 /**
 * @brief audio_au_info_xml (Sent for each frame in the audio stream)
 */
struct audio_au_info_xml
{
  int64_t PTS;                          /**< The PTS of the audio frame in 27 MHz units */
  int32_t length;                       /**< The length in bytes of the audio frame */
  uint8_t reserved[52];                 /**< Reserved for later use */
};
/** @} */
//////////////////////  DVD HLI structures  ///////////////////////////////

// section 4.4.3.2 Highlight General Information (HL_GI)

// table 4.4.3.2-1 HL_GI
struct HL_GI
{
  uint8_t HLI_SS[2];
  uint8_t HLI_S_PTM[4];
  uint8_t HLI_E_PTM[4];
  uint8_t BTN_SL_E_PTM[4];
  uint8_t BTN_MD[2];
  uint8_t BTN_OFN[1];
  uint8_t BTN_Ns[1];
  uint8_t NSL_BTN_Ns[1];
  uint8_t reserved1[1];
  uint8_t FOSL_BTNN[1];
  uint8_t FOAC_BTNN[1];
};


// section 4.4.3.3 Button Color Information Table (BTN_COLIT)

// figure 4.4.3.3-1 BTN_COLI
struct BTN_COLI
{
  uint8_t SL_COLI[4];
  uint8_t AC_COLI[4];
};


// section 4.4.3.4 Button Information Table (BTNIT)

// figure 4.4.3.4-1 BTNI
struct BTNI
{
  uint8_t BTN_POSI[6];
  uint8_t AJBTN_POSI[4];
  uint8_t BTN_CMD[8];
};


struct HLI
{
  struct HL_GI    hl_gi;
  struct BTN_COLI btn_coli[3];
  struct BTNI     bnti[36];
};


// pointer to an HLI structure with size
struct HLI_PTR
{
  int32_t still_menu;
  int32_t size;
  struct HLI*    hli;
};


// BUF_SEEK_INFO
struct buf_seek_info
{
  int64_t  seek_pos;
  uint8_t* bfr;
  uint32_t bfr_size;
};



/**
 * @name DMUX_CHUNK_INFO and DMUX_CHUNK_INFO_EX
 * @{
 **/

#define MCDMUX_FLAGS_UNIT_START                 0x00000001  /**<@brief A unit starts in this chunk @hideinitializer*/
#define MCDMUX_FLAGS_LPCM_FRAME_FOUND           0x00000002  /**<@brief A DVD LPCM frame was found @hideinitializer*/
#define MCDMUX_FLAGS_UNIT_END                   0x00000004  /**<@brief A unit ends in this chunk @hideinitializer*/
#define MCDMUX_FLAGS_KEYFRAME                   0x00000008  /**<@brief The chunk is part of a keyframe @hideinitializer*/
#define MCDMUX_FLAGS_I_TYPE                     0x00000008  /**<@brief The chunk is part of an I type frame if known @hideinitializer*/
#define MCDMUX_FLAGS_P_TYPE                     0x00000010  /**<@brief The chunk is part of an P type frame if known @hideinitializer*/
#define MCDMUX_FLAGS_B_TYPE                     0x00000020  /**<@brief The chunk is part of an B type frame if known @hideinitializer*/
#define MCDMUX_FLAGS_SPS_FLAG                   0x00000040  /**<@brief The chunk contains a SPS header @hideinitializer*/
#define MCDMUX_FLAGS_PROG_FLAG                  0x00000080  /**<@brief The chunk is from a progressive frame if known @hideinitializer*/
#define MCDMUX_FLAGS_INTERLACED_FLAG            0x00000100  /**<@brief The chunk is from an interlaced frame if known @hideinitializer*/
#define MCDMUX_FLAGS_IDR_FLAG                   0x00000200  /**<@brief The chunk is from an IDR type frame if known @hideinitializer*/
#define MCDMUX_FLAGS_RP_FLAG                    0x00000400  /**<@brief The chunk is from a recovery point type frame if known @hideinitializer*/
#define MCDMUX_FLAGS_LPCM                       0x00001000  /**<@brief LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_DVD_LPCM                   0x00010000  /**<@brief DVD LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_HDMV_LPCM                  0x00020000  /**<@brief HDMV LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_AES3_302M                  0x00040000  /**<@brief AES3 SMPTE 302M LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_AES3_331M                  0x00080000  /**<@brief AES3 SMPTE 331M LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_AES3_382M                  0x00100000  /**<@brief AES3 SMPTE 382M LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_TWOS_LPCM                  0x00200000  /**<@brief TWOS LPCM stream @hideinitializer*/
#define MCDMUX_FLAGS_QT_PCM                     0x00400000  /**<@brief Apple QuickTime PCM stream @hideinitializer*/
#define MCDMUX_FLAGS_AES3_337M_DATA             0x00800000  /**<@brief AES3 SMPTE 337M data stream @hideinitializer*/
#define MCDMUX_FLAGS_AES3_382M_IS_INTERLEAVED   0x01000000  /**<@brief AES3 SMPTE 382M interleved stream @hideinitializer*/
#define MCDMUX_FLAGS_AIFF                       0x02000000  /**<@brief AIFF stream @hideinitializer*/
#define MCDMUX_FLAGS_ALAW                       0x04000000  /**<@brief ALAW stream @hideinitializer*/
/** @} */

struct dmux_chunk_info
{
    uint8_t *ptr;
    int32_t length;
    uint32_t flags;
    int64_t pack_pos;
    int64_t file_pos;
    int64_t PTS;

    // additional data
    // for DVD it contains the extra bytes immediately after the PES header
    // but before the actual audio data (for AC3, LPCM, DTS and SDDS)
    // for AES3 331M PCM audio it contains the first 4 bytes immediately
    // after the PES header but before the actual audio data
    uint8_t info[32];
    uint8_t info_len;
    uint8_t end_of_stream;

    // AES3 specific info
    int32_t aes3_payload_length;
    int32_t aes3_channel;
};


// same as above with added fields
struct dmux_chunk_info_ex
{
    uint8_t *ptr;
    int32_t length;
    uint32_t flags;
    int64_t pack_pos;
    int64_t file_pos;
    int64_t PTS;

    // additional data
    // for DVD it contains the extra bytes immediately after the PES header
    // but before the actual audio data (for AC3, LPCM, DTS and SDDS)
    // for AES3 331M PCM audio it contains the first 4 bytes immediately
    // after the PES header but before the actual audio data
    uint8_t info[32];
    uint8_t info_len;
    uint8_t end_of_stream;

    // AES3 specific info
    int32_t aes3_payload_length;
    int32_t aes3_channel;

    // number of CC discons for this chunk, mp2 only
    uint32_t cc_discon_count;
    uint8_t preroll;
    uint8_t reserved[127];
};


// DMFV_STREAM_INFO
struct dmfv_stream_info
{
    uint8_t  type;
    uint32_t dwRate;              // dwRate / dwScale = framerate
    uint32_t dwScale;             // dwRate / dwScale = framerate
    int32_t  biWidth;             // Width in pixels of the video
    int32_t  biHeight;            // Height in pixels of the video
    int16_t  biBitCount;          // Bits per Pixel
    uint32_t biCodec;
    uint32_t biCompression;
    uint32_t biSizeImage;         //  Size in bytes of the largest video frame
    uint32_t dwInterlaceFlags;
    uint32_t dwPictAspectRatioX;  //  Specifies the horizontal resolution of the target device in pixels per meter
    uint32_t dwPictAspectRatioY;  // Specifies the vertical resolution of the target device in pixels per meter
};


// OUTPUT_SEGMENT_INFO
struct output_segment_info
{
    uint64_t duration;            // duration in 100ns units
    uint64_t length;              // length in bytes

    uint8_t reserved[128];
};

#pragma pack(pop)

////////////////////////////////
//
/** \defgroup auxinfo_group Parser auxillery commands
@{
*/
/** \brief auxillery commands*/
enum parse_aux
{
  PARSE_INIT           = 0x00010000, /**<  clear parser-state, save-buffer etc. Call it after seeking \ingroup hevc_aux_cmd */
  PARSE_FRAMES         = 0x00010001, /**<  set video parser to parse frame-header/data, setup output-frame \ingroup hevc_aux_cmd */
  PARSE_OPTIONS        = 0x00010002, /**<  set decoder options \ingroup hevc_aux_cmd */
  PARSE_AUD_HDR        = 0x00010FFF, /**<  set audio parser to parse sample-header/data */

  GET_PARSE_STATE      = 0x00010003, /**<  Get the actual state of the decoder. This is normally done after a copybyte call  */
  GET_SEQ_PARAMSP      = 0x00010004, /**<  Get the sequence header parameters after decoding is done  */
  GET_SEQ_PARAMSPEX    = 0x00010014, /**<  Get the extended video sequence parameters after decoding is done */
  GET_GOP_PARAMSP      = 0x00010005, /**<  Get group of pictures header after decoding is done  */
  GET_PIC_PARAMSP      = 0x00010006, /**<  Get the picture parameters after decoding is done  */
  GET_PIC              = 0x00010007, /**<  Can be sent to both the subpic and video decoders */
  SET_USER_DATA        = 0x00010008, /**<  Add additional data for the next vs->copybyte call. The user data will be attached to the first frames header and returned with this frame. To get this data after a PARSE_FRAME is completed use the GET_USER_DATAP command. This is normally used for time stamp information. */
  GET_USER_DATAP       = 0x00010009, /**<  Retrieve data, which was previously attached to some data by the SET_USER_DATA command.  */
  // use these messages to get video frames from the decoder
  // with a subpic overlayed
  GET_PIC_WITH_SP      = 0x0001000A, /**<  Send to get a frame with subpic overlayed */
  PARSE_FRAMES_WITH_SP = 0x0001000B, /**<  Set video parser to parse frame-header/data, setup output-frame with subpic overlay */

  PARSE_INIT_AFTER_SEEK= 0x0001000C, /**<    */
  GET_DECODED_PIC_PAR  = 0x0001000D, /**<    */
  GET_DECODED_PIC      = 0x0001000E, /**<    */

  GET_PIC_PARAMSPEX    = 0x0001000F, /**<   Get video picture header and coding extension (MPEG2/AVC only) */

  CLEAN_PARSE_STATE    = 0x00010010, /**<   Does the same as \ref GET_PARSE_STATE and clear parse state */
  GET_PARSE_SETTINGS   = 0x00010011, /**<   Get the parser settings */
  PUT_PARSE_SETTINGS   = 0x00010012, /**<   Put the parser settings  */

  GET_PIC_TYPE         = 0x00010013, /**<   Get picture type */
  PARSE_CLOSE          = 0x00010015, /**<   Close decoder */

  GET_OVERLAY          = 0x00010020, /**<   Overlay a subpic on a frame */
  GET_LAST_PTS         = 0x00010021, /**<   Get the last subpic PTS available */
  GET_PALETTE          = 0x00010022, /**<   Get the subpicture palette, using subpic_palette_tt below */
  // can be sent to both the subpic and video decoders
  SET_PALETTE          = 0x00010023, /**<   Set the subpicture palette, using subpic_palette_tt below */
  // can be sent to both the subpic and video decoders
  SET_SP_PTS           = 0x00010024, /**<   Send the PTS for the current input subpic */
  SET_PIC_PTS          = 0x00010025, /**<   Set the PTS of the frame for which a subpic will be requested */
  // with either GET_PIC or GET_OVERLAY
  INIT_FRAME_PARSER    = 0x00010202, /**<   Init parser (allocate memory etc.) to be able to decode */
  // stream with seq-header defined parameters

  GET_STREAM_USER_DATA = 0x000101B2, /**<    */
  PARSE_SEQ_HDR        = 0x000101B3, /**<   Decode the next data for any header */
//  PARSE_GOP_HDR        = 0x000101B8, //unused
//  SEARCH_PIC_HDR       = 0x00000100, //unused

  GET_SEQ_EXT_PARAMSP  = 0x000101B4, /**<  Get sequence extension header after decoding is done  */

  PARSE_REINIT         = 0x00030001, /**<  Reinitialize wma decoder */
  SET_CPU_NUM          = 0x000101B5, /**<  Sets number of logical CPUs, which can be used in specified SMP mode by the SET_SMP_MODE command  */
  GET_SEI              = 0x000101B6, /**<  Get sei messages after decoding is done. */
  SET_SMP_MODE         = 0x000101B7, /**<  Sets symmetric multiprocessing (SMP) mode of decoding.  */
  GET_SEI_PARAMS       = 0x000101B8, /**<    */
  GET_IN_SEQ_PARAMSP   = 0x000101b9, /**<  Get the sequence header parameters from input elementary stream */

  DEC_AVC_RESERVED_00010081 = 0x00010081,
  DEC_AVC_RESERVED_00010084 = 0x00010084,
  SET_LOOP_FILTER_MODE      = 0x00010085, /**< Set loop filter mode option \hideinitializer */
  DEC_AVC_RESERVED_00010086 = 0x00010086, 
  DEC_AVC_RESERVED_00010087 = 0x00010087,
  DEC_AVC_RESERVED_00010088 = 0x00010088,
  DEC_AVC_RESERVED_00010089 = 0x00010089,
  DEC_AVC_RESERVED_0001008d = 0x0001008d,
  SET_MV_PRECISION          = 0x0001008a, /**< Limits precision of motion vectors option \hideinitializer */
  DEC_AVC_RESERVED_0001008b = 0x0001008b,
  DEC_AVC_RESERVED_0001008c = 0x0001008c,

  SET_DECODE_BASE_LAYERS	= 0x0001008e, /**<  Enable/Disable base layers decoding. SVC decoder's stuff   */
  DEC_AVC_RESERVED_0001008f = 0x0001008f,

  SET_MAX_DQT				= 0x00010300, /**< Set maximum allowed D,Q,T layer. SVC decoder's stuff  */
  GET_LAYER_DQT				= 0x00010301, /**< Get the current layer D,Q,T after decoding is done. SVC decoder's stuff  */
  DEC_AVC_RESERVED_00010302 = 0x00010302,

  DEC_AVC_RESERVED_00010305 = 0x00010305,
  SET_TEMPORAL_ID			= 0x00010306, /**<    */

  // Analysis API
  ANALYSIS_OPTIONS      = 0x00010093, /**<    */
 
  ANALYSIS_BITSTREAM    = 0x00000040, /**<  Coded bitstream extraction */
  ANALYSIS_GET_PIC      = 0x00010094, /**<    */
  ANALYSIS_GET_DPB      = 0x00010095, /**<    */

  VESA_SET_SETTINGS     = 0x00010096, /**<  Set VESA parameters  */

  VESA_GET_BUFFERS      = 0x00010098, /**<  Get VESA buffers after decoding is done. */

  SET_BRIGHTNESS        = 0x00010099, /**<    */
  SET_AFFINITY_MASK     = 0x00010100, /**<    */
  GET_GOP_TIME_CODE     = 0x00010101, /**<    */
  GET_SEQ_DISP_EXT      = 0x00010102, /**<    */
  SET_CROPPING_RECT     = 0x00010103, /**<    */
  GET_BRIGHTNESS        = 0x00010104, /**<    */

  HWACC_GET_HW_CAPS		= 0x00200000, /**<   To enumerate hardware acceleration device */
  HWACC_SET_ACC_MODE    = 0x00200001, /**<   Set hardware acceleration mode */
  HWACC_GET_PIC         = 0x00200002, /**<   Get d3d surface after decoding in hardware mode is done.  */

  GET_PIC_DELAYED		= 0x00300004, /**<  MPEG4 decoder's stuff  */
  GET_DEC_FEATURES		= 0x00300005, /**<  AVC,VC1,MPEG4,MPEG2 decoders' stuff */

  GET_NATIVE_COLORIMETRY = 0x00300008, 
  /*!< \brief Returns colorimetry information for the output picture.

  It is intended to give the user the opportunity to know colorimetry before the frame buffer is converted 
  to output and override it with bufstream#auxinfo
  calls @ref OVERRIDE_NATIVE_COLORIMETRY and @ref SET_OUTPUT_COLORIMETRY \ref if needed.
  By default decoders preserve colorimetry stored in a stream.
\code
bufstream* stream;
colorimetry_t *colorimetry_info;
stream->auxinfo(stream, 0, GET_NATIVE_COLORIMETRY, &colorimetry_info, sizeof(colorimetry_t)); 
\endcode
*/
  OVERRIDE_NATIVE_COLORIMETRY = 0x00300009, 
  /**< \brief Overrides native colorimetry which is specified in a stream for the output picture.

  This feature allows changing invalid or incorrect colorimetry.
  When \ref colorimetry_t is configured with "AUTO" values such as \ref transfer_characteristics_t::TRANSFER_CHARACTERISTICS_AUTO "TRANSFER_CHARACTERISTICS_AUTO", \ref OVERRIDE_NATIVE_COLORIMETRY and
  \ref SET_OUTPUT_COLORIMETRY will update these values with the correct ones from the stream.

  \code
// Example HDR color conversion from PQ to SDR BT.2020
bufstream* decoder;
colorimetry_t native_colorimetry_info;
// initialize the native_colorimetry_info by AUTO values
// all AUTO values will be resolved to the native colorimetry from the output picture
set_colorimetry_from_color_description(&native_colorimetry_info, COLOR_DESCRIPTION_AUTO);

native_colorimetry_info.color_primaries = COLOR_PRIMARIES_BT2020;
// override transfer characteristics to the PQ, without concern to native colorimetry from picture
native_colorimetry_info.transfer_characteristics = TRANSFER_CHARACTERISTICS_SMPTE_ST_2084;
native_colorimetry_info.matrix_coefficients = MATRIX_COEFFICIENTS_BT2020_NON_CONST;
// colorimetry_info.signal_range will be taken from output picture

decoder->auxinfo(decoder, 0, OVERRIDE_NATIVE_COLORIMETRY, &native_colorimetry_info, sizeof(colorimetry_t));
colorimetry_t output_colorimetry;
set_colorimetry_from_color_description(&native_colorimetry_info, COLOR_DESCRIPTION_AUTO);
// set required transfer characteristics from decoder to BT.2020
output_colorimetry.transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2020_10BIT;
decoder->auxinfo(decoder, 0, SET_OUTPUT_COLORIMETRY, &output_colorimetry, sizeof(colorimetry_t));
  \endcode
  */
  
  SET_OUTPUT_COLORIMETRY = 0x00300010  
  /*!< \brief Specifies the desired output colorimetry for the output picture. 

  If native and output colorimetric settings are different, the decoder will apply appropriate color conversions.
  This bufstream#auxinfo command can be called at any time during decoder lifetime and changes will take effect starting
  next \ref bufstream::auxinfo command \ref GET_PIC.
  \note Meta information like \ref display_metadata_t "Mastering Display Color Volume" and \ref content_light_level_t "Content Light Level" will not be used from output colorimetry.

  \image html colorimetry.svg
`colorimetry_t` contains  the following AUTO values in the fields:
- \ref colorimetry_t::color_primaries "color_primaries"                   - \ref color_primaries_t::COLOR_PRIMARIES_AUTO "COLOR_PRIMARIES_AUTO"
- \ref colorimetry_t::transfer_characteristics "transfer_characteristics" - \ref transfer_characteristics_t::TRANSFER_CHARACTERISTICS_AUTO "TRANSFER_CHARACTERISTICS_AUTO"
- \ref colorimetry_t::matrix_coefficients "matrix_coefficients"           - \ref matrix_coefficients_::MATRIX_COEFFICIENTS_AUTO "MATRIX_COEFFICIENTS_AUTO"
- \ref colorimetry_t::signal_range "signal_range"                         - \ref pic_signal_range_flag_t::PIC_SIGNAL_RANGE_AUTO "PIC_SIGNAL_RANGE_AUTO"


  \code
bufstream* stream;
colorimetry_t colorimetry_info;
set_colorimetry_from_color_description(&colorimetry_info, COLOR_DESCRIPTION_AUTO);
// set output transfer characteristics to BT.709
colorimetry_info.transfer_characteristics = TRANSFER_CHARACTERISTICS_BT709_6;
// set output signal range to FULL for showing it on the monitor
colorimetry_info.signal_range = PIC_SIGNAL_RANGE_FULL;
stream->auxinfo(stream, 0, SET_OUTPUT_COLORIMETRY, &colorimetry_info, sizeof(colorimetry_t));
//...
frame_tt frame{};
// initialization the frame
// ...
// Get picture as BT.709
if (BS_OK != stream->auxinfo(stream, 0, GET_PIC, &frame, sizeof(frame)))
    // error handling
  \endcode
  */

};
/** @} */ //end group

#endif // #if !defined (__AUXINFO_INCLUDED__)

