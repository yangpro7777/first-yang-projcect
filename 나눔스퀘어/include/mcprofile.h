/**
 @file	mcprofile.h
 @brief Profile and Preset definitions for default codec settings

 @verbatim
 * File: mcprofile.h
 *
 * Desc: profile/preset defines for default codec settings
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 *
 @endverbatim
 **/

#if !defined (__MC_PROFILE_INCLUDED__)
#define __MC_PROFILE_INCLUDED__

/////////////////////////////////////////////////////////////////
//
// returned strings of the default setting functions should contain the profile name
// and some codec specific data terminated by '\0' if the profile is known by the codec
// otherwise return NULL
//
// samples:
// MCPROFILE_VCD:   "VCD, PAL, MPEG-2 Video\0"      for MPEG video encoder
// MCPROFILE_DVD:   "DVD, NTSC, MPEG-2 Video\0"     for MPEG video encoder
//
// MCPROFILE_VCD:   "VCD, MPEG-1 Audio Layer 2\0"   for MPEG audio encoder
// MCPROFILE_DVD:   "DVD, MPEG-1 Audio Layer 2\0"   for MPEG audio encoder
// a.s.o
//
//
/////////////////////////////////////////////////////////////////


#define MCPROFILE_DEFAULT                       0x00000000       /**< @brief default setting of codec */


/////////////////////////////////////////////////////////////////
//
// reserved ranges
//
// 0x0001 - 0x0FFF   reserved for private codec profiles
//
/////////////////////////////////////////////////////////////////

/**
*@name MPEG-2 MainConcept Profiles
*@{
*/
/**
*@ingroup MPEG2_PRESETS_AND_PROFILES
*@{
*/
#define MCPROFILE_VCD                           0x00001000       /**< @brief MPEG-1 VideoCD @hideinitializer */
#define MCPROFILE_SVCD                          0x00001001       /**< @brief MPEG-2 SuperVCD @hideinitializer */
#define MCPROFILE_DVD                           0x00001010       /**< @brief MPEG-2 DVD-Video @hideinitializer */
#define MCPROFILE_DVD_MPEG1                     0x00001011       /**< @brief MPEG-1 DVD-Video @hideinitializer */
#define MCPROFILE_DVD_DVR                       0x00001012       /**< @brief MPEG-2 DVD-VR @hideinitializer */
#define MCPROFILE_DVD_DVR_MPEG1                 0x00001013       /**< @brief MPEG-1 DVD-VR @hideinitializer */
#define MCPROFILE_DVD_PVR                       0x00001014       /**< @brief MPEG-2 DVD+VR, not yet implemented! @hideinitializer */
#define MCPROFILE_DVD_PVR_MPEG1                 0x00001015       /**< @brief MPEG-1 DVD+VR, not yet implemented! @hideinitializer */

#define MCPROFILE_DTV                           0x00001019       /**< @brief Networked Digital Television @hideinitializer */
#define MCPROFILE_DVB                           0x00001020       /**< @brief DVB @hideinitializer */
#define MCPROFILE_MMV                           0x00001021       /**< @brief MPEG-2 MICROMV @hideinitializer */
#define MCPROFILE_DVHS                          0x00001022       /**< @brief DVHS @hideinitializer */
#define MCPROFILE_ATSC                          0x00001023       /**< @brief MPEG-2 ATSC Terrestrial @hideinitializer */
#define MCPROFILE_ATSCHI                        0x00001024       /**< @brief MPEG-2 ATSC Terrestrial high data rate @hideinitializer */
#define MCPROFILE_CABLELABS                     0x00001025       /**< @brief CableLabs MPEG-2 MP @ ML (3.18MBit) @hideinitializer */
#define MCPROFILE_ATSC_C                        0x00001026       /**< @brief MPEG-2 ATSC Cable @hideinitializer */

#define MCPROFILE_HDV_HD1                       0x00001030       /**< @brief MPEG-2 HDV HD1 @hideinitializer */
#define MCPROFILE_HDV_HD2                       0x00001031       /**< @brief MPEG-2 HDV HD2 @hideinitializer */

