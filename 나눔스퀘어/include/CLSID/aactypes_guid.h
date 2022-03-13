/********************************************************************
 Created: 2004/11/03 
 File name: aactypes_guid.h
 Purpose: GUIDs for AAC media types

 Copyright (c) 2004-2011 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __AACTYPES_GUID_H__
#define __AACTYPES_GUID_H__


///////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

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

// {000000FF-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_AAC =
{ WAVE_FORMAT_RAW_AAC1, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

// temporary MC defined
// {00000180-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_AAC_MPEG2 =
{ WAVE_FORMAT_FRAUNHOFER_IIS_MPEG2_AAC, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

// temporary MC defined
// {0000A106-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_AAC_MPEG4 =
{ WAVE_FORMAT_MPEG4_AAC, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

// {00000278-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_AAC_SONY =
{ WAVE_FORMAT_SONY_AAC, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

// {4134504D-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_AAC_NERO =
{ 0x4134504d, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

// {00004143-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_AAC_DIVIO =
{ WAVE_FORMAT_DIVIO_MPEG4_AAC, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

// {000001FF-0000-0010-8000-00AA00389B71}
static const GUID MEDIASUBTYPE_LATMAAC =
{ WAVE_FORMAT_LATMAAC, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };


#endif // RC_INVOKED
////////////////////////////////////////////////////

#endif //__AACTYPES_GUID_H__


