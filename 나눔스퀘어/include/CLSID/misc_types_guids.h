/********************************************************************
 Created: 2015/04/08 
 File name: misc_types_guids.h
 Purpose: GUIDs for miscellaneous media types

 Copyright (c) 2015 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MISC_TYPES_GUIDS_H__
#define __MISC_TYPES_GUIDS_H__

#include <aviriff.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <wmcodecdsp.h>

#define MC_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  static const GUID name \
  = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define MC_DEFINE_STANDARD_GUID(name, format) \
  MC_DEFINE_GUID(name,                       \
  format, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

#define MC_DEFINE_VIDEO_GUID(name, format) MC_DEFINE_STANDARD_GUID(name, format)

#define MC_DEFINE_AUDIO_GUID(name, format) MC_DEFINE_STANDARD_GUID(name, format)

#define MC_FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
  (((DWORD)(ch4) & 0xFF00) << 8) |    \
  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
  (((DWORD)(ch4) & 0xFF000000) >> 24))

#define MC_MAKE_FOURCC(ch0, ch1, ch2, ch3)  \
  ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
  ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

/**************************************************************************************/
/**   Stream media types                                                             **/
/**************************************************************************************/

static const GUID MEDIASUBTYPE_FLV =
{ 0xa0329586, 0x47ef, 0x4f4f, { 0x98, 0xb7, 0xd0, 0x6e, 0x15, 0xf2, 0x95, 0x1b } };

static const GUID MEDIATYPE_UserData = 
{ 0xa437b651, 0x9bcd, 0x480c, { 0xb3, 0xbf, 0x62, 0x7d, 0x2a, 0x1e, 0x95, 0x6e } };

// {A5F2AB55-0EFD-459f-A68B-1D347FDC2FF3}
static const GUID MEDIATYPE_PRIVATE = 
{ 0xa5f2ab55, 0xefd, 0x459f, { 0xa6, 0x8b, 0x1d, 0x34, 0x7f, 0xdc, 0x2f, 0xf3 } };

// {78C4E641-0ED1-11d3-BC6B-00A0C90D63B5}
static const GUID MEDIATYPE_MPEG2PSI =  
{0x78c4e641, 0x0ed1, 0x11d3, {0xbc, 0x6b, 0x00, 0xa0, 0xc9, 0xd, 0x63, 0xb5} };

// {93C6E675-EBA1-476C-BFAA-545C7F691CAC}
static const GUID MEDIATYPE_3GP =
{0x93C6E675, 0xEBA1, 0x476C, {0xbf, 0xaa, 0x54, 0x5c, 0x7f, 0x69, 0x1c, 0xac} };

static const GUID MEDIATYPE_LOGOS =
{0x813BACB7, 0x18A7, 0x4068, {0xAA, 0xBD, 0x8F, 0x8C, 0xD5, 0x70, 0x11, 0xBC} };

//{49952F4C-3EDC-4A9B-8906-1DE02A3D4BC2}
static const GUID MEDIASUBTYPE_HAALI =
{0x49952F4C, 0x3EDC, 0x4A9B, {0x89, 0x06, 0x1D, 0xE0, 0x2A, 0x3D, 0x4B, 0xC2} };

/**************************************************************************************/
/**   Subtitles                                                                      **/
/**************************************************************************************/


// Subtitles or closed caption similar to "media player classic" value from moreuuids.h
// {E487EB08-6B26-4be9-9DD3-993434D313FD}
static const GUID MEDIATYPE_Subtitle =
{ 0xe487eb08, 0x6b26, 0x4be9, {0x9d, 0xd3, 0x99, 0x34, 0x34, 0xd3, 0x13, 0xfd } };

// {A33D2F7D-96BC-4337-B23B-A8B9FBC295E9}
static const GUID FORMAT_SubtitleInfo =
{ 0xa33d2f7d, 0x96bc, 0x4337, { 0xb2, 0x3b, 0xa8, 0xb9, 0xfb, 0xc2, 0x95, 0xe9 } };

// {87C0B230-03A8-4FDF-8010-B27A5848200D}
static const GUID MEDIASUBTYPE_UTF8 =
{ 0x87c0b230, 0x3a8, 0x4fdf, { 0x80, 0x10, 0xb2, 0x7a, 0x58, 0x48, 0x20, 0xd  } };

// CFF-TT samples
// {6ADE9D08-168F-438C-AF3F-9845DB211098}
static const GUID MEDIASUBTYPE_CFFTT = 
{ 0x6ade9d08, 0x168f, 0x438c, { 0xaf, 0x3f, 0x98, 0x45, 0xdb, 0x21, 0x10, 0x98 } };

// {E06D802D-DB46-11CF-B4D1-0080-5F6CBBEA}
static const GUID MEDIASUBTYPE_SUBPICTURE =
{0xe06d802d, 0xdb46, 0x11cf, {0xb4, 0xd1, 0x00, 0x80, 0x5f, 0x6c, 0xbb, 0xea} };

// {53425644-0000-0010-8000-00AA00389B71}
MC_DEFINE_VIDEO_GUID(MEDIASUBTYPE_DVBS, MC_FCC('DVBS'));

