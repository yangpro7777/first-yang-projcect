/** 
\file   common_mc.h
\brief  IModuleConfig common parameter's identifiers

 @verbatim
 File: common_mc.h
 Desc: IModuleConfig common parameter's identifiers
 
 Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.
 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 */


#ifndef _COMMON_MC_
#define _COMMON_MC_

/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//    GUID                        Value Type        Available range        Default Value    Note
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Unified for all components
//
// MC_OPERATION_MODE            VT_INT            [OperationMode_t]    READONLY        Operation mode 
// MC_OEM_NAME                  VT_BSTR           Readonly             undefined       OEM customer Name
// MC_OUT_SAMPLE_SIZE           VT_INT            type limits          0               Output sample size: 0 - size should be calculated automatically
// MC_PRESET                    VT_INT            type limits          undefined       A number of grouped settings
// MC_AFFINITY_MASK             VT_UI4            type limits          undefined       A bit vector in which each bit represents the processors that a process or thread (depending on component implementation) is allowed to run on
// MC_AUTOPERSIST               VT_UI4            [0, 1]               1               Enables/disables automatic updating of registry settings
//
// Encoder's parameters  (video, audio)
//
// MC_BITRATE_AVG                VT_INT\VT_I8     type limits          undefined       Average Bit rate. The value type is codec specific. For more information see codec DS filter page.
// MC_BITRATE_MAX                VT_INT\VT_I8     type limits          undefined       Maximum Bit rate. The value type is codec specific. For more information see codec DS filter page.
// MC_BITRATE_MIN                VT_INT           type limits          undefined       Minimum Bit rate
// MC_BITRATE_MODE               VT_INT           type limits          undefined       Bit Rate Control Mode
// MC_BITRATE_PASS               VT_INT           type limits          undefined       Multipass encoding mode
// MC_BITRATE_STAT               VT_BSTR          type limits          undefined       File path for storage rate control statistics
// MC_PROFILE                    VT_INT           type limits          undefined       Profile
// MC_LEVEL                      VT_INT           type limits          undefined       Level
// MC_GOP_LENGTH                 VT_INT           0 ... type limits    undefined       Distance between key frames (I-pictures)
// MC_GOP_BCOUNT                 VT_INT           0 ... type limits    undefined       Number of B-pictures between reference frames
// MC_ASPECT_RATIO               VT_INT           type limits          undefined       Picture aspect ratio code
//
// Audio Encoder's parameters 
//
// MC_JOINT_CODING               VT_INT           type limits          undefined       Joint coding mode for stereo audio streams (mid-side, intensity or dual channel)
// MC_BANDWIDTH                  VT_INT           type limits          undefined       Bandwidth of the low-pass filter applied before encoding
// MC_CRC_PROTECTION             VT_INT           type limits          undefined       Enable CRC protection or not
//
//    Statistics (read-only)
//
// MC_ENCODED_FRAMES             VT_INT           type limits          undefined       Encoded and delivered frames (read-only)
// MC_FPS                        VT_R8            type limits          undefined       Frames per second. Should return actual speed per second (system time) (read-only)
// MC_CALC_PSNR                  VT_INT           [0, 1]               [0]             Calculate overall PSNR or not
// MC_OVERALL_PSNR               VT_R8            [0, limit of DOUBLE] READONLY        Overall PSNR (luma and chroma)
// MC_BITRATE_REAL               VT_R8            [0, limit of DOUBLE] READONLY        Real average bitrate
//
// Video Decoder's parameters
//
// MC_HardwareAcceleration       VT_UI4           [0, 1]               [0]             Switching between software and hardware decode modes
// MC_HardwareAccelerationState  VT_UI4           [0, 2]               [0]             Get hardware acceleration state (whether it is used or not currently)
// MC_PostProcess                VT_UI4           [0, 1]               [0]             Enable/disable deblocking and dering filtering (only software mode)
// MC_Brightness                 VT_UI4           [0, 255]             [128]           Brightness level
// MC_Contrast                   VT_UI4           [0, 255]             [128]           Contrast
// MC_Deinterlace                VT_UI4           [0, 3]               [0]             Selecting output deinterlacing mode
// MC_Quality                    VT_UI4           [0, 3]               [0]             Selecting skip mode(I,IP,IBP) or obey quality messages
// MC_LoopFilter                 VT_UI4           [0, 2]               [0]             Set loop filter mode for DBF or SAO: 0=off, 1=standard, 2=reference frame only
// MC_OSD                        VT_UI4           [0, 1]               [0]             Enable/disable on screen display info (only software mode)
// MC_CapturePicture             VT_VOID|VT_BYREF []                   [0]             Get last decoded picture. Call GetValue(...) and send empty VARIANT argument. Reinterpreted pbVal to EM2VD_CapturePictureInfo struct pointer. Free allocated memory throw CoTaskMemFree on the end.
// MC_CaptureUserData			 VT_VOID|VT_BYREF []				   [0]			   Get user data                                                                                     
//
// MC_FramesDecoded              VT_UI4           []                   [0]             Count of decoded frames (read only)
// MC_FramesSkipped              VT_UI4           []                   [0]             Count of skipped frames (read only)
// MC_MultiThread                VT_UI4           [0, 1]               [1]             Use multi-thread if available (available on multi-processors systems, only software decode mode, only MPEG-2 streams)
// MC_NumThreads				 VT_UI4           [,codec limit]       [0]             Number of threads to be used by codec
// MC_MediaTimeStart             VT_I8            []                                   Media time of current frame (it may be a byte offset of current frame)
// MC_MediaTimeStop              VT_I8            []    
// MC_ResetStatistics            VT_UI4           [0, 1]               [0]             Reset decoder statistics (decoded and skipped frames counter).
// MC_ErrorConcealment           VT_UI4           [0, 1]               [1]             Don't show frames with errors
// MC_VMRMaintainAspectRatio     VT_UI4           [0, 1]               [1]             Enable "maintain aspect ratio" for VMR-7,9 while connection is complete
// MC_StreamOrder                VT_UI4           [0, 1]               [0]             Output frames in streams order
// MC_OutputQueue                VT_UI4           type limits          [0]             Output queue size. 0 - disable output queue.
//
// VES analysis
//
// MC_VESA_SETTINGS              VT_VOID|VT_BYREF []                   [0]             Set/get VESA settings (VESA_settings_t, mcvesa.h)
// MC_VESA_BUFFERS               VT_VOID|VT_BYREF []                   [1]             Get VESA buffers (VESA_buffers_t, mcvesa.h)

