/********************************************************************
 Created: 2004/11/03 
 File name: dec_aac_guid_demo.h
 Purpose: Demo GUIDs for AAC Decoder DS Filter

 Copyright (c) 2004-2008 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __AACDEC_GUID_H__
#define __AACDEC_GUID_H__


////////////////////////////////////////////////////////////

#undef  COMPANY_HIDEICON

#define COMPANY_DS_MERIT_AACDECODER (MERIT_PREFERRED-1)


///////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

// {B51FABD7-8260-4C8A-82AD-6896FCF9AF92}
static const GUID CLSID_AACDecoder = 
{ 0xb51fabd7, 0x8260, 0x4c8a, { 0x82, 0xad, 0x68, 0x96, 0xfc, 0xf9, 0xaf, 0x92 } };

// {B51FABD7-3BE9-429C-A23C-0AE7AAF6DD19}
static const GUID CLSID_AACDecAbout = 
{ 0xb51fabd7, 0x3be9, 0x429c, { 0xa2, 0x3c, 0xa, 0xe7, 0xaa, 0xf6, 0xdd, 0x19 } };

// {B51FABD7-3BE9-429C-A23C-0AE7AAF6DD20}
static const GUID CLSID_AACDecSettings =
{ 0xb51fabd7, 0x3be9, 0x429c, { 0xa2, 0x3c, 0xa, 0xe7, 0xaa, 0xf6, 0xdd, 0x20 } };

#endif // RC_INVOKED
////////////////////////////////////////////////////

#endif //__AACDEC_GUID_H__


