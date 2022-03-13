/**
@file  mcdefs.h
@brief definitions and constants for different codecs

@verbatim
@file  mcdefs.h
@brief definitions and constants for different codecs
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.

MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
@endverbatim
**/


#if !defined (__MC_DEFS_INCLUDED__)
#define __MC_DEFS_INCLUDED__

#include <stddef.h>
#include <mctypes.h>

#include "auxinfo.h"
#include "mccallbacks.h"
#include "mcstreamtypes.h"

/**
 * @name Maximum GOP size limits
 * @{
 **/
#define MAXM               8      ///< Max distance between I/P frames
#define MAXN             128      ///< Max number of frames in Group of Pictures
/** @} */

/**
 * @name MPEG Presets
 * Possible values of @ref mpeg_v_settings::video_type variable
 * @{
 **/
/**
 * @ingroup MPEG2_PRESETS_AND_PROFILES
 * @{
 **/
#define MPEG_MPEG1                 0       /**<@brief generic ISO/IEC 11172-1/2 @hideinitializer */
#define MPEG_VCD                   1       /**< @brief MPEG-1 VideoCD @hideinitializer */
#define MPEG_MPEG2                 2       /**<@brief generic ISO/IEC 13818-1/2 @hideinitializer */
#define MPEG_SVCD                  3       /**<@brief MPEG-2 SuperVCD @hideinitializer */
#define MPEG_DVD                   4       /**<@brief MPEG-2 DVD-Video @hideinitializer */
#define MPEG_TS                    5       /**<@brief generic MPEG-2 transport stream, PAT and PMT only @hideinitializer */
#define MPEG_PVA                   6       /**<@brief PVA stream type, strictly for decoding, do not use for encoding @hideinitializer */
#define MPEG_NONE                  7       /**<@brief indicates no muxing @hideinitializer */
#define MPEG_DVB                   8       /**<@brief DVB @hideinitializer */
#define MPEG_MMV                   9       /**<@brief MPEG-2 MICROMV @hideinitializer */
#define MPEG_DVHS                  10      /**<@brief DVHS @hideinitializer */
#define MPEG_ATSC                  11      /**<@brief MPEG-2 ATSC Terrestrial @hideinitializer */
#define MPEG_ATSCHI                12      /**<@brief MPEG-2 ATSC Terrestrial (high data rate) @hideinitializer */
#define MPEG_DVD_MPEG1             13      /**<@brief MPEG-1 DVD-Video @hideinitializer */
#define MPEG_DVD_DVR               14      /**<@brief MPEG-2 DVD-VR @hideinitializer */
#define MPEG_DVD_DVR_MPEG1         15      /**<@brief MPEG-1 DVD-VR @hideinitializer */
#define MPEG_DVD_PVR               16      /**<@brief MPEG-2 DVD+VR, not yet implemented! @hideinitializer */
#define MPEG_DVD_PVR_MPEG1         17      /**<@brief MPEG-1 DVD+VR, not yet implemented! @hideinitializer */
#define MPEG_HDV_HD1               18      /**<@brief MPEG-2 HDV HD1 @hideinitializer */
#define MPEG_HDV_HD2               19      /**<@brief MPEG-2 HDV HD2 @hideinitializer */
#define MPEG_D10_25                20      /**<@brief D-10 MPEG-2 4:2:2P @ ML (25 MBit) @hideinitializer */
#define MPEG_D10_30                21      /**<@brief D-10 MPEG-2 4:2:2P @ ML (30 MBit) @hideinitializer */
#define MPEG_D10_40                22      /**<@brief D-10 MPEG-2 4:2:2P @ ML (40 MBit) @hideinitializer */
#define MPEG_D10_50                23      /**<@brief D-10 MPEG-2 4:2:2P @ ML (50 MBit) @hideinitializer */
#define MPEG_CABLELABS             24      /**<@brief CableLabs MPEG-2 MP @ ML 480i\@29.97(3.18MBit) @hideinitializer */
#define MPEG_HD_DVD                25      /**<@brief HD DVD @hideinitializer */
#define MPEG_BD                    26      /**<@brief Blu Ray Disc @hideinitializer */
#define MPEG_BD_HDMV               27      /**<@brief Blu Ray Disc (Main Video) @hideinitializer */
#define MPEG_XDCAM_IMX             28      /**<@brief XDCAM IMX @hideinitializer */
#define MPEG_XDCAM_IMX_25          28      /**<@brief XDCAM IMX MPEG-2 4:2:2P @ ML (25 MBit) @hideinitializer */
#define MPEG_XDCAM_IMX_30          29      /**<@brief XDCAM IMX MPEG-2 4:2:2P @ ML (30 MBit) @hideinitializer */
#define MPEG_XDCAM_IMX_40          30      /**<@brief XDCAM IMX MPEG-2 4:2:2P @ ML (40 MBit) @hideinitializer */
#define MPEG_XDCAM_IMX_50          31      /**<@brief XDCAM IMX MPEG-2 4:2:2P @ ML (50 MBit) @hideinitializer */
#define MPEG_XDCAM_HD              32      /**<@brief XDCAM HD @hideinitializer */
#define MPEG_XDCAM_1080_CBR_25     32      /**<@brief XDCAM HD MP \@ H-14 4:2:0 1440x1080 (25 Mbit CBR) @hideinitializer */
#define MPEG_XDCAM_1080_VBR_17_5   33      /**<@brief XDCAM HD MP \@ HL   4:2:0 1440x1080 (17.5 Mbit VBR) @hideinitializer */
#define MPEG_XDCAM_1080_VBR_35     34      /**<@brief XDCAM HD MP \@ HL   4:2:0 1440x1080 (35 Mbit VBR) @hideinitializer */
#define MPEG_XDCAM_540_CBR_12_5    35      /**<@brief XDCAM HD MP \@ H-14 4:2:0 1440x540  (12.5 Mbit CBR) @hideinitializer */
#define MPEG_XDCAM_540_VBR_8_75    36      /**<@brief XDCAM HD MP \@ HL   4:2:0 1440x540  (8.75 Mbit VBR) @hideinitializer */
#define MPEG_XDCAM_540_VBR_17_5    37      /**<@brief XDCAM HD MP \@ HL   4:2:0 1440x540  (17.5 Mbit VBR) @hideinitializer */
#define MPEG_XDCAM_HD_420_1280     38      /**<@brief XDCAM HD MP \@ HL   4:2:0 1280x720  (35 Mbit VBR) @hideinitializer */
#define MPEG_XDCAM_HD_422_1920     39      /**<@brief XDCAM HD 422P \@ HL 4:2:2 1920x1080 (50 Mbit CBR) @hideinitializer */
#define MPEG_XDCAM_HD_422_1280     40      /**<@brief XDCAM HD 422P \@ HL 4:2:2 1280x720  (50 Mbit CBR) @hideinitializer */
#define MPEG_XDCAM_EX              41      /**<@brief XDCAM EX @hideinitializer */
#define MPEG_XDCAM_EX_1920         41      /**<@brief XDCAM EX MP \@ HL   4:2:0 1920x1080 (35 Mbit VBR) @hideinitializer */
#define MPEG_XDCAM_EX_1440         42      /**<@brief XDCAM EX MP \@ H-14 4:2:0 1440x1080 (25 Mbit CBR) @hideinitializer */
#define MPEG_XDCAM_EX_1280         43      /**<@brief XDCAM EX MP \@ HL   4:2:0 1280x720  (35 Mbit VBR) @hideinitializer */
#define MPEG_AVCHD                 44      /**<@brief AVCHD, muxer and pcm encoder type only! @hideinitializer */
#define MPEG_1SEG                  45      /**<@brief OneSeg profile @hideinitializer */
#define MPEG_XDCAM_HD_422_1920_540 46      /**<@brief XDCAM HD 422P \@ HL 4:2:2 1920x540  (25 Mbit CBR), used for slowmotion @hideinitializer */
#define MPEG_ATT                   47      /**<@brief AT&T specs based on the generic ISO 13818-1 TS @hideinitializer */
#define MPEG_CABLELABS_HD1         48      /**<@brief CableLabs MPEG-2 MP \@ HL 1280x720p\@ 59.94 (15MBit) @hideinitializer */
#define MPEG_CABLELABS_HD2         49      /**<@brief CableLabs MPEG-2 MP \@ HL 1920x1080i\@ 29.97 (15MBit) @hideinitializer */
#define MPEG_ATSC_C                50      /**<@brief MPEG-2 ATSC Cable @hideinitializer */
#define MPEG_DTV                   51      /**<@brief Networked Digital Television @hideinitializer */
#define MPEG_HLS                   52      /**<@brief HTTP Live Streaming muxer type @hideinitializer */
#define MPEG_CABLELABS_HD3         53      /**<@brief CableLabs MPEG-2 MP \@ HL 1920x1080p (15MBit) @hideinitializer */

#define MPEG_LAST_MPEG_TYPE        54      /**<@brief last MPEG type @hideinitializer */
/**@}*/
/**@}*/

/**
 * @name Audio types
 * @{
 **/
