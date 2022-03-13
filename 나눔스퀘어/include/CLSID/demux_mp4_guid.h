/********************************************************************
 Created: 2006/11/15 
 File name: demux_mp4_guid_demo.h
 Purpose: Demo GUIDs for MP4 Demuxer DS Filter

 Copyright (c) 2005-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MP4DEMUX_GUID_H__
#define __MP4DEMUX_GUID_H__


////////////////////////////////////////////////////////////

#undef  COMPANY_HIDEICON

#define MP4DEMUXER_FILTER_MERIT   (MERIT_PREFERRED + 0x100)

///////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

// {9880DD26-C0AE-40D2-A1A1-8A22718297C4}
static const GUID CLSID_EMP4Demux = { 0x9880dd26, 0xc0ae, 0x40d2, { 0xa1, 0xa1, 0x8a, 0x22, 0x71, 0x82, 0x97, 0xc4 } };

// {420C079F-CC51-4D99-B3D9-FDE2C6A3F882}
static const GUID CLSID_EMP4DemuxOutputPin =
{ 0x420c079f, 0xcc51, 0x4d99, { 0xb3, 0xd9, 0xfd, 0xe2, 0xc6, 0xa3, 0xf8, 0x88 } };

// {C64EA3E7-4546-44B3-95DA-FEF8F4AA2FA0}
static const GUID CLSID_EMP4Demux_PropertyPage = { 0xc64ea3e7, 0x4546, 0x44b3, { 0x95, 0xda, 0xfe, 0xf8, 0xf4, 0xaa, 0x2f, 0xa0 } };

// {20BED094-A97B-4BD9-8454-248D0E208A84}
static const GUID CLSID_EMP4Demux_AboutPropertyPage = { 0x20bed094, 0xa97b, 0x4bd9, { 0x84, 0x54, 0x24, 0x8d, 0xe, 0x20, 0x8a, 0x84 } };

#endif // RC_INVOKED 
///////////////////////////////////////////////////////////////

#endif __MP4DEMUX_GUID_H__