// MC_OutputYV12				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_YV12 on output pin
// MC_OutputYUY2				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_YUY2 on output pin
// MC_OutputUYVY				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_UYVY on output pin
// MC_OutputAYUV				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_AYUV on output pin
// MC_OutputRGB32				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB32 on output pin
// MC_OutputRGB24				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB24 on output pin
// MC_OutputRGB565				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB24(RGB565) on output pin
// MC_OutputRGB555				VT_UI4				[0, 1]				[1]					Enable/disable MEDIASUBTYPE_RGB24(RGB555) on output pin

namespace MC_COMMON
{
    //! Available values for MC_OPERATION_MODE parameter
    typedef enum OperationMode
    {
        NOTACTIVE_MODE          = 0,    // 
        DEMO_MODE               = 1,    // 
        EVALUATION_MODE         = 2,    // 
        EVALUATION_EXPIRED_MODE = 3,    // 
        FULL_MODE               = 4     // 
    } OperationMode_t;

	//! MC stream type
	typedef enum _tStreamType 
	{
		MC_STREAM_TYPE_UNKNOWN    =  0,
		MC_STREAM_TYPE_SYSTEM     =  1, // MPEG1 System 
		MC_STREAM_TYPE_PROGRAM    =  2, // MPEG2 Programm
		MC_STREAM_TYPE_TRANSPORT  =  3, // MPEG2 Transport
		MC_STREAM_TYPE_MPEG2V     =  4, // MPEG2 Video
		MC_STREAM_TYPE_PVA        =  5, // 
		MC_STREAM_TYPE_MPEG2A     =  6, // MPEG2 Audio
		MC_STREAM_TYPE_AC3        =  7, // AC3 Audio
		MC_STREAM_TYPE_H264       =  8, // H264/AVC Video
		MC_STREAM_TYPE_MPEG4      =  9, // MPEG4
		MC_STREAM_TYPE_VC1        = 10, // VC 1
		MC_STREAM_TYPE_H263       = 11, // H263
		MC_STREAM_TYPE_MPEG4V     = 12, // MPEG4 Video
		MC_STREAM_TYPE_AAC        = 13, // AAC Audio
		MC_STREAM_TYPE_MPEG1V     = 14, // MPEG1 Video
		MC_STREAM_TYPE_MPEG1A     = 15, // MPEG1 Audio
		MC_STREAM_TYPE_DIV3       = 16, // DivX 3
		MC_STREAM_TYPE_DIV4       = 17, // DivX 4
		MC_STREAM_TYPE_DIV5       = 18, // DivX 5
		MC_STREAM_TYPE_VSSH       = 19, // VideoSoft H264 (under development - no parcer)
		MC_STREAM_TYPE_PCM        = 20, // LPCM (under development)
		MC_STREAM_TYPE_DVDSUB     = 21, // DVD Sub-Picture
		MC_STREAM_TYPE_AVS        = 22, //AVS video
        MC_STREAM_TYPE_HEVC       = 23  //HEVC video
	} MC_STREAM_TYPE;
}

//
//    Unified for all components
//

//!   Operation mode (read-only).
//!   Should return component's operation mode (returned by License Control or tuned by application).
// {AFE11383-84D7-4F83-B114-ADC382F97D71}
static const GUID MC_OPERATION_MODE =
{ 0xAFE11383, 0x84D7, 0x4F83, { 0xB1, 0x14, 0xAD, 0xC3, 0x82, 0xF9, 0x7D, 0x71 } };

//!   OEM Name (read-only).
// {F9041053-1903-4497-82C4-519F0DA71196}
static const GUID MC_OEM_NAME =
{ 0xF9041053, 0x1903, 0x4497, { 0x82, 0xC4, 0x51, 0x9F, 0xD, 0xA7, 0x11, 0x96 } };

/**
<dl><dt><b>GUID:</b></dt><dd>
{32DAF502-17D2-4CFD-ABD9-CF5C0F447D0B}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the output sample size.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
static const GUID MC_OUT_SAMPLE_SIZE =
{ 0x32DAF502, 0x17D2, 0x4CFD, { 0xAB, 0xD9, 0xCF, 0x5C, 0xF, 0x44, 0x7D, 0xB } };

/** 
<dl><dt><b>GUID:</b></dt><dd>
{0D6239BD-E7DC-4DDB-AB8D-C21E8AFDA71C}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Preset. A number of grouped settings. 
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
//    Preset. A number of grouped settings. 
// {0D6239BD-E7DC-4DDB-AB8D-C21E8AFDA71C}
static const GUID MC_PRESET =
{ 0xD6239BD, 0xE7DC, 0x4DDB, { 0xAB, 0x8D, 0xC2, 0x1E, 0x8A, 0xFD, 0xA7, 0x1C } };


/** 
<dl><dt><b>GUID:</b></dt><dd>
{570AC266-075F-4D1B-92BE-F2010A91BBF9}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Path to license file, which is used to enable special features of some components. MC_LICENSE_FILE_PATH has lower priority. If MC_LICENSE_BUFFER was specified, current method will be ignored.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_BSTR
</dd></dl>
*/
// {570AC266-075F-4D1B-92BE-F2010A91BBF9}
static const GUID MC_LICENSE_FILE_PATH = 
{ 0x570ac266, 0x75f, 0x4d1b, { 0x92, 0xbe, 0xf2, 0x1, 0xa, 0x91, 0xbb, 0xf9 } };

