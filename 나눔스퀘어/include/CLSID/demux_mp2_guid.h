/********************************************************************
 Created: 2006/11/15 
 File name: demux_mp2_guid_demo.h
 Purpose: Demo GUIDs for MPEG-2 Demultiplexer DS Filter

 Copyright (c) 2005-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MPGDMX_GUID_H__
#define __MPGDMX_GUID_H__

////////////////////////////////////////////////////////////


#undef  COMPANY_HIDEICON

#define COMPANY_DS_MERIT_DEMULTIPLEXER (MERIT_PREFERRED+0xFF)
#define COMPANY_DS_MERIT_PARSER        (MERIT_UNLIKELY-1)


////////////////////////////////////////////////////////////


// {79DF8A70-E7D5-49db-ADC4-A4BE0E193AA2} 
static const GUID CLSID_MPGDMX = 
{ 0x79df8a70, 0xe7d5, 0x49db, { 0xad, 0xc4, 0xa4, 0xbe, 0xe, 0x19, 0x3a, 0xa2 } };

// {58C43825-CCA6-45c0-BB27-3071CC4E4BCC} 
static const GUID CLSID_MPGDMXPropMain = 
{ 0x58c43825, 0xcca6, 0x45c0, { 0xbb, 0x27, 0x30, 0x71, 0xcc, 0x4e, 0x4b, 0xcc } };

// {817EC1FF-6F95-4b03-B82B-E81E441963E4} 
static const GUID CLSID_MPGDMXPropAbout = 
{ 0x817ec1ff, 0x6f95, 0x4b03, { 0xb8, 0x2b, 0xe8, 0x1e, 0x44, 0x19, 0x63, 0xe4 } };

// {4FA2AE25-035A-47ba-8269-0E4A3829EA3C} 
static const GUID CLSID_MPGDMX_PARSER = 
{ 0x4fa2ae25, 0x35a, 0x47ba, { 0x82, 0x69, 0xe, 0x4a, 0x38, 0x29, 0xea, 0x3c } };

// {9C07B6F6-88F0-4768-AA7B-FB6B43377AE0} 
static const GUID CLSID_MPGDMXPropAboutParser = 
{ 0x9c07b6f6, 0x88f0, 0x4768, { 0xaa, 0x7b, 0xfb, 0x6b, 0x43, 0x37, 0x7a, 0xe0 } };


////////////////////////////////////////////////////////////

#endif //__MPGDMX_GUID_H__

