/**
@file demux_mxf_mc.h
@brief Property GUIDs for MainConcept MXF demuxer parameters.

@verbatim
File: demux_mxf_mc.h

Desc: Property GUIDs for MainConcept MXF demuxer parameters.

Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
@endverbatim
*/

#ifndef __MXFDEMUX_HEADER__
#define __MXFDEMUX_HEADER__

// Media Type for non Audio/Video data carried in MXF tracks, e.g. ANC or VBI data. To connect with MainConcept MXF Demultiplexer, MXF Multiplexer and  custom filter.
// {FCDE872C-45E2-4753-B04C-49287B0CEF7A}
static const GUID MEDIATYPE_MXF_DATA = 
{ 0xfcde872c, 0x45e2, 0x4753, { 0xb0, 0x4c, 0x49, 0x28, 0x7b, 0xc, 0xef, 0x7a } };

/*
Pin Media Subtype used to connect with MainConcept MXF Demultiplexer, MXF Multiplexer or custom filter to send or receive 'ANC Frame Elements' (SMPTE ST 436 - 2006, 6.1) 
The sent/received samples shall start with 'Number of ANC packets' and end with the last byte of the 'ANC Payload Byte Array' of the last 'MXF wrapped ANC packet' in the Frame Element.
Use a custom filter to further process ANC Frame Elements.
 */
// {E57C0DC3-B3A0-45e6-AFDB-74E050982066}
static const GUID MEDIASUBTYPE_MXF_ANC_FRAME_ELEMENTS = 
{ 0xe57c0dc3, 0xb3a0, 0x45e6, { 0xaf, 0xdb, 0x74, 0xe0, 0x50, 0x98, 0x20, 0x66 } };

/*
Pin Media Subtype used to connect with MainConcept MXF Demultiplexer, MXF Multiplexer or custom filter to send or receive 'VBI Frame Elements' (SMPTE ST 436 - 2006, 5.1) 
The sent/received samples will start with the 2 bytes 'Number of lines' and end with the last byte of the'VBI Payload Byte Array' of the last 'MXF wrapped VBI line' in the Frame Element.
Use a custom filter to further process VBI Frame Elements.
*/
// {99EBC1C3-5B6A-4a41-82D4-EB640FE21AF7}
static const GUID MEDIASUBTYPE_MXF_VBI_FRAME_ELEMENTS = 
{ 0x99ebc1c3, 0x5b6a, 0x4a41, { 0x82, 0xd4, 0xeb, 0x64, 0xf, 0xe2, 0x1a, 0xf7 } };



/**
@brief Specifies if audio channels should be separated
@brief (Has effect only on multichannel PCM tracks)
@details
<dl><dt><b> Type: </b></dt><dd>VT_BOOL </dd></dl>
<dl><dt><b> Available values:</b></dt><dd> FALSE <br> TRUE </dd></dl>
<dl><dt><b> Default values: </b></dt><dd> FALSE</dd></dl>
@hideinitializer
*/
static const GUID EMXFDemux_SeparateAudioChannels =
{ 0x89238dae, 0x29f0, 0x4aff, { 0x8b, 0xa9, 0xb3, 0xe6, 0x28, 0x5d, 0xfd, 0x85 } };


/**
@brief Specifies if external streams should be parsed
@brief OP-Atom files will be linked
@details
<dl><dt><b> Type: </b></dt><dd>VT_BOOL </dd></dl>
<dl><dt><b> Available values:</b></dt><dd> FALSE <br> TRUE </dd></dl>
<dl><dt><b> Default values: </b></dt><dd> FALSE</dd></dl>
@hideinitializer
*/
static const GUID EMXFDemux_ParseExternalStreams =
{ 0x9035b248, 0x263d, 0x4a58, { 0x8a, 0x90, 0xa9, 0x78, 0x3d, 0x95, 0x5f, 0x97 } };


/**
@brief Specifies whether the demuxer should ignore any MXF track 'origin' value and always set the first access unit to be the timeline zero point
@brief OP-Atom files will be linked
@details
<dl><dt><b> Type: </b></dt><dd>VT_BOOL </dd></dl>
<dl><dt><b> Available values:</b></dt><dd> FALSE <br> TRUE </dd></dl>
<dl><dt><b> Default values: </b></dt><dd> FALSE</dd></dl>
@hideinitializer
*/
// {DA97E799-1595-4127-A496-B388E5F7E185}
static const GUID EMXFDemux_IgnoreOrigin = 
{ 0xda97e799, 0x1595, 0x4127, { 0xa4, 0x96, 0xb3, 0x88, 0xe5, 0xf7, 0xe1, 0x85 } };


#endif //__MXFDEMUX_HEADER__
