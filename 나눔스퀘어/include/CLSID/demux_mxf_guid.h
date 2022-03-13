/********************************************************************
 Created: 2008/03/11 
 File name: demux_mxf_guid_demo.h
 Purpose: GUIDs for MXF Demuxer DS Filter Demo

 Copyright (c) 2005-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef _mxfdemux_guid_h_
#define _mxfdemux_guid_h_


////////////////////////////////////////////////////////////

#define MXFDEMUXER_FILTER_MERIT   (MERIT_PREFERRED-1)

///////////////////////////////////////////////////////////////
// {209FE7B1-5EC8-4352-846B-06E7493039CD}
static const GUID CLSID_MainConceptMXFDemuxer = 
{ 0x209fe7b1, 0x5ec8, 0x4352, { 0x84, 0x6b, 0x6, 0xe7, 0x49, 0x30, 0x39, 0xcd } };

// {AAF90954-B8F3-4a87-8B75-6617DA729B95}
static const GUID CLSID_MainConceptMXFDemuxer_PropPage = 
{ 0xaaf90954, 0xb8f3, 0x4a87, { 0x8b, 0x75, 0x66, 0x17, 0xda, 0x72, 0x9b, 0x95 } };

// {E9688B24-E3C7-4a99-8000-E0EFF5ACB6BF}
static const GUID CLSID_MainConceptMXFDemuxer_AboutPropPage = 
{ 0xe9688b24, 0xe3c7, 0x4a99, { 0x80, 0x0, 0xe0, 0xef, 0xf5, 0xac, 0xb6, 0xbf } };

///////////////////////////////////////////////////////////////

#endif //_mxfdemux_guid_h_