#define NO_AUDIO               0       ///<   no audio
#define MPEG_AUDIO_LAYER1      1       ///<    ISO/IEC 11172-3 layer 1
#define MPEG_AUDIO_LAYER2      2       ///<    ISO/IEC 11172-3 layer 2
#define MPEG_AUDIO_LAYER3      3       ///<    ISO/IEC 11172-3 layer 3
#define PCM_AUDIO              4       ///<    Depreciated, for compatibility only, means the same as DVD_LPCM_AUDIO
#define DVD_LPCM_AUDIO         4       ///<    DVD Linear PCM
#define AC3_AUDIO              5       ///<    Dolby digital
#define MPEG2_AAC_AUDIO        6       ///<    MPEG-2 AAC
#define MPEG4_AAC_AUDIO        7       ///<    MPEG-4 AAC
#define DTS_AUDIO              8       ///<    Digital Theater Sound
#define SDDS_AUDIO             9       ///<    Sony Dynamic Digital Sound
#define AES3_302M_AUDIO        10      ///<    AES3 PCM audio as per SMPTE 302M, MPEG-2 transport stream only
#define AES3_331M_AUDIO        11      ///<    AES3 PCM audio as per SMPTE 331M, MXF only
#define AES3_382M_AUDIO        12      ///<    AES3 PCM audio as per SMPTE 382M, MXF only
#define HDMV_LPCM_AUDIO        13      ///<    HDMV LPCM audio, Blu Ray and AVCHD
#define TWOS_LPCM_AUDIO        14      ///<    TWOS LPCM audio, Sony Professional Media Card
#define AMR_AUDIO              15      ///<    AMR audio
#define MPEG4_AAC_AUDIO_PSP    16      ///<    MPEG-4 AAC with Sony Play Station Portable restrictions
#define WMA_AUDIO              17      ///<    WMA audio
#define QT_PCM_AUDIO           18      ///<    QuickTime PCM audio
#define ALAW_AUDIO             19      ///<    A-law audio
#define AIFF_AUDIO             20      ///<    AIFF audio
#define STD_LPCM_AUDIO         21      ///<    Standard unformatted LPCM (big endian) audio
#define ULAW_AUDIO             22      ///<    MU-law audio
#define WAVE_ADPCM_AUDIO       23      ///<    WAVE style ADPCM audio, format tag 2
#define WAVE_IMA_ADPCM_AUDIO   24      ///<    WAVE style IMA ADPCM audio, format tag 17
/** @} */


/**
 * @name Options for the XXXChkSettings functions
 * @{
 **/
#define CHECK_MPEG_ONLY       0x00000001  ///< only do MPEG checks, do not check for VCD/SVCD/DVD compliance
#define CHECK_ALL             0x00000002  ///< do all checks, normally the check functions return after the first error found
/** @} */

/* default sector sizes */
#define SVCD_SECTOR_SIZE      2324     // Super VideoCD sector size
#define DVD_SECTOR_SIZE       2048     // DVD sector size
#define TS_SECTOR_SIZE        188      // transport stream sector size
#define MAX_SECTOR_SIZE       4096

/**
 * @name Framerates
 * Supported frame rates
 * @{
 **/
#define FRAMERATE0        0       ///< use default frame rate
#define FRAMERATE23       1       ///< 24000/1001       23.976 fps -- NTSC encapsulated film rate
#define FRAMERATE24       2       ///< 24               Standard international cinema film rate
#define FRAMERATE25       3       ///< 25               PAL (625/50) video frame rate
#define FRAMERATE29       4       ///< 30000/1001       29.97 -- NTSC video frame rate
#define FRAMERATE30       5       ///< 30               NTSC drop-frame (525/60) video frame rate
#define FRAMERATE50       6       ///< 50               double frame rate/progressive PAL
#define FRAMERATE59       7       ///< 60000/1001       double frame rate NTSC
#define FRAMERATE60       8       ///< 60               double frame rate drop-frame NTSC
#define FRAMERATE48       9       ///< 48.0 frames per second (DCI)
/** @} */

/* NTSC pulldown flags */
#define PULLDOWN_NONE        0
#define PULLDOWN_23          1    // 23.976/24 played as 29.97/30
#define PULLDOWN_32          2    // 23.976/24 played as 29.97/30
#define PULLDOWN_AUTO        3
#define PULLDOWN_23_PROGSEQ  4    // 23.976/24 played as 59.94/60
#define PULLDOWN_32_PROGSEQ  5    // 23.976/24 played as 59.94/60
/* PAL pulldown flags */
#define PULLDOWN_22          6    // 25 fps played as 50fps
#define PULLDOWN_22_PROGSEQ  7    // 25 fps played as 50fps

/* muxer frame/timestamp combinations */
#define TIMESTAMPS_ALL    0
#define TIMESTAMPS_IPONLY 1
#define TIMESTAMPS_IONLY  2

/* profile IDs, not original MPEG standard value */
#define PROFILE_MULTIVIEW 8  // currently not supported
#define PROFILE_422       7
#define PROFILE_SIMPLE    5
#define PROFILE_MAIN      4
#define PROFILE_SNR       3  // currently not supported
#define PROFILE_SPAT      2  // currently not supported
#define PROFILE_HIGH      1

/* level IDs */
#define LEVEL_LOW         10
#define LEVEL_MAIN         8
#define LEVEL_HIGH1440     6
#define LEVEL_HIGH         4
#define LEVEL_UNSPECIFIED 15

/** \name video start codes 
 * @{
 */
#define PICTURE_START_CODE        0x100L	    /**< \brief  \hideinitializer  */
#define SLICE_MIN_START           0x101L	    /**< \brief  \hideinitializer  */
#define SLICE_MAX_START           0x1AFL	    /**< \brief  \hideinitializer  */
#define USER_START_CODE           0x1B2L	    /**< \brief  \hideinitializer  */
#define SEQ_START_CODE            0x1B3L	    /**< \brief  \hideinitializer  */
#define EXT_START_CODE            0x1B5L	    /**< \brief  \hideinitializer  */
#define SEQ_END_CODE              0x1B7L	    /**< \brief  \hideinitializer  */
#define GOP_START_CODE            0x1B8L	    /**< \brief  \hideinitializer  */
/** \} */

/* system start codes */
#define ISO_END_CODE              0x1B9L
#define PACK_START_CODE           0x1BAL
#define SYSTEM_START_CODE         0x1BBL

#define PROGRAM_STREAM_MAP        0x1BCL
#define PRIVATE_STREAM_1          0x1BDL
#define PADDING_STREAM            0x1BEL
#define PRIVATE_STREAM_2          0x1BFL

#define AUDIO_ELEMENTARY_STREAM   0x1C0L
#define AUDIO_ELEMENTARY_STREAM0  0x1C0L
#define AUDIO_ELEMENTARY_STREAM1  0x1C1L
#define AUDIO_ELEMENTARY_STREAM2  0x1C2L
#define AUDIO_ELEMENTARY_STREAM3  0x1C3L
#define AUDIO_ELEMENTARY_STREAM4  0x1C4L
#define AUDIO_ELEMENTARY_STREAM5  0x1C5L
#define AUDIO_ELEMENTARY_STREAM6  0x1C6L
#define AUDIO_ELEMENTARY_STREAM7  0x1C7L
#define AUDIO_ELEMENTARY_STREAM8  0x1C8L
#define AUDIO_ELEMENTARY_STREAM9  0x1C9L
#define AUDIO_ELEMENTARY_STREAM10 0x1CAL
#define AUDIO_ELEMENTARY_STREAM11 0x1CBL
#define AUDIO_ELEMENTARY_STREAM12 0x1CCL
#define AUDIO_ELEMENTARY_STREAM13 0x1CDL
#define AUDIO_ELEMENTARY_STREAM14 0x1CEL
#define AUDIO_ELEMENTARY_STREAM15 0x1CFL
#define AUDIO_ELEMENTARY_STREAM16 0x1D0L
#define AUDIO_ELEMENTARY_STREAM17 0x1D1L
#define AUDIO_ELEMENTARY_STREAM18 0x1D2L
#define AUDIO_ELEMENTARY_STREAM19 0x1D3L
#define AUDIO_ELEMENTARY_STREAM20 0x1D4L
#define AUDIO_ELEMENTARY_STREAM21 0x1D5L
#define AUDIO_ELEMENTARY_STREAM22 0x1D6L
#define AUDIO_ELEMENTARY_STREAM23 0x1D7L
#define AUDIO_ELEMENTARY_STREAM24 0x1D8L
#define AUDIO_ELEMENTARY_STREAM25 0x1D9L
#define AUDIO_ELEMENTARY_STREAM26 0x1DAL
#define AUDIO_ELEMENTARY_STREAM27 0x1DBL
#define AUDIO_ELEMENTARY_STREAM28 0x1DCL
#define AUDIO_ELEMENTARY_STREAM29 0x1DDL
#define AUDIO_ELEMENTARY_STREAM30 0x1DEL
#define AUDIO_ELEMENTARY_STREAM31 0x1DFL

#define VIDEO_ELEMENTARY_STREAM   0x1E0L
#define VIDEO_ELEMENTARY_STREAM0  0x1E0L
#define VIDEO_ELEMENTARY_STREAM1  0x1E1L
#define VIDEO_ELEMENTARY_STREAM2  0x1E2L
#define VIDEO_ELEMENTARY_STREAM3  0x1E3L
#define VIDEO_ELEMENTARY_STREAM4  0x1E4L
#define VIDEO_ELEMENTARY_STREAM5  0x1E5L
#define VIDEO_ELEMENTARY_STREAM6  0x1E6L
#define VIDEO_ELEMENTARY_STREAM7  0x1E7L
#define VIDEO_ELEMENTARY_STREAM8  0x1E8L
#define VIDEO_ELEMENTARY_STREAM9  0x1E9L
#define VIDEO_ELEMENTARY_STREAM10 0x1EAL
#define VIDEO_ELEMENTARY_STREAM11 0x1EBL
#define VIDEO_ELEMENTARY_STREAM12 0x1ECL
#define VIDEO_ELEMENTARY_STREAM13 0x1EDL
#define VIDEO_ELEMENTARY_STREAM14 0x1EEL
#define VIDEO_ELEMENTARY_STREAM15 0x1EFL

#define PROGRAM_STREAM_DIRECTORY  0x1FFL
#define SYSTEM_START_CODE_MAX     0x1FFL

