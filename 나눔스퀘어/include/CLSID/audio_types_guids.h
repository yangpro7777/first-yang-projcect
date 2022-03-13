/********************************************************************
 Created: 2015/04/08 
 File name: audio_types_guids.h
 Purpose: GUIDs for Audio media types

 Copyright (c) 2015 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __AUDIO_TYPES_GUIDS_H__
#define __AUDIO_TYPES_GUIDS_H__

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
/**   AAC Audio                                                                      **/
/**************************************************************************************/

#ifndef __AACTYPES_GUID_H__

#ifndef WAVE_FORMAT_NEC_AAC
#define WAVE_FORMAT_NEC_AAC 0x00B0
#endif

#ifndef WAVE_FORMAT_RAW_AAC1
#define WAVE_FORMAT_RAW_AAC1 0x00FF
#endif

#ifndef WAVE_FORMAT_FRAUNHOFER_IIS_MPEG2_AAC
#define WAVE_FORMAT_FRAUNHOFER_IIS_MPEG2_AAC 0x0180
#endif

#ifndef WAVE_FORMAT_LATMAAC
#define WAVE_FORMAT_LATMAAC 0x01FF
#endif

#ifndef WAVE_FORMAT_SONY_AAC
#define WAVE_FORMAT_SONY_AAC 0x0278
#endif

#ifndef WAVE_FORMAT_MPEG4_AAC
#define WAVE_FORMAT_MPEG4_AAC 0xA106
#endif

#ifndef WAVE_FORMAT_MPEG_ADTS_AAC
#define WAVE_FORMAT_MPEG_ADTS_AAC 0x1600
#endif

#ifndef WAVE_FORMAT_MPEG_RAW_AAC
#define WAVE_FORMAT_MPEG_RAW_AAC 0x1601
#endif

#ifndef WAVE_FORMAT_MPEG_LOAS
#define WAVE_FORMAT_MPEG_LOAS 0x1602
#endif

#ifndef WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC
#define WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC 0x1608
#endif

#ifndef WAVE_FORMAT_NOKIA_MPEG_RAW_AAC
#define WAVE_FORMAT_NOKIA_MPEG_RAW_AAC 0x1609
#endif

#ifndef WAVE_FORMAT_VODAFONE_MPEG_ADTS_AAC
#define WAVE_FORMAT_VODAFONE_MPEG_ADTS_AAC 0x160A
#endif

#ifndef WAVE_FORMAT_VODAFONE_MPEG_RAW_AAC
#define WAVE_FORMAT_VODAFONE_MPEG_RAW_AAC 0x160B
#endif

#ifndef WAVE_FORMAT_MPEG_HEAAC
#define WAVE_FORMAT_MPEG_HEAAC 0x1610
#endif

#ifndef WAVE_FORMAT_DIVIO_MPEG4_AAC
#define WAVE_FORMAT_DIVIO_MPEG4_AAC 0x4143
#endif

#ifndef WAVE_FORMAT_FAAD_AAC
#define WAVE_FORMAT_FAAD_AAC 0x706D
#endif

// {00000180-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AAC_MPEG2, WAVE_FORMAT_FRAUNHOFER_IIS_MPEG2_AAC);

// {0000A106-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AAC_MPEG4, WAVE_FORMAT_MPEG4_AAC);

// {000000B0-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AAC_NEC, WAVE_FORMAT_NEC_AAC);

// {00000278-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AAC_SONY, WAVE_FORMAT_SONY_AAC);

// {4134504D-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AAC_NERO, MC_FCC('MP4A'));

// {00004143-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AAC_DIVIO, WAVE_FORMAT_DIVIO_MPEG4_AAC);

// {000001FF-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_LATM_AAC, WAVE_FORMAT_LATMAAC);

#endif //__AACTYPES_GUID_H__

/**************************************************************************************/
/**   AMR Audio                                                                      **/
/**************************************************************************************/

#ifndef WAVE_FORMAT_AMR_NB
#define WAVE_FORMAT_AMR_NB 0x7361
#endif

#ifndef WAVE_FORMAT_AMR_WB
#define WAVE_FORMAT_AMR_WB 0x7362
#endif

#ifndef WAVE_FORMAT_AMR_WP
#define WAVE_FORMAT_AMR_WP 0x7363
#endif

#ifndef WAVE_FORMAT_GSM_AMR_CBR
#define WAVE_FORMAT_GSM_AMR_CBR 0x7A21
#endif

#ifndef WAVE_FORMAT_GSM_AMR_VBR_SID
#define WAVE_FORMAT_GSM_AMR_VBR_SID 0x7A22
#endif

//{62776173-0000-0010-8000-00AA00389B71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_SAWB, MC_FCC('sawb'));

// {99C00BDC-3BF1-4889-9873-F1178D3C5679}
static const GUID MEDIASUBTYPE_AMR =  
{0x99c00bdc, 0x3bf1, 0x4889, {0x98, 0x73, 0xf1, 0x17, 0x8d, 0x3c, 0x56, 0x79} };

// {CA9A0EDC-38B0-4FA6-B34A-3019543A0C57}
static const GUID MEDIASUBTYPE_AMR_MPEGABLE =
{0xca9a0edc, 0x38b0, 0x4fa6, {0xb3, 0x4a, 0x30, 0x19, 0x54, 0x3a, 0x0c, 0x57} };

// {000000FE-0000-0010-8000-00aa00389b71}
//static const GUID MEDIASUBTYPE_AMR_FFDSHOW =
//{0x000000fe, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71} };

// {726D6173-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AMR_FFDSHOW, MC_FCC('samr'));

// {4134504D-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_AMR_NERO, MC_FCC('MP4A'));