/**
<dl><dt><b>GUID:</b></dt><dd>
{CB778914-FFD5-4C26-9AD7-8A6F425E3082}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Buffer containing license data, such as the contents of a license file. This is used to enable special features of some components. MC_LICENSE_BUFFER will have more priority and MC_LICENSE_FILE_PATH will be ignored.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_BSTR
</dd></dl>
*/
// {CB778914-FFD5-4C26-9AD7-8A6F425E3082}
static const GUID MC_LICENSE_BUFFER =
{ 0xcb778914, 0xffd5, 0x4c26, { 0x9a, 0xd7, 0x8a, 0x6f, 0x42, 0x5e, 0x30, 0x82 } };

/**
Sets a processor affinity mask for the threads of the specified process. Affinity mask. A bit vector in which each bit represents the processors that  a process or thread (depending on component implementation) is allowed to run onFor more information see description of the SetProcessAffinityMask function in the MSDN documentation.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - MAX_UI4
</dd></dl> \hideinitializer */ // {D030CB55-C27C-4EAA-93CC-1DD13FE2A515}
static const GUID MC_AFFINITY_MASK =
{ 0xD030CB55, 0xC27C, 0x4EAA, { 0x93, 0xCC, 0x1D, 0xD1, 0x3F, 0xE2, 0xA5, 0x1 } };

//!Auto persist management. Enables/disables automatic updating of registry settings
// {5B6B1EA9-7867-4F67-AD77-46F51C214E1D}
static const GUID MC_AUTOPERSIST =
{ 0x5B6B1EA9, 0x7867, 0x4F67, { 0xAD, 0x77, 0x46, 0xF5, 0x1C, 0x21, 0x4E, 0x1D } };


/**
Pointer to stream user data callback interface
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_BYREF

Usage:
1. Create class derived IStreamUserDataCallback should be created. 
\code
  class UD_callback: public IStreamUserDataCallback 
\endcode
2. Overwrite void UserDataCallback(void * buf, int size) method which will be called by filter.
\note Buffer will be valid in scope of this method only. It can be overwritten after this function.
3. Sent pointer to this class into filter via ModuleConfig interface
\code
UD_callback UDC;
CComVariant var;
var.vt = VT_BYREF;
var.byref = &UDC;
MC->SetValue(&MC_STREAM_USER_DATA_CALLBACK_INTERFACE,&var); 
MC->CommitChanges(NULL); 
\endcode
\
Filter will call customer`s UserDataCallback() as soon as user data will be parsed.
\note Control should be returned to decoder as soon as possible to avoid hanging of decoder threads.
</dd></dl> \hideinitializer */
// {25B1563C-EE23-478B-9035-D959DEA16749}
static const GUID MC_STREAM_USER_DATA_CALLBACK_INTERFACE = 
{ 0x25b1563c, 0xee23, 0x478b, { 0x90, 0x35, 0xd9, 0x59, 0xde, 0xa1, 0x67, 0x49 } };

//
//    Uniform Encoder's parameters 
//
// {BDD50372-38E2-4A26-BA15-482B70EE136C}
/**
<dl><dt><b>GUID:</b></dt><dd>
{BDD50372-38E2-4a26-BA15-482B70EE136C}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the average bit rate in bits/sec.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
The value type is codec specific. For more information see codec DS filter page.
</dd></dl>
*/
static const GUID MC_BITRATE_AVG =
{ 0xBDD50372, 0x38E2, 0x4A26, { 0xBA, 0x15, 0x48, 0x2B, 0x70, 0xEE, 0x13, 0x6C } };

/** 
<dl><dt><b>GUID:</b></dt><dd>
{51EB9A74-B317-4fbb-832B-CA5C2DAA193F}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the hypothetical stream scheduler rate in bits/sec.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
The value type is codec specific. For more information see codec DS filter page.
</dd></dl>
*/
//    Maximum Bit rate 
// {51EB9A74-B317-4FBB-832B-CA5C2DAA193F}
static const GUID MC_BITRATE_MAX =
{ 0x51EB9A74, 0xB317, 0x4FBB, { 0x83, 0x2B, 0xCA, 0x5C, 0x2D, 0xAA, 0x19, 0x3F } };

//    Minimum Bit rate
// {38A26DF5-D04E-4931-BBC2-19097F812069}
static const GUID MC_BITRATE_MIN =
{ 0x38A26DF5, 0xD04E, 0x4931, { 0xBB, 0xC2, 0x19, 0x9, 0x7F, 0x81, 0x20, 0x69 } };

/**
<dl><dt><b>GUID:</b></dt><dd>
{CD701824-5F65-4520-8DB0-26548212808C}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the bit rate control mode.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
//    Bit Rate Control Mode
// {CD701824-5F65-4520-8DB0-26548212808C}
static const GUID MC_BITRATE_MODE =
{ 0xCD701824, 0x5F65, 0x4520, { 0x8D, 0xB0, 0x26, 0x54, 0x82, 0x12, 0x80, 0x8C } };

/**
<dl><dt><b>GUID:</b></dt><dd>
{9078A811-26BC-4BA0-8AE7-6FC1CCF7D6D1}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the multi-pass encoding modes.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
// Multipass encoding mode
// {9078A811-26BC-4BA0-8AE7-6FC1CCF7D6D1}
static const GUID MC_BITRATE_PASS =
{ 0x9078A811, 0x26BC, 0x4BA0, { 0x8A, 0xE7, 0x6F, 0xC1, 0xCC, 0xF7, 0xD6, 0xD1 } };

// Multipass statistics file name
// {05E339D3-C15D-4928-A5B7-E5D165ACD7F1}
static const GUID MC_BITRATE_STAT =
{ 0x5E339D3, 0xC15D, 0x4928, { 0xA5, 0xB7, 0xE5, 0xD1, 0x65, 0xAC, 0xD7, 0xF1 } };

/**
<dl><dt><b>GUID:</b></dt><dd>{FDD66DC1-DB28-4df9-ADE8-1FAD172FF045} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>This parameter specifies a subset of the H.264/AVC syntax required for decoding the stream.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// Profile
// {FDD66DC1-DB28-4DF9-ADE8-1FAD172FF045}
static const GUID MC_PROFILE =
{ 0xFDD66DC1, 0xDB28, 0x4DF9, { 0xAD, 0xE8, 0x1F, 0xAD, 0x17, 0x2F, 0xF0, 0x45 } };

/**
<dl><dt><b>GUID:</b></dt><dd>{68AB7A6D-A427-467b-B39D-6651E50C22C4}</dd></dl>
<dl><dt><b>Description:</b></dt><dd>This parameter specifies constraints for the encoding parameters.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
//  Level
// {68AB7A6D-A427-467B-B39D-6651E50C22C4}
static const GUID MC_LEVEL =
{ 0x68AB7A6D, 0xA427, 0x467B, { 0xB3, 0x9D, 0x66, 0x51, 0xE5, 0xC, 0x22, 0xC4 } };


/**
<dl><dt><b>GUID:</b></dt><dd>
{9CD0B169-F1AD-4EA2-B826-EF9E7413C367}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies chroma format.
</dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Monochrome <br>
1 - Chroma 4:2:0 <br>
2 - Chroma 4:2:2 <br>
3 - Chroma 4:4:4 <br>
</dd></dl>
*/
// {9CD0B169-F1AD-4EA2-B826-EF9E7413C367}
static const GUID MC_CHROMA_FORMAT = 
{ 0x9cd0b169, 0xf1ad, 0x4ea2, { 0xb8, 0x26, 0xef, 0x9e, 0x74, 0x13, 0xc3, 0x67 } };