/* subpicture streams */
#define SUBPICTURE_STREAM0        0x20
#define SUBPICTURE_STREAM1        0x21
#define SUBPICTURE_STREAM2        0x22
#define SUBPICTURE_STREAM3        0x23
#define SUBPICTURE_STREAM4        0x24
#define SUBPICTURE_STREAM5        0x25
#define SUBPICTURE_STREAM6        0x26
#define SUBPICTURE_STREAM7        0x27
#define SUBPICTURE_STREAM8        0x28
#define SUBPICTURE_STREAM9        0x29
#define SUBPICTURE_STREAM10       0x2A
#define SUBPICTURE_STREAM11       0x2B
#define SUBPICTURE_STREAM12       0x2C
#define SUBPICTURE_STREAM13       0x2D
#define SUBPICTURE_STREAM14       0x2E
#define SUBPICTURE_STREAM15       0x2F
#define SUBPICTURE_STREAM16       0x30
#define SUBPICTURE_STREAM17       0x31
#define SUBPICTURE_STREAM18       0x32
#define SUBPICTURE_STREAM19       0x33
#define SUBPICTURE_STREAM20       0x34
#define SUBPICTURE_STREAM21       0x35
#define SUBPICTURE_STREAM22       0x36
#define SUBPICTURE_STREAM23       0x37
#define SUBPICTURE_STREAM24       0x38
#define SUBPICTURE_STREAM25       0x39
#define SUBPICTURE_STREAM26       0x3A
#define SUBPICTURE_STREAM27       0x3B
#define SUBPICTURE_STREAM28       0x3C
#define SUBPICTURE_STREAM29       0x3D
#define SUBPICTURE_STREAM30       0x3E
#define SUBPICTURE_STREAM31       0x3F

/* AC3 streams */
#define AC3_STREAM0               0x80
#define AC3_STREAM1               0x81
#define AC3_STREAM2               0x82
#define AC3_STREAM3               0x83
#define AC3_STREAM4               0x84
#define AC3_STREAM5               0x85
#define AC3_STREAM6               0x86
#define AC3_STREAM7               0x87

/* DTS streams */
#define DTS_STREAM0               0x88
#define DTS_STREAM1               0x89
#define DTS_STREAM2               0x8A
#define DTS_STREAM3               0x8B
#define DTS_STREAM4               0x8C
#define DTS_STREAM5               0x8D
#define DTS_STREAM6               0x8E
#define DTS_STREAM7               0x8F

/* SDDS streams */
#define SDDS_STREAM0              0x90
#define SDDS_STREAM1              0x91
#define SDDS_STREAM2              0x92
#define SDDS_STREAM3              0x93
#define SDDS_STREAM4              0x94
#define SDDS_STREAM5              0x95
#define SDDS_STREAM6              0x96
#define SDDS_STREAM7              0x97

/* PCM streams */
#define PCM_STREAM0               0xA0
#define PCM_STREAM1               0xA1
#define PCM_STREAM2               0xA2
#define PCM_STREAM3               0xA3
#define PCM_STREAM4               0xA4
#define PCM_STREAM5               0xA5
#define PCM_STREAM6               0xA6
#define PCM_STREAM7               0xA7

/** \name picture structure 
  * @{
  */

#define TOP_FIELD     1	    /**< \brief  \hideinitializer  */
#define BOTTOM_FIELD  2	    /**< \brief  \hideinitializer  */
#define FRAME_PICTURE 3	    /**< \brief  \hideinitializer  */
/** @} */

/* extension start code IDs */
#define SEQ_ID       1
#define DISP_ID      2
#define QUANT_ID     3
#define SEQSCAL_ID   5
#define PANSCAN_ID   7
#define CODING_ID    8
#define SPATSCAL_ID  9
#define TEMPSCAL_ID 10

/** \name picture coding type 
  * @{
  */
#define I_TYPE 1	    /**< \brief  \hideinitializer  */
#define P_TYPE 2	    /**< \brief  \hideinitializer  */
#define B_TYPE 3	    /**< \brief  \hideinitializer  */
#define D_TYPE 4	    /**< \brief  \hideinitializer  */
/** @} */

/** \name macroblock type 
  * @{
  */
#define MB_INTRA    1	    /**< \brief  \hideinitializer  */
#define MB_PATTERN  2	    /**< \brief  \hideinitializer  */
#define MB_BACKWARD 4	    /**< \brief  \hideinitializer  */
#define MB_FORWARD  8	    /**< \brief  \hideinitializer  */
#define MB_QUANT    16	    /**< \brief  \hideinitializer  */
/** @} */

/** \name motion_type 
  * @{
  */
#define MC_FIELD 1	    /**< \brief  \hideinitializer  */
#define MC_FRAME 2	    /**< \brief  \hideinitializer  */
#define MC_16X8  2	    /**< \brief  \hideinitializer  */
#define MC_DMV   3	    /**< \brief  \hideinitializer  */
/** @} */

/** \name mv_format  
  * @{
  */
#define MV_FIELD 0	    /**< \brief  \hideinitializer  */
#define MV_FRAME 1	    /**< \brief  \hideinitializer  */
/** @} */

/** \name chroma_format  
  * @{
  */
#define CHROMA_UNKNOWN -1   /**< \brief  \hideinitializer  */
#define CHROMA400 0	        /**< \brief  \hideinitializer  */
#define CHROMA420 1	        /**< \brief  \hideinitializer  */
#define CHROMA422 2	        /**< \brief  \hideinitializer  */
#define CHROMA444 3	        /**< \brief  \hideinitializer  */
#define CHROMA211 4	        /**< \brief  \hideinitializer  */
#define CHROMA410 5	        /**< \brief  \hideinitializer  */
#define CHROMA411 6	        /**< \brief  \hideinitializer  */
#define CHROMA810 7	        /**< \brief  \hideinitializer  */
/** @} */

#define SEQHDR_FLAG             0x01
#define GOPHDR_FLAG             0x02
#define SVCD_USER_BLOCK         0x04
#define SEQ_USER_BLOCK          0x08
#define PIC_USER_BLOCK          0x10
#define AU_FLAG                 0x40
#define GOP_USER_BLOCK          0x80

/**
 * @name MPEG Audio mode
 * @{
 **/
#define MPG_MD_STEREO           0			///< Stereo mode
#define MPG_MD_JOINT_STEREO     1			///< Allow joint stereo (intensity stereo) mode
#define MPG_MD_DUAL_CHANNEL     2			///< Dual channel mode
#define MPG_MD_MONO             3			///< Single channel (mono) mode
/** @} */

/**
 * @name MPEG input return codes
 * @{
 **/
#define mpegInErrNone     0       ///< no error
#define mpegInError       1       ///< undefined error
#define mpegInEOF         2       ///< end of file error
/** @} */

// for mpegOutVideoDefaults(mpeg_v_settings *set, int video_type, int PAL);
#define VM_NTSC           0
#define VM_PAL            1


/**
 * @name MPEG-1 layer 1 bit rates
 * @{
 **/
#define L1_AUDIOBITRATE32    1			///< 32 kbit/sec
#define L1_AUDIOBITRATE64    2			///< 64 kbit/sec
#define L1_AUDIOBITRATE96    3			///< 96 kbit/sec
#define L1_AUDIOBITRATE128   4			///< 128 kbit/sec
#define L1_AUDIOBITRATE160   5			///< 160 kbit/sec
#define L1_AUDIOBITRATE192   6			///< 192 kbit/sec
#define L1_AUDIOBITRATE224   7			///< 224 kbit/sec
#define L1_AUDIOBITRATE256   8			///< 256 kbit/sec
#define L1_AUDIOBITRATE288   9			///< 288 kbit/sec
#define L1_AUDIOBITRATE320   10			///< 320 kbit/sec
#define L1_AUDIOBITRATE352   11			///< 352 kbit/sec
#define L1_AUDIOBITRATE384   12			///< 384 kbit/sec
#define L1_AUDIOBITRATE416   13			///< 416 kbit/sec
#define L1_AUDIOBITRATE448   14			///< 448 kbit/sec
/** @} */


/**
 * @name MPEG-1 layer 2 bit rates
 * @{
 **/
#define L2_AUDIOBITRATE32    1			///< 32 kbit/sec
#define L2_AUDIOBITRATE48    2			///< 48 kbit/sec
#define L2_AUDIOBITRATE56    3			///< 56 kbit/sec
#define L2_AUDIOBITRATE64    4			///< 64 kbit/sec
#define L2_AUDIOBITRATE80    5			///< 80 kbit/sec
#define L2_AUDIOBITRATE96    6			///< 96 kbit/sec
#define L2_AUDIOBITRATE112   7			///< 112 kbit/sec
#define L2_AUDIOBITRATE128   8			///< 128 kbit/sec
#define L2_AUDIOBITRATE160   9			///< 160 kbit/sec
#define L2_AUDIOBITRATE192   10			///< 192 kbit/sec
#define L2_AUDIOBITRATE224   11			///< 224 kbit/sec
#define L2_AUDIOBITRATE256   12			///< 256 kbit/sec
#define L2_AUDIOBITRATE320   13			///< 320 kbit/sec
#define L2_AUDIOBITRATE384   14			///< 384 kbit/sec
/** @} */


/**
 * @name MPEG-2 LSF layer 1 bit rates
 * @{
 **/