#define MCPROFILE_D10                           0x00001040       /**< @brief D-10 MPEG-2 4:2:2P @ ML (25 MBit) @hideinitializer */
#define MCPROFILE_D10_25                        0x00001040       /**< @brief D-10 MPEG-2 4:2:2P @ ML (25 MBit) @hideinitializer */
#define MCPROFILE_D10_30                        0x00001041       /**< @brief D-10 MPEG-2 4:2:2P @ ML (30 MBit) @hideinitializer */
#define MCPROFILE_D10_40                        0x00001042       /**< @brief D-10 MPEG-2 4:2:2P @ ML (40 MBit) @hideinitializer */
#define MCPROFILE_D10_50                        0x00001043       /**< @brief D-10 MPEG-2 4:2:2P @ ML (50 MBit) @hideinitializer */
#define MCPROFILE_DPP_SD                        0x00001045       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_DPP_HD                        0x00001046       /**< @brief Deprecated @hideinitializer */

#define MCPROFILE_HD_DVD                        0x00002000       /**< @brief HD DVD @hideinitializer */
/**@}*/
/**@}*/

/**
*@name AVC/H.264 MainConcept Profiles
*@{
*/

// Panasonic AVC-Ultra
// - MXF OpAtom family
// -- AVC-Intra Classes
#define MCPROFILE_AVC_INTRA_CLASS_50            0x00002802       /**< @brief Panasonic P2 AVC Intra (Class 50) @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_100           0x00002803       /**< @brief Panasonic P2 AVC Intra (Class 100) @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_200           0x00002810       /**< @brief Panasonic P2 AVC Intra (Class 200) @hideinitializer */

// - MXF Op1b family
// -- AVC-Intra Classes
#define MCPROFILE_P2_AVC_INTRA_HD_422           0x00002813       /**< @brief Deprecated. Use MCPROFILE_AVC_INTRA_CLASS_HD422 @hideinitializer */
#define MCPROFILE_P2_AVC_INTRA_2K_422           0x00002811       /**< @brief Deprecated. Use MCPROFILE_AVC_INTRA_CLASS_2K422 @hideinitializer */
#define MCPROFILE_P2_AVC_INTRA_4K_422           0x00002812       /**< @brief Deprecated. Use MCPROFILE_AVC_INTRA_CLASS_4K422 @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_HD422         0x00002813       /**< @brief Panasonic P2 AVC Intra Class HD 4:2:2 @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_2K422         0x00002811       /**< @brief Panasonic P2 AVC Intra Class 2K 4:2:2 @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_4K422         0x00002812       /**< @brief Panasonic P2 AVC Intra Class 4K 4:2:2 @hideinitializer */

// -- AVC-LongG Classes
#define MCPROFILE_LONG_GOP_422_CLASS_G50        0x00002900       /**< @brief Deprecated. Use MCPROFILE_AVC_LONGG_CLASS_G50 @hideinitializer */
#define MCPROFILE_LONG_GOP_422_CLASS_G25        0x00002901       /**< @brief Deprecated. Use MCPROFILE_AVC_LONGG_CLASS_G25 @hideinitializer */
#define MCPROFILE_LONG_GOP_420_CLASS_G12        0x00002902       /**< @brief Deprecated. Use MCPROFILE_AVC_LONGG_CLASS_G12 @hideinitializer */
#define MCPROFILE_LONG_GOP_420_CLASS_G6         0x00002903       /**< @brief Deprecated. Use MCPROFILE_AVC_LONGG_CLASS_G6 @hideinitializer */
#define MCPROFILE_AVC_LONGG_CLASS_G50           0x00002900       /**< @brief Panasonic AVC-LongG 4:2:2 Class G50 @hideinitializer */
#define MCPROFILE_AVC_LONGG_CLASS_G25           0x00002901       /**< @brief Panasonic AVC-LongG 4:2:2 Class G25 @hideinitializer */
#define MCPROFILE_AVC_LONGG_CLASS_G12           0x00002902       /**< @brief Panasonic AVC-LongG 4:2:0 Class G12 @hideinitializer */
#define MCPROFILE_AVC_LONGG_CLASS_G6            0x00002903       /**< @brief Panasonic AVC-LongG 4:2:0 Class G6 @hideinitializer */