/**
<dl><dt><b>GUID:</b></dt><dd>
{72EE0B12-AF81-41c4-AAE9-BFB23725F554}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the maximum GOP length.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
//
//    Video GOP structure
//
//    Distance between key frames (I-pictures).
// {72EE0B12-AF81-41C4-AAE9-BFB23725F554}
static const GUID MC_GOP_LENGTH =
{ 0x72EE0B12, 0xAF81, 0x41C4, { 0xAA, 0xE9, 0xBF, 0xB2, 0x37, 0x25, 0xF5, 0x54 } };



/** 
<dl><dt><b>GUID:</b></dt><dd>
{3ADA1696-B94D-4d7e-B7BF-253D1103E835}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
This parameter specifies the maximum consecutive number of B-frames.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
//    Number of B-pictures between reference frames.
// {3ADA1696-B94D-4D7E-B7BF-253D1103E835}
static const GUID MC_GOP_BCOUNT =
{ 0x3ADA1696, 0xB94D, 0x4D7E, { 0xB7, 0xBF, 0x25, 0x3D, 0x11, 0x3, 0xE8, 0x35 } };

/**
Picture aspect ratio code.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_INT 
</dd></dl> \hideinitializer */ 
// {57B6D4ED-52D6-4099-9528-C10995465C00}
static const GUID MC_ASPECT_RATIO =
{ 0x57B6D4ED, 0x52D6, 0x4099, { 0x95, 0x28, 0xC1, 0x9, 0x95, 0x46, 0x5C, 0x0 } };

/**
<dl><dt><b>GUID:</b></dt><dd>{5D097DF4-13DF-4381-96D1-C4E7EF8FD996} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>Input picture color primaries.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// {5D097DF4-13DF-4381-96D1-C4E7EF8FD996}
static const GUID MC_IN_COLOUR_PRIMARIES =
{ 0x5d097df4, 0x13df, 0x4381,{ 0x96, 0xd1, 0xc4, 0xe7, 0xef, 0x8f, 0xd9, 0x96 } };

/**
<dl><dt><b>GUID:</b></dt><dd>{D0C424D0-40B8-421E-9E3D-A6765B6E63D0} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>Input picture transfer characteristics.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// {D0C424D0-40B8-421E-9E3D-A6765B6E63D0}
static const GUID MC_IN_TRANSFER_CHARS =
{ 0xd0c424d0, 0x40b8, 0x421e,{ 0x9e, 0x3d, 0xa6, 0x76, 0x5b, 0x6e, 0x63, 0xd0 } };

/**
<dl><dt><b>GUID:</b></dt><dd>{45DA4378-46A5-491B-86A4-A179D6624529} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>Input picture matrix coefficients.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// {45DA4378-46A5-491B-86A4-A179D6624529}
static const GUID MC_IN_MATR_COEFFS =
{ 0x45da4378, 0x46a5, 0x491b,{ 0x86, 0xa4, 0xa1, 0x79, 0xd6, 0x62, 0x45, 0x29 } };

/**
<dl><dt><b>GUID:</b></dt><dd>{F5966B6A-BEC3-4B82-B114-4FD7BB63412D} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>Output picture color primaries.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// {F5966B6A-BEC3-4B82-B114-4FD7BB63412D}
static const GUID MC_OUT_COLOUR_PRIMARIES =
{ 0xf5966b6a, 0xbec3, 0x4b82,{ 0xb1, 0x14, 0x4f, 0xd7, 0xbb, 0x63, 0x41, 0x2d } };

/**
<dl><dt><b>GUID:</b></dt><dd>{CAD1A4E4-AF92-4315-B1AD-E1E67619F16D} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>Output picture transfer characteristics.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// {CAD1A4E4-AF92-4315-B1AD-E1E67619F16D}
static const GUID MC_OUT_TRANSFER_CHARS =
{ 0xcad1a4e4, 0xaf92, 0x4315,{ 0xb1, 0xad, 0xe1, 0xe6, 0x76, 0x19, 0xf1, 0x6d } };

/**
<dl><dt><b>GUID:</b></dt><dd>{84F437B4-55DD-4B7C-BE0A-43EE1E34C726} </dd></dl>
<dl><dt><b>Description:</b></dt><dd>Output picture matrix coefficients.</dd></dl>
<dl><dt><b>Type:</b></dt><dd>VT_INT</dd></dl>
*/
// {84F437B4-55DD-4B7C-BE0A-43EE1E34C726}
static const GUID MC_OUT_MATR_COEFFS =
{ 0x84f437b4, 0x55dd, 0x4b7c,{ 0xbe, 0xa, 0x43, 0xee, 0x1e, 0x34, 0xc7, 0x26 } };