#define LSF_L1_AUDIOBITRATE32    1			///< 32 kbit/sec
#define LSF_L1_AUDIOBITRATE48    2			///< 48 kbit/sec
#define LSF_L1_AUDIOBITRATE56    3			///< 56 kbit/sec
#define LSF_L1_AUDIOBITRATE64    4			///< 64 kbit/sec
#define LSF_L1_AUDIOBITRATE80    5			///< 80 kbit/sec
#define LSF_L1_AUDIOBITRATE96    6			///< 96 kbit/sec
#define LSF_L1_AUDIOBITRATE112   7			///< 112 kbit/sec
#define LSF_L1_AUDIOBITRATE128   8			///< 128 kbit/sec
#define LSF_L1_AUDIOBITRATE144   9			///< 144 kbit/sec
#define LSF_L1_AUDIOBITRATE160   10			///< 160 kbit/sec
#define LSF_L1_AUDIOBITRATE176   11			///< 176 kbit/sec
#define LSF_L1_AUDIOBITRATE192   12			///< 192 kbit/sec
#define LSF_L1_AUDIOBITRATE224   13			///< 224 kbit/sec
#define LSF_L1_AUDIOBITRATE256   14			///< 256 kbit/sec
/** @} */


/**
 * @name MPEG-2 LSF layer 2 bit rates
 * @{
 **/
#define LSF_L2_AUDIOBITRATE8     1			///< 8 kbit/sec
#define LSF_L2_AUDIOBITRATE16    2			///< 16 kbit/sec
#define LSF_L2_AUDIOBITRATE24    3			///< 24 kbit/sec
#define LSF_L2_AUDIOBITRATE32    4			///< 32 kbit/sec
#define LSF_L2_AUDIOBITRATE40    5			///< 40 kbit/sec
#define LSF_L2_AUDIOBITRATE48    6			///< 48 kbit/sec
#define LSF_L2_AUDIOBITRATE56    7			///< 56 kbit/sec
#define LSF_L2_AUDIOBITRATE64    8			///< 64 kbit/sec
#define LSF_L2_AUDIOBITRATE80    9			///< 80 kbit/sec
#define LSF_L2_AUDIOBITRATE96    10			///< 96 kbit/sec
#define LSF_L2_AUDIOBITRATE112   11			///< 112 kbit/sec
#define LSF_L2_AUDIOBITRATE128   12			///< 128 kbit/sec
#define LSF_L2_AUDIOBITRATE144   13			///< 144 kbit/sec
#define LSF_L2_AUDIOBITRATE160   14			///< 160 kbit/sec
/** @} */


/**
 * @name Bitrate mode
 *Defines for mpeg_v_settings::constant_bitrate value used for both the video encoder and the multiplexer
 * @{
 **/
#define VBR_CONSTANT      1                 ///< constant bitrate (default in all presets)
#define VBR_VARIABLE1     0                 ///< variable bitrate
//#define VBR_VARIABLE2     2   // was never really used
//#define VBR_VARIABLE3     3   // was never really used
#define VBR_CQT           2                 ///< constant quantization (see @ref mpeg_v_settings::mquant_value settings)
#define VBR_CQT_STRICT    3                 ///< strict constant quantization (see @ref mpeg_v_settings::mquant_value settings)
/** @} */

/**
 * @name Extended option defines
 * @brief for passing to @ref h264OutVideoInit, @ref hevcOutVideoPutFrame, etc.
 * @{
 **/
typedef enum opt_ext_id_e
{
    EXT_OPT_PARAM_NULL = 0,             /**<@brief End of extended options array. */
    EXT_OPT_PARAM_FRM_ORG = 1,              /**<@brief Show original frame. */
    EXT_OPT_CHAPTER_LIST = 2 ,               /**<@brief Chapter list option. This option can be passed to @ref h264OutVideoInit to force I frame at certain point in the stream. Please refer to @ref ENC_AVC_CHAPTER_LIST_PAGE. */
    EXT_OPT_PARAM_FRM_ENC = 3,              /**<@brief Show encoded frame. */
    EXT_OPT_PARAM_SHOW_ONLY = 4,            /**<@brief (MPEG-1/2 encoder only) Don't encode, show only. */
    EXT_OPT_PARAM_SEQ_UD = 5,               /**<@brief Write GOP user data. */
    EXT_OPT_PARAM_PIC_UD = 6,               /**<@brief Write picture user data. */
    EXT_OPT_PARAM_SR_DATA = 7,              /**<@brief Smart render data. */
    EXT_OPT_PARAM_RAW_DATA = 8,             /**<@brief (MPEG-1/2 encoder only) Smart render data. */
    EXT_OPT_PARAM_CONTENT = 9,              /**<@brief (MPEG-1/2 encoder only) Data about current frame-content. */
    EXT_OPT_PARAM_ALPHA_PLANE = 1,          /**<@brief Add alpha plane. */
    EXT_OPT_PARAM_VCSD_DATA = 11,            /**<@brief (MPEG-1/2 encoder only) Video content scene detection data. */
    EXT_OPT_PARAM_STOP_DATA = 12,            /**<@brief (MPEG-1/2 encoder only) Not implemented . */
    EXT_OPT_PARAM_FORCE_IDR = 13,            /**<@brief For a current frame forces it to be an IDR (H.264/AVC and H.265/HEVC encoders only) respecting chapter list and minimal IDR interval. */
    EXT_OPT_PARAM_TARGET_VBV = 14,           /**<@brief Target a specific VBV state within given frame interval. */
    EXT_OPT_PARAM_CONSTRAIN_VBV = 15,        /**<@brief (MPEG-1/2 encoder only) Stay beyond a specific VBV state within given frame interval. */
    EXT_OPT_PARAM_CPB_DELAY = 17,            /**<@brief CPB removal delay value to insert into Picture Timing SEI of correspond IDR. */
    EXT_OPT_PARAM_TIMESTAMPS = 18,           /**<@brief Specify PTS for the frame */
    EXT_OPT_PARAM_BITRATE = 19,              /**<@brief (MPEG-1/2 encoder only) For changing bitrate "on the fly" (during encoding). */
    EXT_OPT_PARAM_FIELDS_DROPPED = 20,       /**<@brief Specify number of dropped fields (e.g. by capture device), used to adjust timecode. */
    EXT_OPT_PARAM_DISPLAY_MODE = 21,         /**<@brief Specify frame display mode (frame, top field-bottom field, etc.), used in @ref VIDEO_PULLDOWN_AUTO mode. */
    EXT_OPT_INSERT_FRAME = 22,               /**<@brief Pad skipped frame. */
    EXT_OPT_SET_PREVIEW = 24,                /**<@brief Sets preview callback routine for Smart rendering. */
    EXT_OPT_USER_BUFFER_FULLNESS = 25,       /**<@brief User side source buffer fullness, if present. */
    EXT_OPT_METADATA_MPASS = 26,             /**<@brief @ref ENC_HEVC_MULTI_PASS "Multi-pass" encoding statistic metadata storage object for H.265 /HEVC Encoder. */
    EXT_OPT_EXTERN_FRM_BUF = 27,             /**<@brief (MPEG-1/2 encoder only) Use external frame buffer, I420 and YV12 colorspaces. */
    EXT_OPT_NR_FILT = 28,                    /**<@brief (MPEG-1/2 encoder only) Use preprocessing noise-reduction filter. */
    EXT_OPT_UD_FIRST_SEQHDR = 29,            /**<@brief (MPEG-1/2 encoder only) Put user data after the first sequence header. */
    EXT_OPT_UD_ALL_SEQHDR = 30,              /**<@brief (MPEG-1/2 encoder only) Put user data after all sequence headers. */
    EXT_OPT_FAST_SR = 31,                    /**<@brief (MPEG-1/2 encoder only) Do fast smartrendering (SR copy), if SR is possible. */
    /** @cond */
    EXT_OPT_SR_ON = 32,                      // For internal use only
    /** @endcond */
    EXT_OPT_FIELD_ORDER = 33,                /**<@brief (MPEG-1/2 encoder only) Specify field order for current source frame. */
    EXT_OPT_SKIP = 34,                       /**<@brief (MPEG-1/2 encoder only) Online frame skipping options. */
    EXT_OPT_INV_AUTO_GOP = 35,               /**<@brief (MPEG-1/2 encoder only) Invert of automatic GOP placement feature. */
    EXT_OPT_LINE_FILTER = 36,                /**<@brief (MPEG-1/2 encoder only) Preprocessing line filter. */
    EXT_OPT_START_PAR = 37,                  /**<@brief (MPEG-1/2 encoder only) Starting parameters */
    EXT_OPT_PARAM_PIC_STRUCT = 38,           /**<@brief (H.264/AVC encoder only) Specify picture structure for the current input frame */
    EXT_OPT_SEI_PIC_TIMING = 39,             /**<@brief (H.264/AVC encoder only) Specify picture timing SEI message structure for current input frame */
} ext_opt_id;

/** @} */

#define PCM_EMPHASIS_OFF  0
#define PCM_EMPHASIS_ON   1

#define PCM_MUTE_OFF      0
#define PCM_MUTE_ON       1

#define PCM_16BITS        0
#define PCM_20BITS        1
#define PCM_24BITS        2

#define PCM_NO_DYNAMIC_RANGE_CONTROL  0x80

#define INV_VCD_AUD_ERRORS             0x00000200
#define INV_VCD_AUD_LAYER              INV_VCD_AUD_ERRORS + 0
#define INV_VCD_AUD_MODE               INV_VCD_AUD_ERRORS + 1
#define INV_VCD_AUD_BITRATE            INV_VCD_AUD_ERRORS + 2
#define INV_VCD_AUD_SAMPLE_RATE        INV_VCD_AUD_ERRORS + 3
#define INV_VCD_AUD_TYPE               INV_VCD_AUD_ERRORS + 4

#define INV_SVCD_AUD_ERRORS            0x00001000
#define INV_SVCD_AUD_LAYER             INV_SVCD_AUD_ERRORS + 0
#define INV_SVCD_AUD_EPROTECTION       INV_SVCD_AUD_ERRORS + 1
#define INV_SVCD_AUD_EMPHASIS          INV_SVCD_AUD_ERRORS + 2
#define INV_SVCD_AUD_BITRATE           INV_SVCD_AUD_ERRORS + 3
#define INV_SVCD_AUD_SAMPLE_RATE       INV_SVCD_AUD_ERRORS + 4
#define INV_SVCD_AUD_TYPE              INV_SVCD_AUD_ERRORS + 5