// {73A9D9FB-C0C8-417c-B4D6-28071D78D14B}
static const GUID MEDIASUBTYPE_BLURAY_InteractiveGraphics = 
{ 0x73a9d9fb, 0xc0c8, 0x417c, { 0xb4, 0xd6, 0x28, 0x7, 0x1d, 0x78, 0xd1, 0x4b } };

// {B59B6945-B1D1-4ad6-B416-A84E9F852EDD}
static const GUID MEDIASUBTYPE_BLURAY_PresentationGraphics = 
{ 0xb59b6945, 0xb1d1, 0x4ad6, { 0xb4, 0x16, 0xa8, 0x4e, 0x9f, 0x85, 0x2e, 0xdd } };

// {1204C54D-7FFC-4a48-AD14-DAF87FAC5252}
static const GUID MEDIASUBTYPE_BLURAY_TextST = 
{ 0x1204c54d, 0x7ffc, 0x4a48, { 0xad, 0x14, 0xda, 0xf8, 0x7f, 0xac, 0x52, 0x52 } };

/**************************************************************************************/
/**   MXF input types                                                                **/
/**************************************************************************************/

// {0ae93686-aad5-4a8d-9123-ed575387e1f4} 
static const GUID MEDIASUBTYPE_MXF_OPENCUBE =
{0x1a537913, 0xd36b, 0x4b22, {0xbe, 0x06, 0x50, 0xeb, 0xa7, 0x92, 0x4a, 0x21} };

// {6c90b26c-0b24-455b-bc8f-7c11147755fb} 
static const GUID MEDIASUBTYPE_MXF_XDCAM_VIEWER_OLD = 
{0x6c90b26c, 0x0b24, 0x455b, {0xbc, 0x8f, 0x7c, 0x11, 0x14, 0x77, 0x55, 0xfb} };

// {07853f2b-c3a6-4bef-a11c-c2c524f39446} 
static const GUID MEDIASUBTYPE_MXF_XDCAM_VIEWER_NEW = 
{0x07853f2b, 0xc3a6, 0x4bef, {0xa1, 0x1c, 0xc2, 0xc5, 0x24, 0xf3, 0x94, 0x46} };

// {65A37954-0A32-445f-BE40-DC2D79945773} 
static const GUID MEDIASUBTYPE_MXF_PANASONIC_P2 =
{0x65a37954, 0x0a32, 0x445f, {0xbe, 0x40, 0xdc, 0x2d, 0x79, 0x94, 0x57, 0x73} };


/**************************************************************************************/
/**   MXF non A/V data types                                                                 **/
/**************************************************************************************/

#ifndef __MXFDEMUX_HEADER__

// Media Type for non Audio/Video data carried in MXF tracks, e.g. ANC or VBI data. To connect with MainConcept MXF Demultiplexer, MXF Multiplexer and  custom filter.
// {FCDE872C-45E2-4753-B04C-49287B0CEF7A}
static const GUID MEDIATYPE_MXF_DATA = 
{ 0xfcde872c, 0x45e2, 0x4753, { 0xb0, 0x4c, 0x49, 0x28, 0x7b, 0xc, 0xef, 0x7a } };

/*
Pin Media Subtype used to connect with MainConcept MXF Demultiplexer, MXF Multiplexer or custom filter to send or receive 'ANC Frame Elements' (SMPTE ST 436 - 2006, 6.1) 
The sent/received samples shall start with 'Number of ANC packets' and end with the last byte of the 'ANC Payload Byte Array' of the last 'MXF wrapped ANC packet' in the Frame Element.
Use a custom filter to further process ANC Frame Elements.
 */
// {E57C0DC3-B3A0-45e6-AFDB-74E050982066}
static const GUID MEDIASUBTYPE_MXF_ANC_FRAME_ELEMENTS = 
{ 0xe57c0dc3, 0xb3a0, 0x45e6, { 0xaf, 0xdb, 0x74, 0xe0, 0x50, 0x98, 0x20, 0x66 } };

/*
Pin Media Subtype used to connect with MainConcept MXF Demultiplexer, MXF Multiplexer or custom filter to send or receive 'VBI Frame Elements' (SMPTE ST 436 - 2006, 5.1) 
The sent/received samples will start with the 2 bytes 'Number of lines' and end with the last byte of the'VBI Payload Byte Array' of the last 'MXF wrapped VBI line' in the Frame Element.
Use a custom filter to further process VBI Frame Elements.
*/
// {99EBC1C3-5B6A-4a41-82D4-EB640FE21AF7}
static const GUID MEDIASUBTYPE_MXF_VBI_FRAME_ELEMENTS = 
{ 0x99ebc1c3, 0x5b6a, 0x4a41, { 0x82, 0xd4, 0xeb, 0x64, 0xf, 0xe2, 0x1a, 0xf7 } };

#endif // __MXFDEMUX_HEADER__

#undef MC_DEFINE_STANDARD_GUID
#undef MC_DEFINE_AUDIO_GUID
#undef MC_DEFINE_VIDEO_GUID
#undef MC_DEFINE_GUID
#undef MC_FCC
#undef MC_MAKE_FOURCC

#endif __MISC_TYPES_GUIDS_H__
