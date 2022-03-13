/**
 @file  dec_hevc_guid.h
 @brief GUIDs for HEVC Decoder DS Filter

 @verbatim
 File: dec_hevc_guid.h

 Desc: GUIDs for HEVC Decoder DS Filter

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef __HEVCDEC_GUIDS_H__
#define __HEVCDEC_GUIDS_H__

////////////////////////////////////////////////////////////

#undef  HEVCCOMPANY_HIDEICON

#define HEVCCOMPANY_DS_MERIT_DECODER   (MERIT_NORMAL-1)


////////////////////////////////////////////////////////////
#ifndef RC_INVOKED // don't let resource compiler see this part

// {6A5C39F9-05F2-4860-A3F3-45BFAD353FA3}
static const GUID CLSID_EHEVCVD = 
{ 0x6a5c39f9, 0x05f2, 0x4860, { 0xa3, 0xf3, 0x45, 0xbf, 0xad, 0x35, 0x3f, 0xa3 } };

// {32D29CB1-B7E8-4352-8DFC-7E46A62FED97}
static const GUID CLSID_EHEVCVD_SettingsPropertyPage = 
{ 0x32d29cb1, 0xb7e8, 0x4352, { 0x8d, 0xfc, 0x7e, 0x46, 0xa6, 0x2f, 0xed, 0x97 } };

// {F260DDCE-6FBE-4195-9969-602AF979DF00}
static const GUID CLSID_EHEVCVD_AboutPropertyPage = 
{ 0xf260ddce, 0x6fbe, 0x4195, { 0x99, 0x69, 0x60, 0x2a, 0xf9, 0x79, 0xdf, 0x00 } };
#endif // RC_INVOKED
////////////////////////////////////////////////////////////


#endif //__HEVCDEC_GUIDS_H__
