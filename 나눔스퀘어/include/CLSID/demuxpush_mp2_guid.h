/********************************************************************
 Created: 2006/11/15 
 File name: demuxpush_mp2_guid_demo.h
 Purpose: Demo GUIDs for MPEG-2 Push Demultiplexer DS Filter

 Copyright (c) 2005-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MPGPDMX_GUID_H__
#define __MPGPDMX_GUID_H__

////////////////////////////////////////////////////////////


#undef  COMPANY_HIDEICON

#define COMPANY_DS_MERIT_DEMULTIPLEXER_PUSH   (MERIT_DO_NOT_USE-1)


////////////////////////////////////////////////////////////


// {C80BF529-9F57-4a90-A27B-9D84F7CDC118} 
static const GUID CLSID_MPGPDMX = 
{ 0xc80bf529, 0x9f57, 0x4a90, { 0xa2, 0x7b, 0x9d, 0x84, 0xf7, 0xcd, 0xc1, 0x18 } };


// {F7518647-F501-441a-BEAC-F939CD77F202} 
static const GUID CLSID_MPGPDMXPropMain = 
{ 0xf7518647, 0xf501, 0x441a, { 0xbe, 0xac, 0xf9, 0x39, 0xcd, 0x77, 0xf2, 0x2 } };

// {87B99EA2-1CFD-4518-8ED5-1E1211255035} 
static const GUID CLSID_MPGPDMXPropAbout = 
{ 0x87b99ea2, 0x1cfd, 0x4518, { 0x8e, 0xd5, 0x1e, 0x12, 0x11, 0x25, 0x50, 0x35 } };


////////////////////////////////////////////////////////////

#endif //__MPGPDMX_GUID_H__

