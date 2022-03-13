/********************************************************************
 Created: 2006/11/15 
 File name: demuxpush_mp4_guid.h
 Purpose: GUIDs for MP4 Push Demuxer DS Filter

 Copyright (c) 2005-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MP4DEMUXPUSH_GUID_H__
#define __MP4DEMUXPUSH_GUID_H__


////////////////////////////////////////////////////////////

#undef  COMPANY_HIDEICON

#define MP4DEMUXERPUSH_FILTER_MERIT   MERIT_DO_NOT_USE

///////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

// {3412ECF3-0811-4600-82AB-E3C5401B12AB}
static const GUID CLSID_EMP4DemuxPush = { 0x3412ecf3, 0x811, 0x4600, { 0x82, 0xab, 0xe3, 0xc5, 0x40, 0x1b, 0x12, 0xab } };

// {BF0CEF4A-9894-4503-851B-5228FBC43EA8}
static const GUID CLSID_EMP4DemuxPushOutputPin = { 0xbf0cef4a, 0x9894, 0x4503, { 0x85, 0x1b, 0x52, 0x28, 0xfb, 0xc4, 0x3e, 0xa8 } };

// {47472B5A-D446-4065-8471-2ECE7BBCA065}
static const GUID CLSID_EMP4DemuxPush_PropertyPage = { 0x47472b5a, 0xd446, 0x4065, { 0x84, 0x71, 0x2e, 0xce, 0x7b, 0xbc, 0xa0, 0x65 } };

// {859DEEC7-DF8C-45A2-B361-8B4B27A3691B}
static const GUID CLSID_EMP4DemuxPush_AboutPropertyPage = { 0x859deec7, 0xdf8c, 0x45a2, { 0xb3, 0x61, 0x8b, 0x4b, 0x27, 0xa3, 0x69, 0x1b } };


#endif // RC_INVOKED 
///////////////////////////////////////////////////////////////

#endif __MP4DEMUXPUSH_GUID_H__