// AVC-Intra RP2027
#define MCPROFILE_AVC_INTRA_CLASS_50_RP2027     0x00002804       /**< @brief RP2027 AVC Intra (Class 50) @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_100_RP2027    0x00002805       /**< @brief RP2027 AVC Intra (Class 100) @hideinitializer */
#define MCPROFILE_AVC_INTRA_CLASS_200_RP2027    0x00002806       /**< @brief RP2027 AVC Intra (Class 200) @hideinitializer */

// AVCHD
#define MCPROFILE_AVCHD                         0x00002800       /**< @brief AVCHD @hideinitializer */
#define MCPROFILE_AVCHD_20                      0x00002801       /**< @brief AVCHD 2.0 1080/50p 1080/60p @hideinitializer */

// Sony XAVC
#define MCPROFILE_XAVC_4K                       0x00002809       /**< @brief SONY XAVC Long GOP 4K Profile, for M4 and XD Style @hideinitializer */
#define MCPROFILE_XAVC_HD_M4                    0x0000280A       /**< @brief SONY XAVC Long GOP HD Profile, for MP4 Style @hideinitializer */
#define MCPROFILE_XAVC_HD_XD                    0x0000280B       /**< @brief SONY XAVC Long GOP HD Profile, for XD Style @hideinitializer */
#define MCPROFILE_XAVC_4K_422                   0x0000280F       /**< @brief SONY XAVC Long GOP 4K 4:2:2 10-bit Profile (MXF File format) @hideinitializer */

#define MCPROFILE_XAVC_HD_INTRA_CLASS_50_CBG    0x0000280C       /**< @brief SONY XAVC HD Intra CBG Class 50 Profile (RP2027, XD Style) @hideinitializer */
#define MCPROFILE_XAVC_HD_INTRA_CLASS_100_CBG   0x00002807       /**< @brief SONY XAVC HD Intra CBG Class 100 Profile (RP2027, XD Style) @hideinitializer */
#define MCPROFILE_RESERVED_HD_200               0x0000280D       /**< @brief Deprecated. @hideinitializer */
#define MCPROFILE_XAVC_HD_INTRA_VBR             0x0000280E       /**< @brief SONY XAVC HD Intra VBR Profile (M4 Style) @hideinitializer */

#define MCPROFILE_XAVC_2K_INTRA_CLASS_100_CBG   0x00002820       /**< @brief SONY XAVC 2K Intra CBG Class 100 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_4K_INTRA_CLASS_300_CBG   0x00002821       /**< @brief SONY XAVC 4K Intra CBG Class 300 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_4K_INTRA_CLASS_480_CBG   0x00002822       /**< @brief SONY XAVC 4K Intra CBG Class 480 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_QFHD_INTRA_CLASS_300_CBG   0x00002823     /**< @brief SONY XAVC QFHD Intra CBG Class 300 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_QFHD_INTRA_CLASS_480_CBG   0x00002824     /**< @brief SONY XAVC QFHD Intra CBG Class 480 Profile (XD Style) @hideinitializer */

#define MCPROFILE_XAVC_2K_INTRA_CLASS_100_VBR   0x00002830       /**< @brief SONY XAVC 2K Intra VBR Class 100 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_4K_INTRA_CLASS_300_VBR   0x00002831       /**< @brief SONY XAVC 4K Intra VBR Class 300 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_4K_INTRA_CLASS_480_VBR   0x00002832       /**< @brief SONY XAVC 4K Intra VBR Class 480 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_QFHD_INTRA_CLASS_300_VBR   0x00002833     /**< @brief SONY XAVC QFHD Intra VBR Class 300 Profile (XD Style) @hideinitializer */
#define MCPROFILE_XAVC_QFHD_INTRA_CLASS_480_VBR   0x00002834     /**< @brief SONY XAVC QFHD Intra VBR Class 480 Profile (XD Style) @hideinitializer */
/**@}*/