//
//    Audio Encoder's parameters 
//
// Joint coding mode for stereo audio streams (mid-side, intensity or dual channel)
// {706A844B-462C-49B2-991C-131988F3DD64}
static const GUID MC_JOINT_CODING =
{ 0x706A844B, 0x462C, 0x49B2, { 0x99, 0x1C, 0x13, 0x19, 0x88, 0xF3, 0xDD, 0x64 } };

// {B2A86C3A-6D1B-4384-B8B5-735497EFAF9E}
static const GUID MC_BANDWIDTH =
{ 0xB2A86C3A, 0x6D1B, 0x4384, { 0xB8, 0xB5, 0x73, 0x54, 0x97, 0xEF, 0xAF, 0x9E } };

// Enable CRC protection or not
// {597881AE-E89C-4AC8-A15A-BA3DB2BDC15A}
static const GUID MC_CRC_PROTECTION =
{ 0x597881AE, 0xE89C, 0x4AC8, { 0xA1, 0x5A, 0xBA, 0x3D, 0xB2, 0xBD, 0xC1, 0x5A } };


//
//    Statistics 
//

// FLAGS

/**
<dl><dt><b>GUID:</b></dt><dd>
{5CCB6D1B-BFCC-4F88-92DC-ADEAADF78B34}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Enables overall PSNR calculation.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
// Calculate overall PSNR or not.
// {5CCB6D1B-BFCC-4F88-92DC-ADEAADF78B34}
static const GUID MC_CALC_PSNR =
{ 0x5CCB6D1B, 0xBFCC, 0x4F88, { 0x92, 0xDC, 0xAD, 0xEA, 0xAD, 0xF7, 0x8B, 0x34 } };


// READONLY

/**
<dl><dt><b>GUID:</b></dt><dd>
{16A9C0E0-9565-4AC8-91C7-82F5B59A0391}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Displays encoded and delivered frames (read-only).
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
// Encoded and delivered frames (read-only)
// {16A9C0E0-9565-4AC8-91C7-82F5B59A0391}
static const GUID MC_ENCODED_FRAMES =
{ 0x16A9C0E0, 0x9565, 0x4AC8, { 0x91, 0xC7, 0x82, 0xF5, 0xB5, 0x9A, 0x3, 0x91 } };

//Output message from DS filters
// {DBF0C01B-632E-4928-A08B-4DE2E9BD7F32}
static const GUID MC_DS_OUTPUT_MESSAGE =
{ 0xdbf0c01b, 0x632e, 0x4928, { 0xa0, 0x8b, 0x4d, 0xe2, 0xe9, 0xbd, 0x7f, 0x32 } };

/**
Retrieves the current frame rate (Frames per second). Should return actual speed per second (system time). Read-only.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UINT 
</dd></dl> \hideinitializer */ // {A1885A39-275D-45ED-8AC1-DB37FA64D7CD}
static const GUID MC_FPS =
{ 0xA1885A39, 0x275D, 0x45ED, { 0x8A, 0xC1, 0xDB, 0x37, 0xFA, 0x64, 0xD7, 0xCD } };

/**
<dl><dt><b>GUID:</b></dt><dd>
{F0DCB6C2-F08B-4C87-97BF-E62E09C64C5D}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Displays overall PSNR (over luma and chroma), read-only.
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
// Overall PSNR (luma and chroma).
// {F0DCB6C2-F08B-4C87-97BF-E62E09C64C5D}
static const GUID MC_OVERALL_PSNR =
{ 0xF0DCB6C2, 0xF08B, 0x4C87, { 0x97, 0xBF, 0xE6, 0x2E, 0x9, 0xC6, 0x4C, 0x5D } };

/**
<dl><dt><b>GUID:</b></dt><dd>
{8AC75EE6-7F3A-4D51-BE5C-BCC510EC3EB5}
</dd></dl>
<dl><dt><b>Description:</b></dt><dd>
Displays real average bitrate (read-only).
</dd></dl>
<dl><dt><b>Type:</b></dt><dd>
VT_INT
</dd></dl>
*/
// Real average bitrate
// {8AC75EE6-7F3A-4D51-BE5C-BCC510EC3EB5}
static const GUID MC_BITRATE_REAL =
{ 0x8AC75EE6, 0x7F3A, 0x4D51, { 0xBE, 0x5C, 0xBC, 0xC5, 0x10, 0xEC, 0x3E, 0xB5 } };


//! Video decoder's params
namespace MC_COMMON
{
    enum State {
        State_Off = 0, /*!< */
        State_On  = 1  /*!< */
    };

    /*!  */
	enum DeinterlaceMode {
		Deinterlace_Weave              = 0, /*!< No deinterlace */
		Deinterlace_VerticalFilter     = 1, /*!< Interpolation, using info from both fields */
		Deinterlace_FieldInterpolation = 2, /*!< Stretch one of fields */
		Deinterlace_VMR                = 3, /*!< */
		Deinterlace_Auto               = 4  /*!< */
	};

    /*! Selects skip mode(I,IP,IBP) or obey quality messages */
    enum QualityMode {
        Quality_ObeyQualityMessages     = 0, /*!<  */
        Quality_I                       = 1, /*!< Skip P and B frames even if they are used as reference. */
        Quality_IP                      = 2, /*!< Skip B frames even if they are used as reference. */
        Quality_IPB                     = 3  /*!< Do not skip frames. */
    };

    enum LoopFilterMode {
        LoopFilter_Off                  = 0,
        LoopFilter_Standard             = 1,
        LoopFilter_Reference            = 2
    };

    enum ErrorConcealmentMode {
        ErrorConcealment_Off            = 0,
        ErrorConcealment_NotShowFramesWithErrors = 1,
        ErrorConcealment_Temporal       = 2,
        ErrorConcealment_Spatial        = 3,
        ErrorConcealment_Smart          = 4,
        ErrorConcealment_ConcealErrorFrames = 5
    };

    typedef struct _CapturePictureInfo {
        VOID * pBuffer;
        SIZE_T szBuffer;
    } CapturePictureInfo;