#define INV_DVD_AUD_ERRORS             0x00008000
#define INV_DVD_AUD_LAYER              INV_DVD_AUD_ERRORS + 0
#define INV_DVD_AUD_EPROTECTION        INV_DVD_AUD_ERRORS + 1
#define INV_DVD_AUD_EMPHASIS           INV_DVD_AUD_ERRORS + 2
#define INV_DVD_AUD_PRIVATEBIT         INV_DVD_AUD_ERRORS + 3
#define INV_DVD_AUD_BITRATE            INV_DVD_AUD_ERRORS + 4
#define INV_DVD_AUD_MODE               INV_DVD_AUD_ERRORS + 5
#define INV_DVD_AUD_SAMPLE_RATE        INV_DVD_AUD_ERRORS + 6
#define INV_DVD_AUD_DVR_PCM_CHANNELS   INV_DVD_AUD_ERRORS + 7
#define INV_DVD_AUD_DVR_PCM_QUANT      INV_DVD_AUD_ERRORS + 8
#define INV_DVD_AUD_DVR_PCM_FREQUENCY  INV_DVD_AUD_ERRORS + 9
#define INV_DVD_AUD_TYPE               INV_DVD_AUD_ERRORS + 10
#define INV_DVD_AUD_ENDIAN             INV_DVD_AUD_ERRORS + 11

#define INV_DVB_AUD_ERRORS             0x00012100
#define INV_DVB_AUD_LAYER              INV_DVB_AUD_ERRORS + 0
#define INV_DVB_AUD_EPROTECTION        INV_DVB_AUD_ERRORS + 1
#define INV_DVB_AUD_EMPHASIS           INV_DVB_AUD_ERRORS + 2
#define INV_DVB_AUD_TYPE               INV_DVB_AUD_ERRORS + 3

#define INV_MMV_AUD_ERRORS             0x00013100
#define INV_MMV_AUD_LAYER              INV_MMV_AUD_ERRORS + 0
#define INV_MMV_AUD_EPROTECTION        INV_MMV_AUD_ERRORS + 1
#define INV_MMV_AUD_EMPHASIS           INV_MMV_AUD_ERRORS + 2
#define INV_MMV_AUD_SAMPLE_RATE        INV_MMV_AUD_ERRORS + 3
#define INV_MMV_AUD_BITRATE            INV_MMV_AUD_ERRORS + 4
#define INV_MMV_AUD_TYPE               INV_MMV_AUD_ERRORS + 5

#define INV_DVHS_AUD_ERRORS            0x00014100
#define INV_DVHS_AUD_LAYER             INV_DVHS_AUD_ERRORS + 0
#define INV_DVHS_AUD_EPROTECTION       INV_DVHS_AUD_ERRORS + 1
#define INV_DVHS_AUD_EMPHASIS          INV_DVHS_AUD_ERRORS + 2
#define INV_DVHS_AUD_BITRATE           INV_DVHS_AUD_ERRORS + 3
#define INV_DVHS_AUD_CMODE             INV_DVHS_AUD_ERRORS + 9
#define INV_DVHS_AUD_TYPE              INV_DVHS_AUD_ERRORS + 10

#define INV_ATSC_AUD_ERRORS            0x00015100
#define INV_ATSC_AUD_TYPE              INV_ATSC_AUD_ERRORS + 0

#define INV_HDV_AUD_ERRORS             0x00016100
#define INV_HDV_AUD_LAYER              INV_HDV_AUD_ERRORS + 0
#define INV_HDV_AUD_EPROTECTION        INV_HDV_AUD_ERRORS + 1
#define INV_HDV_AUD_EMPHASIS           INV_HDV_AUD_ERRORS + 2
#define INV_HDV_AUD_SAMPLE_RATE        INV_HDV_AUD_ERRORS + 3
#define INV_HDV_AUD_BITRATE            INV_HDV_AUD_ERRORS + 4
#define INV_HDV_AUD_TYPE               INV_HDV_AUD_ERRORS + 6

// defines for MPEG conformance check errors
#define INV_MPEG_ERRORS                0x00020000

// defines for MPEG audio conformance check errors
#define INV_AUD_ERRORS                 0x00040000
#define INV_AUD_MODE                   INV_AUD_ERRORS + 0
#define INV_AUD_LAYER                  INV_AUD_ERRORS + 1
#define INV_AUD_PSYCH                  INV_AUD_ERRORS + 2
#define INV_AUD_BITRATE                INV_AUD_ERRORS + 3
#define INV_AUD_EMPHASIS               INV_AUD_ERRORS + 4
#define INV_AUD_PRIVATEBIT             INV_AUD_ERRORS + 5
#define INV_AUD_ERROR_PROT             INV_AUD_ERRORS + 6
#define INV_AUD_COPYRIGHT              INV_AUD_ERRORS + 7
#define INV_AUD_ORIGINAL               INV_AUD_ERRORS + 8
#define INV_AUD_SAMPLE_RATE            INV_AUD_ERRORS + 9

//-- AAC audio defines ----------------------------------------------

/**
 * @name Audio object type (mpeg-4 version)
 * @{
 **/
#define AAC_LC                2 ///< use "low complexity" audio object
/**
 * @name The audio object types below are applied to Fraunhofer AAC Encoder only
 * @{
 **/
#define AAC_HEAAC             5 ///< AAC LC + SBR
#define AAC_ER_LC            17 ///< ER AAC LC
#define AAC_PS               29 ///< AAC LC + SBR + PS
#define AAC_MP2_LC          129 ///< virtual AOT MP2 Low Complexity Profile
#define AAC_MP2_SBR         132 ///< virtual AOT MP2 Low Complexity Profile with SBR
#define AAC_SBR_DS          133 ///< virtual AOT for downsampled SBR
/** @} */

//---- MPD Combiner defines------------------------------------------------
/**
 * @name MPD Combiner errors
 * @{
*/
#define mpdCombinerErrNone          0   ///< No error
#define mpdCombinerEmptyInstance    1   ///< Error: empty instance
#define mpdCombinerInvalidNumMpd    2   ///< Error: invalid number of input mpd files
#define mpdCombinerInvalidInputMpd  3   ///< Error: invalid input names of mpd files
#define mpdCombinerErrorMpdRead     4   ///< Error of mpd file reading
#define mpdCombinerErrorMpdCreation 5   ///< Error of mpd file creation
#define mpdCombinerErrorMpdWrite    6   ///< Error of file writing
#define mpdCombinerDifSegmDuration      7   ///< Error: different segment durations in one Adaptation Set
/** @} */

/** \defgroup video_dec_mpeg_group MPEG Video Decoder defines
\{
*/
/**\brief parser return code flags

\attention if doing reordering output picture params are related to reordered picture and not to current (in stream) picture

FRAME_BUFFERED_FLAG means: once more frame is available.
\arg first I-frame parced: VALID_FLAG is not set, FRAME_BUFFERED_FLAG is set.
\arg current B-Frame:      VALID_FLAG is set,     FRAME_BUFFERED_FLAG is set.
\arg last Frame: FRAME_BUFFERED_FLAG is set
\arg To peek buffered frames one need to repeat
     PARCE_FRAME-copybyte(NULL,0)-GET_STSTUS sequence
     while FRAME_BUFFERED_FLAG is set or VALID_FLAG is set

*/
enum parse_flags
{
  PARSE_DONE_FLAG   = 0x00000001, /**< last parse operation was OK, current stream state related */
  PARSE_ERR_FLAG    = 0x00000002, /**< error in stream detected */

  SEQ_HDR_FLAG      = 0x00000100, /**<  next flags are related to */
  EXT_CODE_FLAG     = 0x00000200, /**<  current stream state and means parsed */
  GOP_HDR_FLAG      = 0x00000400, /**<  elements of MPEG syntax */
  PIC_HDR_FLAG      = 0x00000800, /**<  elements of MPEG syntax */

  USER_DATA_FLAG    = 0x00001000, /**<  elements of MPEG syntax */
  SEQ_END_FLAG      = 0x00002000, /**<  elements of MPEG syntax */
  SLICE_START_FLAG  = 0x00004000, /**<  elements of MPEG syntax */
  UNKNOWN_CODE_FLAG = 0x00008000, /**<  elements of MPEG syntax */
  START_CODE_FLAG   = 0x00010000, /**<  elements of MPEG syntax */




  PIC_DECODED_FLAG  = 0x00100000, /**<   */
  PIC_FULL_FLAG     = 0x00200000, /**<   */
  PIC_VALID_FLAG    = 0x00400000, /**<   */
  FRAME_BUFFERED_FLAG=0x00800000, /**<  show that picture is in reordering buffer */
  PIC_ERROR_FLAG    = 0x01000000, /**<  output picture has some errors */
  PIC_MV_ERROR_FLAG = 0x02000000, /**<  output picture has motion-vector errors (clipped in decoder) */

  SEI_FLAG          = 0x04000000, /**<  elements of MPEG syntax */
  VID_HDR_FLAG      = 0x08000000, /**<  elements of MPEG syntax */

  INTERNAL_ERROR    = 0x10000000, /**<  out of memory*/
  PIC_REF_ERROR_FLAG= 0x20000000  /**<  output picture has errors on reference pictures. */
};
/** \} */

#ifndef __APPLE__
#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif
#endif


/** \addtogroup video_dec_mpeg_group
 @{
*/
////////////////////////////////
/** \brief Structure describing video sequence header. Each field's meaning depends on the codec used.

 */