/**
*@ingroup MPEG2_PRESETS_AND_PROFILES
*@{
*/
#define MCPROFILE_BD                            0x00003000       /**<@brief Blu Ray Disc @hideinitializer */
#define MCPROFILE_BD_HDMV                       0x00003001       /**<@brief Blu Ray Disc (Main Video) @hideinitializer */

/**@}*/

#define MCPROFILE_PSP                           0x00004000       /**< @brief Play Station Portable by Sony @hideinitializer */
#define MCPROFILE_PSP_480x270                   0x00004001       /**< @brief Play Station Portable 480x270 by Sony @hideinitializer */
#define MCPROFILE_PSP_640x480                   0x00004002       /**< @brief Play Station Portable 640x480 by Sony @hideinitializer */

#define MCPROFILE_IPOD                          0x00005000       /**< @brief iPod by Apple @hideinitializer */
#define MCPROFILE_IPOD_640x480                  0x00005001       /**< @brief iPod 640x480 by Apple @hideinitializer */
#define MCPROFILE_IPAD                          0x00005010       /**< @brief iPad/iPad2 by Apple @hideinitializer */


#define MCPROFILE_3GPP                          0x00006000       /**< @brief 3GPP  @hideinitializer */
#define MCPROFILE_3GPP2                         0x00006100       /**< @brief 3GPP2 @hideinitializer */

#define MCPROFILE_ISMA                          0x00006200       /**< @brief ISMA @hideinitializer */

#define MCPROFILE_PIFF                          0x00006300       /**< @brief fragmented MP4 PIFF (as part of ISOFF) @hideinitializer */

#define MCPROFILE_IIS_SMOOTHSTREAMING           0x00006301       /**< @brief fragmented MP4 IIS Smooth Streaming Transport Protocol @hideinitializer */
#define MCPROFILE_ULTRAVIOLET                   0x00006302       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_A_SD              0x00006380       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_A_HD              0x00006381       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_A_SD_51C          0x00006382       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_A_HD_51C          0x00006383       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_A_HD_71C          0x00006386       /**< @brief Deprecated @hideinitializer */

#define MCPROFILE_ULTRAVIOLET_V_PD_L1           0x00006390       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_V_PD_L2           0x00006391       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_V_PD_L3           0x00006392       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_V_SD_L1           0x00006393       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_V_SD_L2           0x00006394       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_V_HD_L1           0x00006395       /**< @brief Deprecated @hideinitializer */
#define MCPROFILE_ULTRAVIOLET_V_HD_L2           0x00006396       /**< @brief Deprecated @hideinitializer */

#define MCPROFILE_JP2K_LOSSLESS                 0x00007000       /**< @brief JPEG 2000 lossless @hideinitializer */
#define MCPROFILE_JP2K_LOSSY                    0x00007001       /**< @brief JPEG 2000 lossy @hideinitializer */
#define MCPROFILE_JP2K_DCI_2K                   0x00007100       /**< @brief JPEG 2000 DCI 2K @hideinitializer */
#define MCPROFILE_JP2K_DCI_4K                   0x00007101       /**< @brief JPEG 2000 DCI 4K @hideinitializer */
#define MCPROFILE_MJ2                           0x00007102       /**< @brief JPEG 2000 file format @hideinitializer */


// XDCAM Profiles
/**
*@ingroup MPEG2_PRESETS_AND_PROFILES
*@{
*/
#define MCPROFILE_XDCAM_IMX                     0x00010000       /**< @brief Sony XDCAM IMX @hideinitializer */
#define MCPROFILE_XDCAM_IMX_25                  0x00010000       /**< @brief Sony XDCAM IMX MPEG-2 4:2:2P @ ML (25 MBit) @hideinitializer */
#define MCPROFILE_XDCAM_IMX_30                  0x00010001       /**< @brief Sony XDCAM IMX MPEG-2 4:2:2P @ ML (30 MBit) @hideinitializer */
#define MCPROFILE_XDCAM_IMX_40                  0x00010002       /**< @brief Sony XDCAM IMX MPEG-2 4:2:2P @ ML (40 MBit) @hideinitializer */
#define MCPROFILE_XDCAM_IMX_50                  0x00010003       /**< @brief Sony XDCAM IMX MPEG-2 4:2:2P @ ML (50 MBit) @hideinitializer */