	typedef struct _CaptureUserData {
		VOID * pBuffer;
		SIZE_T szBuffer;
	} CaptureUserData;

	enum AspectRatio {
		DefaultAR = 0,
		AR_1x1    = 1,
		AR_4x3    = 2,
		AR_16x9   = 3
	};
    /*! SMP mode */
	enum SMPMode {
		SMPMode_Off        = 0, /*!< Use only one thread */
		SMPMode_Auto       = 1, /*!< Let the filter decide on one of the following */
		SMPMode_ByPictures = 2, /*!< Parallelization by pictures */
		SMPMode_BySlices   = 3  /*!< Parallelization by slices */
	};

	enum HWACCState {
		HWACCState_Off   = 0,
		HWACCState_DXVA1 = 1,
		HWACCState_DXVA2 = 2,
        HWACCState_DXVA2_copyback = 3
	};

    /** \brief Hardware acceleration modes. Accepted by MPEG-2, VC-1, AVC, HEVC decoders. */
    enum HWACCMode
    {
        HWACCMode_Off    = 0, /**< \brief Enables software decoding only. \hideinitializer */
        HWACCMode_Auto   = 1, /**< \brief Enables hardware acceleration if available. \hideinitializer */
        HWACCMode_HWOnly = 2, /**< \brief Enables hardware acceleration only. If hardware acceleration is unavailable, decoder will reject pin connection. \hideinitializer */
    };

#ifndef __STREAM_USER_DATA_INTERFACE_DEFINED__
#define __STREAM_USER_DATA_INTERFACE_DEFINED__

 /* interface IModuleConfig */
 /* [unique][uuid][local][object] */ 


 //EXTERN_C const IID IID_IFilterCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

 MIDL_INTERFACE("25B1563C-EE23-478B-9035-D959DEA16749")
IStreamUserDataCallback
{
 public:
   IStreamUserDataCallback(){};                                    //default constructor
   virtual void UserDataCallback(void * buf, int size) = 0; //Should be overwritten in customer application
};

#endif
#endif 	/* __STREAM_USER_DATA_INTERFACE_DEFINED__ */
}

/**
This method enables/disables DXVA hardware acceleration (if available). DXVA allows you to decode video data using the video adapter GPU that reduces the CPU usage. In the hardware mode the output video pin of the decoder can connect only to filters with input pins exposing the AMVideoAccelerator interface (standard filters such as VMR-7, VMR-9, Overlay Mixer, Overlay Mixer2). Use the hardware mode only in cases of H.264/AVC video stream playback. If you need the uncompressed video data on the decoder output pin for further processing (e.g. compression), use the software mode.
\note Please keep in mind that you might need to use EVR (Enhanced Video Renderer) for DXVA 2.0 hardware acceleration.

\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
\ref MC_COMMON::HWACCMode_Off "HWACCMode_Off" - \copybrief MC_COMMON::HWACCMode_Off <br>
\ref MC_COMMON::HWACCMode_Auto "HWACCMode_Auto" - \copybrief MC_COMMON::HWACCMode_Auto <br>
\ref MC_COMMON::HWACCMode_HWOnly "HWACCMode_HWOnly" - \copybrief MC_COMMON::HWACCMode_HWOnly
</dd></dl> \hideinitializer */ // {BA6DDF74-5F8A-4BDC-88BB-A2563314BC3E}
static const GUID MC_HardwareAcceleration =
{ 0xBA6DDF74, 0x5F8A, 0x4BDC, { 0x88, 0xBB, 0xA2, 0x56, 0x33, 0x14, 0xBC, 0x3E } };

/**
Parameter to get hardware acceleration status, i.e. whether it is used or currently not. Read-only flag.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
HWACCState_Off - Software decoding. <br>
HWACCState_DXVA1 - Hardware accelerated decoding. DXVA1.01 API is in use. <br>
HWACCState_DXVA2 - Hardware accelerated decoding. DXVA2.0 API is in use.
</dd></dl> \hideinitializer */ // {D035CBA4-15C2-490F-B694-C15D77A4B788}
static const GUID MC_HardwareAccelerationState =
{ 0xd035cba4, 0x15c2, 0x490f, { 0xb6, 0x94, 0xc1, 0x5d, 0x77, 0xa4, 0xb7, 0x88 } };

/**
Enables or disables the compression artifacts removing.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_INT </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Postprocess filters are switched off. <br>
1 - Deblocking filter must be applied to decoded pictures.
</dd></dl> \hideinitializer */ // {9CF1A334-E72B-4A6D-BBE8-199595944546}
static const GUID MC_PostProcess =
{ 0x9CF1A334, 0xE72B, 0x4A6D, { 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46 } };

/**
Sets the brightness level.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_I4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - 255
</dd></dl> \hideinitializer */// {9CF1A336-E72B-4A6D-BBE8-199595944546}
static const GUID MC_Brightness =
{ 0x9CF1A336, 0xE72B, 0x4A6D, { 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46 } };

/**
Sets the contrast level.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_I4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - 255
</dd></dl> \hideinitializer */// {23E2776D-3BB5-4AA5-9563-4DBE5AF9539E}
static const GUID MC_Contrast =
{ 0x23E2776D, 0x3BB5, 0x4AA5, { 0x95, 0x63, 0x4D, 0xBE, 0x5A, 0xF9, 0x53, 0x9E } };

/**
Specifies the deinterlace mode.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Deinterlace off. <br>
1 - Vertical filter. <br>
2 - Field interpolation. <br>
3 - VMR (Video Mixing Renderer) <br>
4 - Auto mode, if stream interlace - field interpolation. 
</dd></dl> \hideinitializer */ // {9CF1A332-E72B-4A6D-BBE8-199595944546}
static const GUID MC_Deinterlace =
{ 0x9CF1A332, 0xE72B, 0x4A6D, { 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46 } };

