/**
@file demux_mp4_mc.h
@brief Property GUIDs for MainConcept MP4 demuxer parameters.

@verbatim
File: demux_mp4_mc.h

Desc: Property GUIDs for MainConcept MP4 demuxer parameters.

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
@endverbatim
*/

#if !defined(__PROPID_EMP4DEMUX_HEADER__)
#define __PROPID_EMP4DEMUX_HEADER__

namespace MC_DS_MP4_Demuxer {

    typedef struct _tagRotationMatrix {

        double rotation_matrix[9]; /*3x3*/
    } RotationMatrix;
};

//////////////////////////////////////////////////////////////////////////
//	GUID							Type	Available range		Default Value		Description
//
//	EMP4Demux_EnableUsingFieldRate VT_INT			0,1   	       1				Use field rate instead frame rate
//	EMP4Demux_EnableAnnexBFormat   VT_INT			0,1   	       0				Annex B output format for H.264/AVC
//	EMP4Demux_EnableADTSHeaders	   VT_INT			0,1   	       0				Add ADTS headers for AAC

//////////////////////////////////////////////////////////////////////////
//	Parameters GUIDs
//////////////////////////////////////////////////////////////////////////

/**
@brief Specifies if a support edit list is enabled
@details 
<dl><dt><b> Type: </b></dt><dd>VT_INT </dd></dl>  
<dl><dt><b> Available values:</b></dt><dd> 0 <br> 1 </dd></dl> 
<dl><dt><b> Default values: </b></dt><dd> 0 </dd></dl>
@hideinitializer
*/
static const GUID MP4Demux_EDTSSupport = 
{ 0x7c22408a, 0x45d3, 0x4bfc, { 0x8c, 0x69, 0xb7, 0xb, 0x73, 0xb, 0x41, 0xf9 } };


//! @name Additional config
//! @{

/**
@brief Specifies CFF Required Metadata document
@details 
<dl><dt><b> Type: </b></dt><dd>VT_BSTR </dd></dl>  
<dl><dt><b> Available values:</b></dt><dd> Empty string <br> Metadata document </dd></dl> 
<dl><dt><b> Default values: </b></dt><dd> Empty string </dd></dl>
@hideinitializer
*/
static const GUID EMP4Demux_CFFRequiredMetadata = 
{ 0x9de5748f, 0xec7f, 0x4858, { 0x8f, 0xff, 0x43, 0x56, 0x95, 0xdb, 0x46, 0x72 } };


/**
@brief Specifies CFF Required Metadata document
@details 
<dl><dt><b> Type: </b></dt><dd>VT_LONG </dd></dl>  
<dl><dt><b> Available values:</b></dt><dd> 0-1 <br> init flag </dd></dl> 
<dl><dt><b> Default values: </b></dt><dd> 0 </dd></dl>
@hideinitializer
*/

static const GUID MP4Demuxer_InitDone = 
{ 0x7741ff89, 0xf055, 0x4115, { 0xbe, 0x13, 0xa2, 0xf8, 0x8, 0x56, 0xee, 0x3b } };

/**
@brief Represents rotation matrix 3x3 of doubles
@details
<dl><dt><b> Type: </b></dt><dd>VT_UINT_PTR </dd></dl>
<dl><dt><b> Available values:</b></dt><dd> any <br> any</dd></dl>
<dl><dt><b> Default values: </b></dt><dd> 1 1 1 1 1 1 1 1 1 </dd></dl>
@hideinitializer
*/
// {6DF4F67A-70FD-41AA-944A-81E5FEBF6E06}
static const GUID MP4Demuxer_OutputVideoPin_RotationMatrix =
{ 0x6df4f67a, 0x70fd, 0x41aa, { 0x94, 0x4a, 0x81, 0xe5, 0xfe, 0xbf, 0x6e, 0x6 } };


#endif //#if !defined(__PROPID_EMP4DEMUX_HEADER__)