#define MCPROFILE_XDCAM_DV                      0x00010050       /**< @brief Sony XDCAM DV25 @hideinitializer */

#define MCPROFILE_XDCAM_HD                      0x00010100       /**< @brief Sony XDCAM HD @hideinitializer */
#define MCPROFILE_XDCAM_1080_CBR_25             0x00010100       /**< @brief Sony XDCAM MP/H-14 (25 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_1080_VBR_17_5           0x00010101       /**< @brief Sony XDCAM MP/HL (17.5 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_1080_VBR_35             0x00010102       /**< @brief Sony XDCAM MP/HL (35 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_540_CBR_12_5            0x00010103       /**< @brief Sony XDCAM MP/H-14 (12.5 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_540_VBR_8_75            0x00010104       /**< @brief Sony XDCAM MP/HL (8.75 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_540_VBR_17_5            0x00010105       /**< @brief Sony XDCAM MP/HL (17.5 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_EX_1920                 0x00010110       /**< @brief Sony XDCAM EX MP/HL   4:2:0 1920x1080 (35 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_EX_1440                 0x00010111       /**< @brief Sony XDCAM EX MP/H-14 4:2:0 1440x1080 (25 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_EX_1280                 0x00010112       /**< @brief Sony XDCAM EX MP/HL   4:2:0 1280x720  (35 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_HD_420_1440             0x00010113       /**< @brief Sony XDCAM HD MP/H-14 4:2:0 1440x1080 (25 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_HD_420_1280             0x00010114       /**< @brief Sony XDCAM HD MP/HL   4:2:0 1280x720  (35 Mbit VBR) @hideinitializer */
#define MCPROFILE_XDCAM_HD_422_1920             0x00010115       /**< @brief Sony XDCAM HD 422P/HL 4:2:2 1920x1080 (50 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_HD_422_1280             0x00010116       /**< @brief Sony XDCAM HD 422P/HL 4:2:2 1280x720  (50 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_HD_420_1280_CBR_25      0x00010117       /**< @brief Sony XDCAM HD MP/HL   4:2:0 1280x720  (25 Mbit CBR) @hideinitializer */
#define MCPROFILE_XDCAM_HD_422_1920_540         0x00010118       /**< @brief Sony XDCAM HD 422P/HL 4:2:2 1920x540  (25 Mbit CBR) @hideinitializer */
/**@}*/

#define MCPROFILE_XDCAM_SXS_HD                  0x00010300       /**< @brief Sony XDCAM HD for SxS memory cards @hideinitializer */
#define MCPROFILE_XDCAM_SXS_IMX                 0x00010330       /**< @brief Sony XDCAM IMX for SxS memory cards @hideinitializer */
#define MCPROFILE_XDCAM_SXS_DV                  0x00010360       /**< @brief Sony XDCAM DV for SxS memory cards @hideinitializer */

#define MCPROFILE_XAVC_SXS                      0x00010390       /**< @brief Sony XAVC Single Essence Location MXF profile  (SMPTE RDD-32 7.3.1) @hideinitializer */
#define MCPROFILE_XAVC_MEL                      0x00010391       /**< @brief Sony XAVC Multiple Essence Location MXF profile  (SMPTE RDD-32 7.3.2) @hideinitializer */