/**
Selects skip mode(I,IP,IBP) or obey quality messages
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
\ref MC_COMMON::Quality_ObeyQualityMessages "Quality_ObeyQualityMessages" - Respect quality messages from downstream filter and choose frame types to skip automatically to maintain synchronized playback.  <br>
\ref MC_COMMON::Quality_I "Quality_I" - Skip P and B frames even if they are used as reference.  <br>
\ref MC_COMMON::Quality_IP "Quality_IP" - Skip B frames even if they are used as reference.  <br>
\ref MC_COMMON::Quality_IPB  "Quality_IPB" - Do not skip frames.
</dd></dl> \hideinitializer */ // {9CF1A330-E72B-4A6D-BBE8-199595944546}
static const GUID MC_Quality =
{ 0x9CF1A330, 0xE72B, 0x4A6D, { 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x46 } };

/**
Enables/disables "On screen display" feature (displaying of the decoding statistical information such as current frame type, number of decoded frames, number of skipped frames).
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_INT </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
\ref MC_COMMON::State_Off "State_Off" - OSD feature is switched off.  <br>
\ref MC_COMMON::State_On "State_on" - OSD feature is switched on.
</dd></dl> \hideinitializer */ // {F5C51906-ED89-4C6E-9C37-A5CCB34F5389}
static const GUID MC_OSD =
{ 0xF5C51906, 0xED89, 0x4C6E, { 0x9C, 0x37, 0xA5, 0xCC, 0xB3, 0x4F, 0x53, 0x89 } };


/**
Captures the current picture.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_BYREF
\note For the frame capturing the IModuleConfig::GetValue method must be called with the pointer to a non-initialized VARIANT variable as the second parameter. Decoder allocates a memory block for the picture and CapturePictureInfo structure (see common_mc.h). At the beginning of the allocated buffer there is the CapturePictureInfo structure and then the BITMAPINFOHEADER structure followed by the captured frame. The VARIANT::byref pointer must be casted to the CapturePictureInfo pointer. When all operations with the captured picture are finished, you need to call CoTaskMemFree(Var.byref).

Here the sample to illustrate using of this option:
\code
	VARIANT	Var;
	if (SUCCEEDED(pIModuleConfig->GetValue(&MC_CapturePicture, &Var))
	{
		CapturePictureInfo * pPicture = (CapturePictureInfo *)Var.byref;

		//Work with captured picture

		CoTaskMemFree(Var.byref);
	}
\endcode
</dd></dl> \hideinitializer */ // {6118A160-0FF0-43C8-94E4-345AC9E9F362}
static const GUID MC_CapturePicture =
{ 0x6118A160, 0xFF0, 0x43C8, { 0x94, 0xE4, 0x34, 0x5A, 0xC9, 0xE9, 0xF3, 0x62 } };

// {BE21D0FD-B531-4F98-AB7B-104BCF9B1330}
static const GUID MC_CaptureUserData = 
{ 0xbe21d0fd, 0xb531, 0x4f98, { 0xab, 0x7b, 0x10, 0x4b, 0xcf, 0x9b, 0x13, 0x30 } };

/**
Retrieves the count of decoded frames (read only).
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UINT 
</dd></dl> \hideinitializer */
// {575BA759-6F13-4A84-A126-005A5523D203}
static const GUID MC_FramesDecoded =
{ 0x575BA759, 0x6F13, 0x4A84, { 0xA1, 0x26, 0x0, 0x5A, 0x55, 0x23, 0xD2, 0x3 } };

/**
Retrieves the count of skipped frames (read only).
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UINT 
</dd></dl> \hideinitializer */
// {592A9FAB-CBF8-4592-A0B2-21D0C79DACE4}
static const GUID MC_FramesSkipped =
{ 0x592A9FAB, 0xCBF8, 0x4592, { 0xA0, 0xB2, 0x21, 0xD0, 0xC7, 0x9D, 0xAC, 0xE4 } };

/**
The option specifies the use of multi-thread if available (available on multi-processors systems, and only in software decode mode). It also specifies the way of parallel decoding.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
\ref MC_COMMON::SMPMode_Off "SMPMode_Off" - Serial decoding. <br>
\ref MC_COMMON::SMPMode_Auto "SMPMode_Auto" - Check for multi-threading and use "by picture" mode if it is available. <br>
\ref MC_COMMON::SMPMode_ByPictures "SMPMode_ByPictures" - Parallel decoding by pictures. <br>
\ref MC_COMMON::SMPMode_BySlices "SMPMode_BySlices" - Parallel decoding by slices (it is actual for multi-slice pictures). <br>
</dd></dl> \hideinitializer */ // {0612C1C6-DEF7-4D01-A0DA-90F426F9B312}
static const GUID MC_MultiThread =
{ 0x612C1C6, 0xDEF7, 0x4D01, { 0xA0, 0xDA, 0x90, 0xF4, 0x26, 0xF9, 0xB3, 0x12 } };


/**
Specifies the number of threads that will be used for decoding. When the value is equal to "0", the decoder uses the number of threads which are equal to the number of processors that are installed on the PC.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - 16
</dd></dl> \hideinitializer */ // {39108716-B2F5-491C-9E9B-C2ABF38475B4}
static const GUID MC_NumThreads =
{ 0x39108716, 0xb2f5, 0x491c, { 0x9e, 0x9b, 0xc2, 0xab, 0xf3, 0x84, 0x75, 0xb4 } };

/**
Retrieves the beginning media time of current frame (it may be a byte offset of current frame)
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_I8
</dd></dl> \hideinitializer */
// {3903A73E-6A89-4E09-8E9F-95E8A56F614D}
static const GUID MC_MediaTimeStart =
{ 0x3903A73E, 0x6A89, 0x4E09, { 0x8E, 0x9F, 0x95, 0xE8, 0xA5, 0x6F, 0x61, 0x4D } };

/**
Retrieves the ending media time of current frame (it may be a byte offset of current frame)
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_I8
</dd></dl> \hideinitializer */
// {BCF5D243-B80E-400A-9B60-035A1D3E5C38}
static const GUID MC_MediaTimeStop =
{ 0xBCF5D243, 0xB80E, 0x400A, { 0x9B, 0x60, 0x3, 0x5A, 0x1D, 0x3E, 0x5C, 0x38 } };