struct SEQ_Params
{
/**
<table>
<tr> <td><b> AVC </b></td>		<td> horizontal size of video. </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> horizontal size of video. </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> horizontal size of video.</td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>

</table>
*/
  int horizontal_size;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> vertical size of video. </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> vertical size of video. </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vertical size of video.</td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vertical_size;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> \arg 0 -  forbidden;	\arg 1 - 1:1 (square) \arg 2 - 12:11 \arg 3 - 10:11 \arg 4 - 16:11 \arg 5 - 40:33 \arg 6 to 14 - reserved \arg 15 - extended pixel aspect ratio</td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int aspect_ratio_information;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_rate_code;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int bit_rate_value;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vbv_buffer_size;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int constrained_parameters_flag;

};

/** \name brief video sequence header.  Fields' meaning depend on codec used.
\{
 */
struct SEQ_ParamsEx
{

/**
<table>
<tr> <td><b> AVC </b></td>		<td> horizontal size of video </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> horizontal size of video  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> horizontal size of video </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> horizontal size of video </td> </tr>
</table>
*/
  int horizontal_size;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> vertical size of video </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> vertical size of video  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vertical size of video </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> vertical size of video </td> </tr>
</table>
*/
  int vertical_size;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> aspect ratio indicator (table E-1)  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> defines the value of pixel aspect ratio (see values and their interpretation in SEQ_Params description) </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int aspect_ratio_information;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_rate_code;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int bit_rate_value;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> the decoder counts this value as \code ((first_half_vbv_buffer_size << 3) | latter_half_vbv_buffer_size) *16384; \endcode where first_half_vbv_buffer_size, latter_half_vbv_buffer_size are values from MPEG-4 video object layer bitstream </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vbv_buffer_size;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> combination of stream's constraint flags (bit 4 - constraint_set3_flag, bit 5 - constraint_set2_flag, ... , bit 7 - constraint_set0_flag)  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int constrained_parameters_flag;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> horizontal aspect ratio</td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> par_width - pixel aspect ratio from video object layer header when  aspect_ratio_information = 15 (extended par)  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int aspect_ratio_width;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> vertical aspect ratio</td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> par_height - pixel aspect ratio from video object layer header when  aspect_ratio_information = 15 (extended par)  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int aspect_ratio_height;
  // Those fields used if frame_rate_code == 0. frame_rate = (double)scale/(double)units.
/**
<table>
<tr> <td><b> AVC </b></td>		<td> Time scale  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Time scale   </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vop_time_increment_resolution (number of ticks in one second). </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> Time scale  </td> </tr>
</table>
*/
  int scale;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> Number of units in tick  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Number units in tick.   </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> fixed_vop_time_increment (represents the number of ticks between two successive frames in the display order). </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> Number units in tick. </td> </tr>
</table>
*/
  int units;

/**
<table>
<tr> <td><b> HEVC </b></td>	<td> Specifies the maximum number of temporal sub-layers that may be present in each CVS referring to the SPS. </td> </tr>
</table>
*/
	int max_sub_layers;
};
/**\}*/

/** \brief video sequnce extension

 ISO/IEC 13818-2 section 6.2.2.3: sequence_extension
 */
struct SEQ_Ext_Params
{
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int profile_and_level_indication;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> frame_mbs_only_flag </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int progressive_sequence;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> chroma_format_idc flag </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int chroma_format;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int horizontal_size_extension;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vertical_size_extension;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int bit_rate_extension;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vbv_buffer_size_extension;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> LOW_LATENCY_FLAG presenting </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int low_delay;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_rate_extension_n;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.3.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_rate_extension_d;
};

/** \brief sequence display extension

 ISO/IEC 13818-2 section 6.3.6: sequence display extension
 */
struct SEQ_Disp_Ext_Params
{
  int video_format; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */
  int colour_description; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */
  int colour_primaries; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */
  int transfer_characteristics; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */
  int matrix_coefficients; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */
  int display_horizontal_size; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */

  int display_vertical_size; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.6: sequence display extension */
};

////////////////////////////////
/** \brief video picture header

ISO/IEC 13818-2 section 6.2.3: picture_header
 */
struct PIC_Params
{
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> in case of H-263 stream it's just temporal reference, but for MPEG-4 it represents the  vop_time_increment from the last synchronization point (in clock ticks) </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int temporal_reference;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> picture type of current displayed frame </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int picture_coding_type;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Same meanings as picture_coding_type. Obsolete data field. The  picture_coding_type should be used instead.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> picture type of current displayed frame (obsolete, use picture_coding_type field instead) </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int picture_type;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vbv_delay;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A  </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int full_pel_forward_vector;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vop_fcode_forward, is used for decoding motion vectors </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int forward_f_code;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int full_pel_backward_vector;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vop_fcode_backward, is used for decoding motion vectors </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int backward_f_code;
};

////////////////////////////////
/** \brief video picture header and coding extension

In this structure the decoder returns information about a decoded picture including the coding extension information.

 */
struct PIC_ParamsEx
{
  // picture_header
/**
<table>
<tr> <td><b> AVC </b></td>		<td> POC </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> in case of H-263 stream it's just temporal reference, but for MPEG-4 it represents the  vop_time_increment from the last synchronization point (in clock ticks) </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/

  int temporal_reference;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> picture type of decoded frame </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> picture type of decoded frame </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> picture type of decoded frame </td> </tr>
</table>
*/
  int picture_coding_type;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> obsolete, do not use </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Same meanings as picture_coding_type. Obsolete data field. The  picture_coding_type should be used instead.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> obsolete, do not use </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> obsolete, do not use </td> </tr>
</table>
*/
  int picture_type;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int vbv_delay;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int full_pel_forward_vector;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vop_fcode_forward, is used for decoding motion vectors </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int forward_f_code;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int full_pel_backward_vector;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.8.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> vop_fcode_backward, is used for decoding motion vectors </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int backward_f_code;

  //  picture_coding_extension() header
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int f_code[2][2];
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int intra_dc_precision;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> \arg 0 - frame, mbaff or complete fields pair, 	\arg 1- top field, \arg 2-bottom field </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int picture_structure;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> top_field_first flag </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> top_field_first flag </td> </tr>
</table>
*/
  int top_field_first;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_pred_frame_dct;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int concealment_motion_vectors;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int q_scale_type;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int intra_vlc_format;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int alternate_scan;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> repeat_first_field flag </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> repeat_first_field flag </td> </tr>
</table>
*/
  int repeat_first_field;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int chroma_420_type;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> progressive frame </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> alternate_vertical_scan_flag: indicates the using of alternate vertical scan mode for interlaced frames.  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int progressive_frame;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td>  N/A  </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int composite_display_flag;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int v_axis;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> field or mbaff sequence </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int field_sequence;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int sub_carrier;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int burst_amplitude;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.10.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int sub_carrier_phase;

  // picture_display_extension() header
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.12.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int number_of_frame_center_offsets; /**< \brief  derived */
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.12.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_center_horizontal_offset[3];
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.12.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int frame_center_vertical_offset[3];

  // quant matrix entension
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.11.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> load_intra_quant_mat: is set to '1' if video object layer has intra_quant_mat </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int load_intra_quantizer_matrix;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.11.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> load_nonintra_quant_mat: is set to '1' if video object layer has nonintra_quant_mat </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int load_non_intra_quantizer_matrix;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.11.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int load_chroma_intra_quantizer_matrix;
/**
<table>
<tr> <td><b> AVC </b></td>		<td> N/A </td> </tr>
<tr> <td><b> MPEG-2 </b></td>	<td> Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.3.11.  </td> </tr>
<tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
<tr> <td><b> VC-1 </b></td>	<td> N/A </td> </tr>
</table>
*/
  int load_chroma_non_intra_quantizer_matrix;
  //  unsigned char quant_matrixes[4][64];
};


////////////////////////////////
/** \brief video picture header and coding extension

In this structure the decoder returns information about a decoded picture including the coding extension information.

 */
struct PIC_Type
{
    int progressive;
    /**
    <table>
    <tr> <td><b> AVC </b></td>		<td> Specifies progressive picture (0 - interlace; 1 - progressive) </td> </tr>
    <tr> <td><b> MPEG-2 </b></td>	<td> N/A </td> </tr>
    <tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
    <tr> <td><b> VC-1 </b></td>	    <td> N/A </td> </tr>
    </table>
     */

    int top_field_first;
    /**
    <table>
    <tr> <td><b> AVC </b></td>		<td> Specifies field order. (1 - Top field is first, 0 - Bottom field is first) </td> </tr>
    <tr> <td><b> MPEG-2 </b></td>	<td> N/A </td> </tr>
    <tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
    <tr> <td><b> VC-1 </b></td>	    <td> N/A </td> </tr>
    </table>
    */

     int picture_type[3];
    /**
    <table>
    <tr> <td><b> AVC </b></td>		<td> Specifies picture types. picture_type[0] for progressive picture, picture_type[1] for first field, picture_type[2] for second field (1 - I, 2- P, 3 - B)</td> </tr>
    <tr> <td><b> MPEG-2 </b></td>	<td> N/A </td> </tr>
    <tr> <td><b> MPEG-4 </b></td>	<td> N/A </td> </tr>
    <tr> <td><b> VC-1 </b></td>	    <td> N/A </td> </tr>
    </table>
    */
};