#define MCPROFILE_PANASONIC_P2                  0x00011000       /**< @brief outdated, do not use  @hideinitializer */
#define MCPROFILE_PANASONIC_P2_DVCPRO           0x00011001       /**< @brief Panasonic P2 DVCPRO compatible MXF multiplex (Op-Atom, clip wrapped, audio SMPTE 382M) @hideinitializer */
#define MCPROFILE_PANASONIC_P2_AVCI             0x00011002       /**< @brief Deprecated. Use the dedicated AVC-Ultra profiles (MCPROFILE_AVC_INTRA_*) @hideinitializer */
#define MCPROFILE_PANASONIC_P2_AVC_LG           0x00011003       /**< @brief Deprecated. Use the dedicated AVC-Ultra profiles (MCPROFILE_AVC_LONGG_*) @hideinitializer */
#define MCPROFILE_PANASONIC_P2_AVC_ULTRA        0x00011003       /**< @brief Deprecated. Use the dedicated AVC-Ultra profiles (MCPROFILE_AVC_INTRA_* and MCPROFILE_AVC_LONGG_*) @hideinitializer */

#define MCPROFILE_FLASH_LOWRES                  0x00011010       /**< @brief Adobe Flash (Low Resolution Web Content) @hideinitializer */
#define MCPROFILE_FLASH_HIGHRES                 0x00011011       /**< @brief Adobe Flash (High Resolution Web Content) @hideinitializer */

#define MCPROFILE_SILVERLIGHT                   0x00012000       /**< @brief Microsoft Silverlight, Smooth Streaming @hideinitializer */

#define MCPROFILE_DASH264                       0x00013000       /**< @brief Profile for DASH-264 compliance, VOD and live profiles. @hideinitializer */
#define MCPROFILE_DASH_L1                       0x00013001       /**< @brief DASH Level  1 @hideinitializer */
#define MCPROFILE_DASH_L2                       0x00013002       /**< @brief DASH Level  2 @hideinitializer */
#define MCPROFILE_DASH_L3                       0x00013003       /**< @brief DASH Level  3 @hideinitializer */
#define MCPROFILE_DASH_L4                       0x00013004       /**< @brief DASH Level  4 @hideinitializer */
#define MCPROFILE_DASH_L5                       0x00013005       /**< @brief DASH Level  5 @hideinitializer */
#define MCPROFILE_DASH_L6                       0x00013006       /**< @brief DASH Level  6 @hideinitializer */
#define MCPROFILE_DASH_L7                       0x00013007       /**< @brief DASH Level  7 @hideinitializer */
#define MCPROFILE_DASH_L8                       0x00013008       /**< @brief DASH Level  8 @hideinitializer */
#define MCPROFILE_DASH_L9                       0x00013009       /**< @brief DASH Level  9 @hideinitializer */
#define MCPROFILE_DASH_L10                      0x00013010       /**< @brief DASH Level 10 @hideinitializer */
#define MCPROFILE_DASH_L11                      0x00013011       /**< @brief DASH Level 11 @hideinitializer */
#define MCPROFILE_DASH_L12                      0x00013012       /**< @brief DASH Level 12 @hideinitializer */
#define MCPROFILE_HLS                           0x00014000       /**< @brief Profile for HLS compliance @hideinitializer */
#define MCPROFILE_HLS_CELL_L1                   0x00014001
#define MCPROFILE_HLS_CELL_L2                   0x00014002
#define MCPROFILE_HLS_CELL_L3                   0x00014003
#define MCPROFILE_HLS_WIFI_L1                   0x00014010
#define MCPROFILE_HLS_WIFI_L2                   0x00014011
#define MCPROFILE_HLS_WIFI_L3                   0x00014012
#define MCPROFILE_HLS_WIFI_L4                   0x00014013
#define MCPROFILE_HLS_WIFI_L5                   0x00014014

