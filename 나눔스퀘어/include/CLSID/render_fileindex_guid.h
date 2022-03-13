/********************************************************************
 Created: 2006/11/15 
 File name: render_fileindex_guid_demo.h
 Purpose: Demo GUIDs for DS File Index Renderer Filter

 Copyright (c) 2005-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __SINKFILTER_GUID_H__
#define __SINKFILTER_GUID_H__

////////////////////////////////////////////////////////////

#define SFCOMPANY_DS_MERIT   (MERIT_DO_NOT_USE-1)
#undef	SFCOMPANY_HIDEICON

////////////////////////////////////////////////////////////

// {EDEE9686-1FE3-420D-A7BA-ADC57BD943DB}
static const GUID CLSID_ESF = 
{ 0xedee9686, 0x1fe3, 0x420d, { 0xa7, 0xba, 0xad, 0xc5, 0x7b, 0xd9, 0x43, 0xdb } };

// {EF02D026-4AD5-481E-9388-B23687EA6B4E}
static const GUID CLSID_ESF_AboutPage = 
{ 0xef02d026, 0x4ad5, 0x481e, { 0x93, 0x88, 0xb2, 0x36, 0x87, 0xea, 0x6b, 0x4e } };

// {45AF1BB3-BE8B-4E6C-B408-C1842D7A6B9B}
static const GUID CLSID_ESF_PropertyPage = 
{ 0x45af1bb3, 0xbe8b, 0x4e6c, { 0xb4, 0x8, 0xc1, 0x84, 0x2d, 0x7a, 0x6b, 0x9b } };

////////////////////////////////////////////////////////////

#endif //__SINKFILTER_GUID_H__