////////////////////////////////
// video frame info for parser
#if !defined (__FRAME_TT_INCLUDED__)
#define __FRAME_TT_INCLUDED__
/** \brief  video frame info for parser

The frame_tt structure is used to inform the codec about the destination colorspace and buffer.

The decoder fills passed frame_tt data structure: sets pointers to image planes, picture width, height, stride and specifies chroma format by setting the four_cc data field to FOURCC_YV12 (if sequence has 4:2:0 chroma  sampling) or to FOURCC_YV16 (if sequence has 4:2:2 chroma sampling).

*/
typedef struct
{
  uint32_t       width; /**< the number of pixels per row used for output, normally this is the horizontal size */
  uint32_t       height; /**< the number of lines used for output, normally this is the vertical size*/

/**
 for possible FourCC values look at \ref FourCC
 or set 0 for getting access to internal decoded frame buffer without color conversion. The decoder fills passed frame_tt data structure: sets pointers to image planes, picture width, height, stride and specifies chroma format by setting 				the four_cc data field to FOURCC_YV12 (if sequence has 4:2:0 chroma sampling) or to FOURCC_YV16 (if sequence has 4:2:2 chroma sampling).</td>  </tr>
</table>
*/
  uint32_t       four_cc;
  uint32_t       state; /**< state */
  unsigned char *plane[4]; /**< The decoder fills passed frame_tt data structure: sets pointers to image planes, picture width, height, stride and specifies chroma format by setting the four_cc data field to FOURCC_YV12 (if sequence has 4:2:0 chroma  sampling) or to FOURCC_YV16 (if sequence has 4:2:2 chroma sampling).*/
  int32_t        stride[4]; /**< offset for each line in the output planes. Use negative values for upside down images*/
  void         (*done)(void *this_object); /**< \brief */

/** pointer to ext_cc_struct structure.

  This structure is passed to the decoder pointer for external procedure, which will be used for color conversion of decoded pictures to planar format.
If the fourcc data member equals to FOURCC_EXTP, then the ext_cc data member must not be zero and the decoder uses external procedure for color conversion of decoded pictures.
\warning Specifying FOURCC_EXTP without specifying a correctly filled ext_cc data structure is invalid and may result in useless output or even crashes!
*/
  void          *ext_cc;
  int32_t       cc_pix_range;       /**<@brief Pixel range conversion parameter. It is useful in cases where elementary stream contains incorrect 
information about color range or does not have it all. For color conversion and deinterlacing it explicitly sets color range for input 
and output. Color conversion refers to resampling, YUV &rarr; RGB/XYZ conversion and pixel range expanding. If 
input and output ranges are the same and no conversion happens user gets decoder buffer as it is.

Following values are available for MPEG2 decoder:

<table>
<tr>
    <th>cc_pix_range value</th>
    <th>Input range</th>
    <th>Output range</th>
    <th>MPEG2 Decoder</th>
</tr>
<tr>
    <td>0</td>
    <td rowspan="2">Full range [0..255]</td>
    <td>Full range [0..255]</td>
    <td>In case of color conversion or deinterlace all sample values are converted to full range. Otherwise, no conversion happens. </td>
</tr>
<tr>
    <td>1</td>
    <td>Short range [16..235]</td>
    <td>Force short range.</td>
</tr>
<tr>
    <td>2</td>
    <td rowspan="2">Short range [16..235]</td>
    <td>Short range [16..235]</td>
    <td>In case of color conversion or deinterlace all sample values are converted to short range. Otherwise, no conversion happens. </td>
</tr>
<tr>
    <td>3</td>
    <td>Full range [0..255]</td>
    <td>Force full range.</td>
</tr>
</table>

H.264/AVC and H.265/HEVC elementary streams contain pixel range information in video_full_range_flag. Thus, decoder can either take this flag into account, <b>cc_pix_range</b> is equal to 0, 1 and 2, or completely disregard it.

<table>
<tr>
    <th>cc_pix_range value</th>
    <th>Input range</th>
    <th>Output range</th>
    <th>H.264/AVC and H.265/HEVC Decoders</th>
</tr>
<tr>
    <td>0</td>
    <td rowspan="3">Taken from stream</td>
    <td>Taken from stream</td>
    <td>Preserve color range from the stream. If stream range is full conversion happens similar to the case when <b>cc_pix_range</b> equals to 3. Otherwise, similar to the case when <b>cc_pix_range</b> equals to 5.
    </td>
</tr>
<tr>
    <td>1</td>
    <td>Short range</td>
    <td>If stream range is full conversion happens similar to the case when <b>cc_pix_range</b> equals to 4. Otherwise, similar to the case when <b>cc_pix_range</b> equals to 5.
    </td>
</tr>
<tr>
    <td>2</td>
    <td>Full range</td>
    <td>If stream range is full conversion happens similar to the case when <b>cc_pix_range</b> equals to 3. Otherwise, similar to the case when <b>cc_pix_range</b> equals to 6.
    </td>
</tr>
<tr>
    <td>3</td>
    <td rowspan="2">Full range</td>
    <td>Full range</td>
    <td>In case of color conversion or deinterlace all sample values are converted to full range. Otherwise, no conversion happens. </td>
</tr>
<tr>
    <td>4</td>
    <td>Short range</td>
    <td>Force short range.</td>
</tr>
<tr>
    <td>5</td>
    <td rowspan="2">Short range</td>
    <td>Short range</td>
    <td>In case of color conversion or deinterlace all sample values are converted to short range. Otherwise, no conversion happens. </td>
</tr>
<tr>
    <td>6</td>
    <td>Full range</td>
    <td>Force full range.</td>
</tr>
</table>

@note In scope of cc_pix_range definition "resampling" means color conversion when luma stays the same and chroma changes, for example, from 4:2:2 to 4:2:0 sampling. Resampling happens when colorspace changes from YV12 to YUY2 or YV24 to YV16.
@par
@note For H.264/AVC and H.265/HEVC Decoders short (studio) range for luma is [16*2^(bit_depth - 8) .. 235*2^(bit_depth - 8)] and [16*2^(bit_depth - 8) .. 240*2^(bit_depth - 8)] for chroma. And
 full (computer) range for for luma and chroma is [0 .. (2^bit_depth -1)].
@par
@warning From Codec SDK 11.0 Release behavior for H.264/AVC Decoder <b>cc_pix_range</b> equal to 0 was changed. To emulate old behavior use following scheme:
<table>
<tr>
    <th>Colorspace</th>
    <th>cc_pix_range value  </th>
</tr>
<tr>
    <td>@ref FOURCC_R24C @ref FOURCC_R32C @ref FOURCC_YuYv @ref FOURCC_UyVy @ref FOURCC_argb </td>
    <td>1</td>
</tr>
<tr>
    <td>@ref FOURCC_BGR3 @ref FOURCC_BGR4 @ref FOURCC_BGRA @ref FOURCC_BGRA32F @ref FOURCC_R555 @ref FOURCC_R565 @ref FOURCC_ARGB @ref FOURCC_RGBA</td>
    <td>2</td>
</tr>
<tr>
    <td>other colorspaces</td>
    <td>3</td>
</tr>
</table>
                                      @hideinitializer*/
  uint32_t       reserved3;
} frame_tt;/**< \brief */
#endif

////////////////////////////////
/** \brief user data

In this structure the decoder returns information about user data found in the video stream.
*/
typedef struct
{
    long data_size;    /**< size of user data. */
    uint8_t* p_buffer; /**< pointer to the memory buffer which contains found user data. */
} UserData_Info;

/** \brief User data buffer info structure

In this structure the decoder returns user data found in video stream.
*/
typedef struct
{
    int found_user_data; /**< number of user data found in video stream. */
    int start_code; /**< start code of syntactic structure (sequence, group of pictures or picture header) where user data was found in the video stream.  */
    UserData_Info** pp_params; /**< array of pointers to the data structures UserData_Info, which contain information about user data found in video stream. */
} UserData_Params;

/**
The frame_ext_tt structure is used to inform the decoder about the destination colorspace and buffers for decoded picture and user data associated with current
picture.

*/
typedef struct
{
    uint8_t version;     /**< version number of the structure. Current version number is equal to 1.*/
    frame_tt frame_data; /**< the frame_tt structure.*/
    UserData_Params*
        pic_user_data_params; /**< the UserData_Params structure which is used to receive user data from the decoder associated with current decoded picture. */
} frame_ext_tt;

/** \endcond */
////////////////////////////////
/** \name  video parser frame skip types */
/** @{ */ 
#define SKIP_NONE           0   /**< \brief   decode all frame types  \hideinitializer  */
#define SKIP_B              1   /**< \brief   skip B frames  \hideinitializer  */
#define SKIP_PB             2   /**< \brief   skip P and B frames  \hideinitializer  */
#define SKIP_IPB            3   /**< \brief   skip I, P and B frames  \hideinitializer  */
#define SKIP_NONREF         4   /**< \brief   skip non-reference frames  \hideinitializer  */
/** @} */

////////////////////////////////
/** \name   video reordering flag  */
/** @{ */ 
#define INTERN_REORDERING_FLAG      0x00000010L  /**< \brief   make frame reordering in the parser  \hideinitializer  */
#define IGNORE_BROKEN_LINK          0x00000020L  /**< \brief   ignore the broken link flag, only use when feeding back-to-back files to the decoder  \hideinitializer  */
#define H264_ASSUME_FRAME_RATE      0x00000040L  /**< \brief   rate in H264 bitstream is a frame rate, not a field rate  \hideinitializer  */
#define DEINTERLACING_FLAG          0x00000100L  /**< \brief   deinterlace video, bitfield bit 8..12  \hideinitializer  */
#define DEINTERLACING_FLAGS         0x00000F00L  /**< \brief   deinterlace video, bitfield bit 8..12  \hideinitializer  */
/** @} */

////////////////////////////////
/** \name   video downscaling flag     */
/** @{ */ 
#define DECODE_FULL_SIZE            0x00000000L  /**< \brief   full size decoding  \hideinitializer  */
#define DECODE_HALF_SIZE            0x00000001L  /**< \brief   horz/vert half size decoding  \hideinitializer  */
#define DECODE_QUARTER_SIZE         0x00000002L  /**< \brief   quarter size decoding  \hideinitializer  */
/** @} */

////////////////////////////////
/** \name   video OSD flag  */
/** @{ */ 
#define OSD_FORMAT_FLAGS            0x0001E000L /**< \brief   OSD format, bits 13..16  \hideinitializer  */