#define MCPROFILE_HLS_L1                        MCPROFILE_HLS_CELL_L1  /**< @brief HLS over cellular network       : Level  1,  416x234 @hideinitializer */
#define MCPROFILE_HLS_L2                        MCPROFILE_HLS_CELL_L2  /**< @brief HLS over cellular network       : Level  2,  480x270 @hideinitializer */
#define MCPROFILE_HLS_L3                        MCPROFILE_HLS_CELL_L3  /**< @brief HLS over cellular/Wi-Fi network : Level  3,  640x360 @hideinitializer */
#define MCPROFILE_HLS_L4                        MCPROFILE_HLS_WIFI_L1  /**< @brief HLS over Wi-Fi network          : Level  4,  768x432 @hideinitializer */
#define MCPROFILE_HLS_L5                        0x00014015             /**< @brief HLS over Wi-Fi network          : Level  5,  960x540 @hideinitializer */
#define MCPROFILE_HLS_L6                        MCPROFILE_HLS_WIFI_L2  /**< @brief HLS over Wi-Fi network          : Level  6, 1280x720 @hideinitializer */
#define MCPROFILE_HLS_L7                        MCPROFILE_HLS_WIFI_L3  /**< @brief HLS over Wi-Fi network          : Level  7, 1280x720 high bitrate @hideinitializer */
#define MCPROFILE_HLS_L8                        MCPROFILE_HLS_WIFI_L4  /**< @brief HLS over Wi-Fi network          : Level  8, 1920x1080 @hideinitializer */
#define MCPROFILE_HLS_L9                        MCPROFILE_HLS_WIFI_L5  /**< @brief HLS over Wi-Fi network          : Level  9, 1920x1080 high bitrate @hideinitializer */
#define MCPROFILE_HLS_L10                       0x00014016             /**< @brief HLS over Wi-Fi network          : Level 10, 2560x1440@hideinitializer */
#define MCPROFILE_HLS_L11                       0x00014017             /**< @brief HLS over Wi-Fi network          : Level 11, 3840x2160 @hideinitializer */
#define MCPROFILE_HLS_L12                       0x00014018             /**< @brief HLS over Wi-Fi network          : Level 12, 3840x2160 high bitrate @hideinitializer */

/**
*@defgroup VC3_VIDEO_TYPES MainConcept VC-3 Encoder presets
*@{
**/
#define MCPROFILE_VC3_SQ_720p_TR                0x00015000       /**< @brief  progressive,        960x720,       8 bit,  medium quality,          compression ID: 1258 @hideinitializer */
#define MCPROFILE_VC3_SQ_720p                   0x00015001       /**< @brief  progressive,       1280x720,       8 bit,  medium quality,          compression ID: 1252 @hideinitializer */
#define MCPROFILE_VC3_HQ_720p                   0x00015002       /**< @brief  progressive,       1280x720,       8 bit,  high quality,            compression ID: 1251 @hideinitializer */
#define MCPROFILE_VC3_HQX_720p                  0x00015003       /**< @brief  progressive,       1280x720,      10 bit,  high extended quality,   compression ID: 1250 @hideinitializer */

#define MCPROFILE_VC3_LB_1080p                  0x00015010       /**< @brief  progressive,       1920x1080,      8 bit,  low quality,             compression ID: 1253 @hideinitializer */
#define MCPROFILE_VC3_SQ_1080p_TR               0x00015011       /**< @brief  progressive,       1440x1080,      8 bit,  medium quality,          compression ID: 1259 @hideinitializer */
#define MCPROFILE_VC3_SQ_1080p                  0x00015012       /**< @brief  progressive,       1920x1080,      8 bit,  medium quality,          compression ID: 1237 @hideinitializer */
#define MCPROFILE_VC3_HQ_1080p                  0x00015013       /**< @brief  progressive,       1920x1080,      8 bit,  high quality,            compression ID: 1238 @hideinitializer */
#define MCPROFILE_VC3_HQX_1080p                 0x00015014       /**< @brief  progressive,       1920x1080,     10 bit,  high extended quality,   compression ID: 1235 @hideinitializer */
#define MCPROFILE_VC3_444_1080p                 0x00015015       /**< @brief  progressive,       1920x1080,     10 bit,  RGB 4:4:4 quality,       compression ID: 1256 @hideinitializer */