/**************************************************************************************/
/**   MPEG Audio                                                                      **/
/**************************************************************************************/ 

#ifndef WAVE_FORMAT_MPEG
#define WAVE_FORMAT_MPEG 0x0050
#endif

#ifndef WAVE_FORMAT_MPEGLAYER3
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#endif

// {00000055-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_MP3, WAVE_FORMAT_MPEGLAYER3);

/**************************************************************************************/
/**   Dolby Audio                                                                    **/
/**************************************************************************************/ 

#ifndef WAVE_FORMAT_DVM
#define WAVE_FORMAT_DVM 0x2000
#endif

// used by MPEG2 demuxers to identify Dolby E streams:

// {39043665-3589-492B-A0D6-BF490C7D2AF0}
static const GUID MEDIASUBTYPE_DOLBY_E = 
{ 0x39043665, 0x3589, 0x492b, { 0xa0, 0xd6, 0xbf, 0x49, 0xc, 0x7d, 0x2a, 0xf0 } };

/**************************************************************************************/
/**   DTS Audio                                                                    **/
/**************************************************************************************/ 

#ifndef WAVE_FORMAT_DTS
#define WAVE_FORMAT_DTS 0x0008
#endif

#ifndef WAVE_FORMAT_DTS_DS
#define WAVE_FORMAT_DTS_DS 0x0190
#endif

#ifndef WAVE_FORMAT_DTS2
#define WAVE_FORMAT_DTS2 0x2001
#endif

static const GUID MEDIASUBTYPE_DTSHD_SONIC = 
{ 0x75806E09, 0x191B, 0x48E8, { 0xA3, 0x8B, 0x09, 0xE4, 0x50, 0xF6, 0x0F, 0x1F } };

/**************************************************************************************/
/**   AES Audio                                                                      **/
/**************************************************************************************/ 

// {028E1122-4387-4870-BAD3-6B1D6578EA1F}
static const GUID MEDIASUBTYPE_AES3_302 = 
{0x28e1122, 0x4387, 0x4870, {0xba, 0xd3, 0x6b, 0x1d, 0x65, 0x78, 0xea, 0x1f} };

/**************************************************************************************/
/**   QCELP Audio                                                                    **/
/**************************************************************************************/ 

// {4D434C51-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_QCLM, MC_FCC('QCLM'));

// {706C6351-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_Qclp, MC_FCC('Qclp'));

// 4C4C454E-0000-0010-8000-00AA00389B71
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_NELL, MC_FCC('NELL'));

/**************************************************************************************/
/**   PCM Audio                                                                      **/
/**************************************************************************************/ 

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 0x0001
#endif

#ifndef WAVE_FORMAT_ADPCM
#define WAVE_FORMAT_ADPCM 0x0002
#endif

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

#ifndef WAVE_FORMAT_ALAW
#define WAVE_FORMAT_ALAW 0x0006
#endif

#ifndef WAVE_FORMAT_MULAW
#define WAVE_FORMAT_MULAW 0x0007
#endif

#ifndef WAVE_FORMAT_DVI_ADPCM
#define WAVE_FORMAT_DVI_ADPCM 0x0011
#endif

// {525F0058-522F-4312-B40F-11AC4158E02B}
DEFINE_GUID(MEDIASUBTYPE_BD_LPCM, 
0x525f0058, 0x522f, 0x4312, 0xb4, 0xf, 0x11, 0xac, 0x41, 0x58, 0xe0, 0x2b);

// {00000002-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_ADPCM, WAVE_FORMAT_ADPCM);

// {00000011-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_DVI_ADPCM, WAVE_FORMAT_DVI_ADPCM);

// {00000006-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_ALAW, WAVE_FORMAT_ALAW);

// {00000007-0000-0010-8000-00aa00389b71}
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_MULAW, WAVE_FORMAT_MULAW);

// {736f7774-0000-0010-8000-00aa00389b71} // big-endian 8,16 bit AIFF
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_twos, MC_FCC('twos'));

// {74776f73-0000-0010-8000-00aa00389b71} // little-endian 8,16 bit AIFF
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_sowt, MC_FCC('sowt'));

// {34326E69-0000-0010-8000-00aa00389b71} (big-endian int24)
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_IN24, MC_FCC('in24'));

// {32336E69-0000-0010-8000-00aa00389b71} (big-endian int32)
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_IN32, MC_FCC('in32'));

// {32336C66-0000-0010-8000-00aa00389b71} (fl32)
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_FL32, MC_FCC('fl32'));

// {34366c66-0000-0010-8000-00aa00389b71} (fl64)
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_FL64, MC_FCC('fl64'));

//guids from lavfilters : Reverse GUIDs for little-endian 'in24', 'in32', 'fl32', 'fl64'

// {696E3234-0000-0010-8000-00aa00389b71} (little-endian int24, reverse 'in24')
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_IN24_le, MC_FCC('42ni'));

// {696E3332-0000-0010-8000-00aa00389b71} (little-endian int32, reverse 'in32')
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_IN32_le, MC_FCC('23ni'));

// {666C3332-0000-0010-8000-00aa00389b71} (little-endian float32, reverse 'fl32')
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_FL32_le, MC_FCC('23lf'));

// {6c663634-0000-0010-8000-00aa00389b71} (little-endian float64, reverse 'fl64')
MC_DEFINE_AUDIO_GUID(MEDIASUBTYPE_PCM_FL64_le, MC_FCC('46lf'));

#undef MC_DEFINE_STANDARD_GUID
#undef MC_DEFINE_AUDIO_GUID
#undef MC_DEFINE_VIDEO_GUID
#undef MC_DEFINE_GUID
#undef MC_FCC
#undef MC_MAKE_FOURCC

#endif // __AUDIO_TYPES_GUIDS_H__