/**
Reset decoder statistics (decoded and skipped frames counter).
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4
</dd></dl> \hideinitializer */
// Reset decoder statistics (decoded and skipped frames counter).
// {164966A1-2BFD-4C74-A80E-E5769A219B9F}
static const GUID MC_ResetStatistics =
{ 0x164966A1, 0x2BFD, 0x4C74, { 0xA8, 0xE, 0xE5, 0x76, 0x9A, 0x21, 0x9B, 0x9F } };

/**
The option specifies how the decoder should handle frames with mistakes or errors. 
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - ErrorConcealment_Off - Decode and show all frames.  <br>
1 - ErrorConcealment_NotShowFramesWithErrors - Don't display frames in which mistakes or errors are detected. <br>
5 - ErrorConcealment_ConcealErrorFrames - Do error concelament process to hide broken regions of frame.
</dd></dl> \hideinitializer */ // {BB8F00E9-655B-47C4-966A-A4B4BBF8D2D2}
static const GUID MC_ErrorConcealment =
{ 0xBB8F00E9, 0x655B, 0x47C4, { 0x96, 0x6A, 0xA4, 0xB4, 0xBB, 0xF8, 0xD2, 0xD2 } };

/**
Enable to preserve aspect ratio of the source video when output video pin is connected to the VMR-7,9 
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Do not preserve aspect ratio of the source file. <br>
1 - Preserve aspect ratio of the source file.
</dd></dl> \hideinitializer */ // {7F261D15-62E2-46E5-86EB-3537DD72DD56}
static const GUID MC_VMRMaintainAspectRatio =
{ 0x7F261D15, 0x62E2, 0x46E5, { 0x86, 0xEB, 0x35, 0x37, 0xDD, 0x72, 0xDD, 0x56 } };

// Set/get VESA settings (VESA_settings_t, mcvesa.h)
// {B1E68179-E7A1-4CF9-96B5-4D8F9E671C6C}
static const GUID MC_VESA_SETTINGS =
{ 0xb1e68179, 0xe7a1, 0x4cf9, { 0x96, 0xb5, 0x4d, 0x8f, 0x9e, 0x67, 0x1c, 0x6c } };

//Get VESA buffers (VESA_buffers_t, mcvesa.h)
// {9238CD58-BC76-4679-90AB-808168CB720C}
static const GUID MC_VESA_BUFFERS =
{ 0x9238cd58, 0xbc76, 0x4679, { 0x90, 0xab, 0x80, 0x81, 0x68, 0xcb, 0x72, 0xc } };

// {4B5E5B27-8D3F-492A-8B28-1265E7E5CE51}
static const GUID MC_LowLatency = 
{ 0x4b5e5b27, 0x8d3f, 0x492a, { 0x8b, 0x28, 0x12, 0x65, 0xe7, 0xe5, 0xce, 0x51 } };

/**
Specifies whether the decoder should display the output frames in stream order or not.
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Feature is disabled (default). <br>
1 - Feature is enabled.
</dd></dl> \hideinitializer */ // {938C1C39-2D34-4EE5-9789-E67C0D1ADD40}
static const GUID MC_StreamOrder =
{ 0x938c1c39, 0x2d34, 0x4ee5, { 0x97, 0x89, 0xe6, 0x7c, 0xd, 0x1a, 0xdd, 0x40 } };

// {0C58E5D5-B706-4724-8F00-43C65773F75D}
static const GUID MC_OutputQueue =
{ 0xc58e5d5, 0xb706, 0x4724, { 0x8f, 0x0, 0x43, 0xc6, 0x57, 0x73, 0xf7, 0x5d } };

// {4B0A9195-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputYUY2 =
{ 0x4b0a9195, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A9196-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputUYVY =
{ 0x4b0a9196, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A9197-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputYV12 =
{ 0x4b0a9197, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A9198-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputRGB32 =
{ 0x4b0a9198, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A9199-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputRGB24 =
{ 0x4b0a9199, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A919A-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputRGB565 =
{ 0x4b0a919a, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A919B-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputRGB555 =
{ 0x4b0a919b, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

// {4B0A919C-A6AE-41F6-BF58-8321D76E1661}
static const GUID MC_OutputAYUV =
{ 0x4b0a919c, 0xa6ae, 0x41f6, { 0xbf, 0x58, 0x83, 0x21, 0xd7, 0x6e, 0x16, 0x61 } };

/**
The option specifies pixel range (CLAMP). 
\details <dl><dt><b>  Type: </b></dt><dd> 
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>

0 - No conversion. Keep short range if VUI-video_full_range_flag==0 and use color resampling (for example 4:2:0 to 4:2:2 ). <br>
1 - Conversion to short range [16..235]. If VUI-video_full_range_flag==1.<br>
2 - Conversion to full range [0..255]. If VUI-video_full_range_flag==0. <br>
3 - Force no conversion. Ignore VUI-video_full_range_flag. <br>
4 - Force conversion to short range [16..235]. Ignore VUI-video_full_range_flag. <br>
5 - Force keep short range [16..235]. Ignore VUI-video_full_range_flag. <br>
6 - Force conversion to full range [0..255]. Ignore VUI-video_full_range_flag. <br>
</dd></dl> \hideinitializer */ // {BB8F00E9-655B-47C4-966A-A4B4BBF8D2D2}
// {86CECED2-A106-424E-9D40-B66282D8EF6C}
static const GUID MC_PixRangeFlag = 
{ 0x86ceced2, 0xa106, 0x424e, { 0x9d, 0x40, 0xb6, 0x62, 0x82, 0xd8, 0xef, 0x6c } };

//was added for NextTalk customer 
// {CDA79DE8-534A-4C45-9DCF-01E266407F2F}
static const GUID MC_IgnoreDiscontinuity = 
{ 0xcda79de8, 0x534a, 0x4c45, { 0x9d, 0xcf, 0x1, 0xe2, 0x66, 0x40, 0x7f, 0x2f } };

#endif // _COMMON_MC_