#define MCPROFILE_VC3_SQ_1080i_TR               0x00015020       /**< @brief  interlaced,        1440x1080,      8 bit,  medium quality,          compression ID: 1260 @hideinitializer */
#define MCPROFILE_VC3_SQ_1080i                  0x00015021       /**< @brief  interlaced,        1920x1080,      8 bit,  medium quality,          compression ID: 1243 @hideinitializer */
#define MCPROFILE_VC3_HQ_1080i_TR               0x00015022       /**< @brief  interlaced,        1440x1080,      8 bit,  high quality,            compression ID: 1244 @hideinitializer */
#define MCPROFILE_VC3_HQ_1080i                  0x00015023       /**< @brief  interlaced,        1920x1080,      8 bit,  high quality,            compression ID: 1242 @hideinitializer */
#define MCPROFILE_VC3_HQX_1080i                 0x00015024       /**< @brief  interlaced,        1920x1080,     10 bit,  high extended  quality,  compression ID: 1241 @hideinitializer */

#define MCPROFILE_VC3_HQ_DCI_2K                 0x00015030       /**< @brief  progressive,       2048x1080,      8 bit,  high quality,            compression ID: 1272 @hideinitializer */
#define MCPROFILE_VC3_HQX_DCI_2K                0x00015031       /**< @brief  progressive,       2048x1080,     10 bit,  high extended quality,   compression ID: 1271 @hideinitializer */
#define MCPROFILE_VC3_444_DCI_2K                0x00015032       /**< @brief  progressive,       2048x1080,     10 bit,  RGB 4:4:4 quality,       compression ID: 1270 @hideinitializer */

#define MCPROFILE_VC3_HQ_DCI_4K                 0x00015040       /**< @brief  progressive,       4096x2160,      8 bit,  high quality,            compression ID: 1272 @hideinitializer */
#define MCPROFILE_VC3_HQX_DCI_4K                0x00015041       /**< @brief  progressive,       4096x2160,     10 bit,  high extended quality,   compression ID: 1271 @hideinitializer */
#define MCPROFILE_VC3_444_DCI_4K                0x00015042       /**< @brief  progressive,       4096x2160,     10 bit,  RGB 4:4:4 quality,       compression ID: 1270 @hideinitializer */

// Resolution independent profiles
#define MCPROFILE_VC3_LB                        0x00015050       /**< @brief  progressive,  any resolution,      8 bit,  low quality,             compression ID: 1274 @hideinitializer */
#define MCPROFILE_VC3_SQ                        0x00015051       /**< @brief  progressive,  any resolution,      8 bit,  medium quality,          compression ID: 1273 @hideinitializer */
#define MCPROFILE_VC3_HQ                        0x00015052       /**< @brief  progressive,  any resolution,      8 bit,  high quality,            compression ID: 1272 @hideinitializer */
#define MCPROFILE_VC3_HQX                       0x00015053       /**< @brief  progressive,  any resolution,  10/12 bit,  high extended quality,   compression ID: 1271 @hideinitializer */
#define MCPROFILE_VC3_444                       0x00015054       /**< @brief  progressive,  any resolution,  10/12 bit,  RGB 4:4:4 quality,       compression ID: 1270 @hideinitializer */
/** @}*/

/**
*@defgroup CABLELABS_VIDEO_TYPES MainConcept CableLabs global profiles
*@{
**/
#define MCPROFILE_CABLELABS_SD                  0x00016000       /**< @brief CableLabs SD Profile @hideinitializer */
#define MCPROFILE_CABLELABS_HD1                 0x00016001       /**< @brief CableLabs HD1 Profile @hideinitializer */
#define MCPROFILE_CABLELABS_HD2                 0x00016002       /**< @brief CableLabs HD2 Profile @hideinitializer */
#define MCPROFILE_CABLELABS_HD3                 0x00016003       /**< @brief CableLabs HD3 Profile @hideinitializer */
/** @}*/

#define MCPROFILE_ARIB_STD_B32_8K               0x00017000       /**< @brief DRAFT 8K HEVC Encoding profile for ARIB STD-B32. Currently compatible with Socionext S8 player, but not yet fully ARIB approved.  */

/////////////////////////////////////////////////////////////////


#endif // #if !defined (__MC_PROFILE_INCLUDED__)