#define OSD_PICT_TIMECODE           0x00002000L /**< \brief   Show/hide picture time code, bit 13 (1 - show, 0 - hide )  \hideinitializer  */
#define OSD_PICT_TYPE               0x00004000L /**< \brief   Show/hide picture type, bit 14 (1 - show, 0 - hide )  \hideinitializer  */

#define OSD_SCALE_MASK              0x001C0000L /**< \brief   Scale coefficient for OSD, bits 18..20   \hideinitializer  */
#define OSD_SCALE_START_BIT         0x00000012L /**< \brief   First bit of scale coefficient  \hideinitializer  */

#define POST_PROCESSING_FLAG        0x00200000L /**< \brief  Disable/enable post processing, bit 21  \hideinitializer  */

#define LOW_LATENCY_FLAG            0x00400000L /**< \brief   decode with minimal output delay, bit 22  \hideinitializer  */
#define FLOATING_POINT_IDCT         0x00800000L /**< \brief   Use double precision floating point IDCT, bit 23  \hideinitializer  */
/** @} */

/** \name   smp mode enum  */
/** @{ */
#define SMP_NONE                    0x00000000L /**< \brief   Serial decoding.  \hideinitializer  */
#define SMP_BY_PICTURES             0x00000001L /**< \brief   Parallel decoding by pictures.  \hideinitializer  */
#define SMP_BY_SLICES               0x00000002L /**< \brief   Parallel decoding by slices.  \hideinitializer  */
/** @} */

/** \name  GET_PIC options  */
/** @{ */
#define DEINTERLACE_VERTICAL_SMOOTHING	0x00000200L /**< \brief   Vertical filtering  \hideinitializer  */
#define DEINTERLACE_FIELD_INTERPOLATION	0x00000100L /**< \brief   Top field interpolation  \hideinitializer  */
#define DEINTERLACE_FIELD_SELECT		0x00000200L /**< \brief   Bottom field interpolation  \hideinitializer  */
#define UPSAMPLE_CHROMA					0x00000400L /**< \brief   Deprecated  \hideinitializer  */
#define REORDER_FIELDS					0x00000800L /**< \brief   Changes the order of fields  \hideinitializer  */
/** @} */

/** \} */ //group end
////////////////////////////////
// audio header

/**
 * @name Audio buffer
 * @{
 **/

/**
  * @brief This structure is used to inform the parser where to put decoded audio data
  */
typedef struct
{
  unsigned char *bfr;			/**< @brief Pointer to a user buffer that will receive the PCM data */
  unsigned long bfr_size;		/**< @brief Size bytes of buffer on input or used buffer on output */
} aud_bfr_tt;
/** @} */

/**
 * @brief WMA configuration structure.
 */
typedef struct
{
  unsigned short wFormatTag;            ///<@brief specifies the unique ID of the codec used, must be 0x162 for WMA9
  unsigned short nChannels;             ///<@brief the number of audio channels
  unsigned long  nSamplesPerSec;        ///<@brief the sampling rate of the audio stream
  unsigned long  nAvgBytesPerSec;       ///<@brief the average number of bytes per second of audio stream
  unsigned short nBlockAlign;           ///<@brief the block alignment, or block size, in bytes
  unsigned short nValidBitsPerSample;   ///<@brief the number of bits per sample of monaural data
  unsigned long  nChannelMask;          ///<@brief the channel mapping
  unsigned short wEncodeOpt;            ///<@brief the options used by encoder
  unsigned short wAdvancedEncodeOpt;    ///<@brief the options used by encoder
  unsigned long  dwAdvancedEncodeOpt2;  ///<@brief the options used by encoder
  /**
  * @name Reserved
  * @{
  **/
  int reserved[256 - 4*2 - 3 /* 6*0.5 */ ];
  /** @} */
} wmad_config;

/**
 * @brief WMA PCM ouput format structure.
 */
typedef struct
{
  unsigned long outnSamplesPerSec;          ///<@brief the sampling rate of the output audio stream
  unsigned long outnChannels;               ///<@brief the number of audio channels of the output audio stream
  unsigned long outnChannelMask;            ///<@brief the channel mapping of the output audio stream
  unsigned long outnValidBitsPerSample;     ///<@brief the number of bits per sample of monaural data of the output audio stream
  unsigned long outcbPCMContainerSize;      ///<@brief the number of bytes per sample of monaural data of the output audio stream

  /**
  * @name Reserved
  * @{
  **/
  int reserved[256-5*2];
  /** @} */
} wmad_pcm_format;

/**
 * @brief WMA output information structure.
 */
typedef struct
{
  /**
  * @name Reserved
  * @{
  **/
    int reserved[256 * 4];
  /** @} */

} wmad_output_info;

/**
 * @brief WMA options structure.
 */
typedef struct
{
  unsigned int use_pcm_format;      /**<@brief specifies if the output PCM format would be customized
                                                by the user through the pcm_format struct, or PCM would have default settings*/
  wmad_pcm_format pcm_format;       ///<@brief structure that holds optional data for PCM output control

  wmad_output_info* p_output_info;  /**<@brief points to output_info structure through which the decoder provides user
                                                with additional data about the decoding process*/

  /**
  * @name Reserved
  * @{
  **/
  int reserved[256-1-1];
  /** @} */

} wmad_options;

/**
 * @brief WMA input information structure.
 */
typedef struct
{
  wmad_config  config;
  wmad_options options;

  /**
  * @name Reserved
  * @{
  **/
  int reserved[256];
  /** @} */

} wmad_input_info;
// end WMA



/**< \brief  acceleration modes enumeration */
enum hwacc_modes
{
  ACC_MODE_NONE = 0,
  ACC_MODE_DXVA1 = 1,
  ACC_MODE_DXVA2 = 2,
};
/** \brief DXVA2 configuration structure */
typedef struct
{
  void*       d3d_device_manager;     /**< \brief IDirect3DDeviceManager9  */
  uint32_t    num_allocated_surfaces; /**< \brief [out] number of allocataed surfaces  */
}dxva2_config_t;

typedef struct
{
    void*       video_acc;              //IAMVideoAccelerator
    uint32_t    num_allocated_surfaces;
}dxva1_config_t;

typedef struct dxva_surface_s dxva_surface_t;
struct dxva_surface_s
{
  uint32_t (MC_EXPORT_API *addref)(dxva_surface_t *surf);
  uint32_t (MC_EXPORT_API *release)(dxva_surface_t *surf);

  uint32_t    surface_index;
  void*       d3d_surface;        /**< \brief IDirect3DSurface9  */
};

typedef struct {
	dxva1_config_t dxva1_config;
	uint32_t num_config_picture_decode;
	void * config_picture_decode;//DXVA_ConfigPictureDecode *
	uint32_t num_config_alpha_load;
	void * config_alpha_load;//DXVA_ConfigAlphaLoad *
	uint32_t num_config_alpha_combine;
	void * config_alpha_combine;//DXVA_ConfigAlphaCombine *
} dxva1_caps_t;

enum hwacc_proc_device_type {
	ACC_MODE_DXVA2_UNKNOWN = 0,
	ACC_MODE_DXVA2_BOB,
	ACC_MODE_DXVA2_PROGRESSIVE
};

// time code info
typedef struct
{
	uint32_t DF;       // dropped frame flag
	uint32_t Hour;
	uint32_t Min;
	uint32_t Sec;
	uint32_t Frm;
	uint32_t FP;      // field phase flag in case of 50p/60p
  uint32_t base;    // the rounded timecode base, e.g. 24,25,30,50,60..
                    // note that 720p 50/59.94 fps sometimes has 25/30 base, thus Frm increments only every 2nd frame. odd frames have FP set then
	int32_t reserved[9];
}mc_time_code_t;

//---------------------------------- frame colorspace information -----------------------------------------

/** \brief  frame color space information

The frame_colorspace_info_tt structure is used to define frame color space parameters.
*/
typedef struct
{
    uint32_t width;                 ///< Frame width
    uint32_t height;                ///< Frame height
    uint32_t fourcc;                ///< FourCC format. More detail information about FOURCC is here: @ref COLORSPACE_PAGE
    uint32_t planes;                ///< Number of planes 
    int32_t  stride[4];             ///< Offset for each line in the planes. Use negative values for upside down images.
    int32_t  plane_offset[4];       ///< Offset for the planes
    uint32_t frame_size;            ///< Size of frame buffer
    uint32_t plane_height[4];       ///< Height of plane
    uint32_t plane_width[4];        ///< Width of plane
} frame_colorspace_info_tt;


/** \name User data type defines
 * @brief to set user data type in @ref enc_user_data_tt::type
 * @{
 **/
typedef enum user_data_type_e {
    USER_DATA_REGISTERED = 1,   /**< @brief User data registered by ITU-T Rec. T.35. */
    USER_DATA_UNREGISTERED = 2, /**< @brief Unregistered user data. @hideinitializer */
    USER_DATA_VAM = 3,          /**< @brief User data for adaptive quantization. */
    USER_DATA_CUSTOM  = 256     /**< @brief User data to be passed through encoding process and received via @ref AUXINFO_USER_DATA auxinfo call in the application with related encoded picture. */
}user_data_type_t;
/** @} */

#ifndef __APPLE__
#pragma pack(pop)
#endif

/**
 * @name MPEG output return codes
 * @{
 **/
#define mpegOutErrNone    0       ///< no error
#define mpegOutCancel     1       ///< user canceled settings
#define mpegOutError      2       ///< undefined error
#define mpegOutDecline    3       ///< unable to use input settings, resolution, audio etc.
#define mpegOutEOFReached 4       ///< returned by SR, when mpegin returns mpegInEOF
/** @} */

#endif // #if !defined (__MC_DEFS_INCLUDED__)
