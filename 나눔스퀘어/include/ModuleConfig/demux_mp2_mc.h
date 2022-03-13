/*!
 * @file demux_mp2_mc.h
 * @brief Property GUIDs for MainConcept MP2 pull demuxer parameters.
 * File: demux_mp2_mc.h
 *
 * Desc: Property GUIDs for MainConcept MP2 pull demuxer parameters.
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * 
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUID                           Value Type    Available range     Default     Note

// EMPGDMX_SUBPICTURE_SUPPORT       VT_I4       0,1                 0	        This parameter enables or disables the subpicture support.
// EMPGDMX_INIT_MODE	            VT_I4       0,1                 0	        This parameter specifies algorithm of the initial stream duration calculation
// EMPGDMX_INIT_DATA_LENGTH         VT_UI4      1000-ULONG_MAX      4194304     This parameter specifies the data length (in bytes) that is read and analized during the initial detection of streams.
// EMPGDMX_SEEK_DATA_LIMIT          VT_UI4      0-ULONG_MAX         0           This parameter specifies a limit (in bytes) of the number of bytes to process during a seek.
// EMPGDMX_DISABLE_ACCURATENAVIGATION   VT_I4   0,1                 0           When this parameter is equal to 1, the demultiplexer simply calculates the position on the basis of bitrate information and does not perform the stream scanning.
// EMPGDMX_DISABLE_NAVIGATION       VT_I4       0,1      		    0           This parameter enables or disables the demultiplexer navigation abilities.
// EMPGDMX_ALWAYS_PTS_NAVIGATION    VT_I4       0,1          		0           This parameter sets the PTS navigation as the preferred navigation mode. If the correct navigation using PTS is impossible the navigation mode is switched to the navigation using bitrate.
// EMPGDMX_DISABLE_NAVIGATIONINITSCAN   VT_I4	0,1    	            0	        If this parameter is equal to 1 then the demultiplexer does not try to scan data at the end of stream and the duration is calculated only on basis of bitrate. 
// EMPGDMX_EXTERNAL_DURATION        VT_I8	    0-_I64_MAX	        _I64_MAX    This parameter allows specifying the stream duration which will be returned with the IMediaSeeking interface. The value is specified in 100 ns. units.
// EMPGDMX_PTSJUMPTHRESHOLD         VT_I8       0-_I64_MAX          15000000    This parameter allows specifying the maximum threshold value for the PTS discontinuity detection. The value is specified in 100 ns. units.      
// EMPGDMX_STREAMTYPE               VT_I4       Read only           0           This parameter contains the type of the detected stream after connecting to the source. Described in MC_COMMON::STREAM_TYPE.
// EMPGDMX_BASETIME                 VT_I8       0-_I64_MAX          _I64_MAX    This parameter contains the time of base PTS of stream. The value is specified in 100 ns. units.      
// EMPGDMX_INDEX_MODE				VT_I4		0,1,2,3				0			0 - noindex, 1 - load index if present, 2 - load index always, 3 - as 2 and store index
// EMPGDMX_INDEX_LOAD				VT_BSTR										Force load index - if empty string, then load default name index
// EMPGDMX_INDEX_SAVE				VT_BSTR										Force save index - if empty string, then save default name index
// EMPGDMX_TS_TYPE_TABLE          VT_SAFEARRAY							{ USHORT ,USHORT } = original type and type for replacement
// EMPGDMX_DISABLE_IDR_SEARCH     VT_I4         0,1                  0          During  navigation  disable search for IDR (H264 streams). It is  correct to start  playback   from IDR but some streams has no IDRs or just  one at the  begining
// EMPGDMX_IDR_SEARCH_MODE        VT_I4         0,1                  0          If H.264 IDR searching is enabled, this specifies the action to take if an IDR frame is not found.

#if !defined(__PROPID_PULLDEMUXERH25320031526_)
#define __PROPID_PULLDEMUXERH25320031526_

#include "common_mc.h"

/*!
@brief This parameter enables or disables the subpicture support.
@details
<dl><dt>Type:           </dt>   <dd>  VT_BOOL  </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Demultiplexer does not generate an output pin for the subpicture stream.
 - 1 - Demultiplexer generates an output pin for the subpicture stream (if the processed stream contains subpictures).
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0        </dd></dl>
@hideinitializer
*/
// {9DA8B680-8F52-4a88-806D-EBEB289BBB25}
static const GUID EMPGDMX_SUBPICTURE_SUPPORT = { 0x9da8b680, 0x8f52, 0x4a88, { 0x80, 0x6d, 0xeb, 0xeb, 0x28, 0x9b, 0xbb, 0x25 } };

/*!
@brief This parameter specifies algorithm of the initial stream duration calculation.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4  </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Normal initialization mode, scans the stream and checks monotony of the PTS increment. If PTS is incremented monotonically, the duration is calculated by PTS. If there is a gap or reset of PTS, the duration is calculated by bitrate.
 - 1 - Simple initialization mode, does not perform the stream scanning to check monotony of the PTS increment. The duration is always calculated by PTS.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0        </dd></dl>
@hideinitializer
*/
// {64B52AAA-C4E5-45be-AD97-82B5E247FFCA}
static const GUID EMPGDMX_INIT_MODE = { 0x64b52aaa, 0xc4e5, 0x45be, { 0xad, 0x97, 0x82, 0xb5, 0xe2, 0x47, 0xff, 0xca } };

/*!
@brief This parameter specifies the data length (in bytes) that is read and analyzed during the initial detection of streams.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4            </dd></dl>
<dl><dt>Available range:</dt>   <dd>  1000–ULONG_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  4194304           </dd></dl>
@hideinitializer
*/
// {2BB2DCDC-E4AD-4ea6-B3C7-7D5ACA983968}
static const GUID EMPGDMX_INIT_DATA_LENGTH = { 0x2bb2dcdc, 0xe4ad, 0x4ea6, { 0xb3, 0xc7, 0x7d, 0x5a, 0xca, 0x98, 0x39, 0x68 } };

/*!
@brief This parameter specifies a limit (in bytes) of the number of bytes to process during a seek.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4       </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0–ULONG_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0 (no limit) </dd></dl>
@hideinitializer
*/
// {5DA413D7-8F31-44F0-A035-088F31D3FC8B}
static const GUID EMPGDMX_SEEK_DATA_LIMIT = { 0x5da413d7, 0x8f31, 0x44f0, { 0xa0, 0x35, 0x8, 0x8f, 0x31, 0xd3, 0xfc, 0x8b } };

/*!
@brief Normally during the navigation process the demultiplexer scans the stream data to find the frame which corresponds to the requested navigation time. When this parameter is equal to 1, the demultiplexer simply calculates the position on the basis of bitrate information and does not perform the stream scanning.
@details
<dl><dt>Type:           </dt>   <dd>  VT_BOOL            </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Accurate navigation is enabled.
 - 1 - Accurate navigation is disabled.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0                  </dd></dl>
@hideinitializer
*/
// {4CEC0C13-A72D-42e4-90DF-CE8B91D5C5E1}
static const GUID EMPGDMX_DISABLE_ACCURATENAVIGATION = { 0x4cec0c13, 0xa72d, 0x42e4, { 0x90, 0xdf, 0xce, 0x8b, 0x91, 0xd5, 0xc5, 0xe1 } };

/*!
@brief This parameter enables or disables the demultiplexer navigation abilities.
@details
<dl><dt>Type:           </dt>   <dd>  VT_BOOL            </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Demultiplexer performs navigation when it receives the navigation command.
 - 1 - Demultiplexer does not perform navigation when it receives the navigation
command.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0                  </dd></dl>
@hideinitializer
*/
// {36CBFB53-6D75-4ccd-B67A-CFA4093E20F3}
static const GUID EMPGDMX_DISABLE_NAVIGATION = { 0x36cbfb53, 0x6d75, 0x4ccd, { 0xb6, 0x7a, 0xcf, 0xa4, 0x9, 0x3e, 0x20, 0xf3 } };

/*!
@brief This parameter sets the PTS navigation as the preferred navigation mode. If the correct navigation using PTS is impossible the navigation mode is switched to the navigation using bitrate.

“PTS navigation” means to use PTS on PES packets during navigation to a specified
position. The demuxer tries to detect PTS’s discontinuity and other problems, and then can reject to navigate using PTS. It can use “On-Bitrate Navigation” instead. Usually for PS and TS streams, for which the PTS can be read, the PTS is used during navigation process. During navigation the “nearest” PTS is searched. (i.e. it searches the packet with MIN(|PTS(i) – BaseTime – Time_req | ) where (i) is the packet number and Time_req is converted to units used in PTS. Base_Time is the PTS of the position from which the streaming starts). Then the exact position of the requested frame is calculated. Since not every frame has PTS, the PTS for frames without PTS on PES is calculated. At last, the frame with a timestamp “nearest” to the requested position is searched.

There is no direct why to seek using absolute PTS value. But the Pull Demultiplexer provides the BaseTime value for the stream.
@details
<dl><dt>Type:           </dt>   <dd>  VT_BOOL            </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Using the PTS navigation when it is possible and switching to the bitrate
navigation in other cases.
 - 1 - PTS navigation is forced at all navigation calls.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0                  </dd></dl>
@hideinitializer
*/
// {F02C83CA-29E3-4044-BFD4-59906D358683}
static const GUID EMPGDMX_ALWAYS_PTS_NAVIGATION = { 0xf02c83ca, 0x29e3, 0x4044, { 0xbf, 0xd4, 0x59, 0x90, 0x6d, 0x35, 0x86, 0x83 } };

/*!
@brief At the initialization process in order to calculate the duration, the demultiplexer tries to read the last PTS from the stream and calculates the duration as difference between the first and last PTS. If this parameter is equal to 1 then the demultiplexer does not try to scan data at the end of stream and the duration is calculated only on basis of bitrate.
@details
<dl><dt>Type:           </dt>   <dd>  VT_BOOL            </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Normal process of the stream scanning.
 - 1 - Initial scanning is blocked.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0                  </dd></dl>
@hideinitializer
*/
// {25A0589F-C6AE-40b1-998E-FDF145ACA5EC}
static const GUID EMPGDMX_DISABLE_NAVIGATIONINITSCAN = { 0x25a0589f, 0xc6ae, 0x40b1, { 0x99, 0x8e, 0xfd, 0xf1, 0x45, 0xac, 0xa5, 0xec } };

/*!
@brief This parameter allows specifying the stream duration which will be returned with the IMediaSeeking interface. The demuxer tries to get the stream's duration by analyzing the stream. However, if the user knows the exact duration of the stream then it can be passed to the filter using this parameter, in case it fails to detect it. The value is specified in 100 ns. units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8       </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  _I64_MAX    </dd></dl>
@hideinitializer
*/
// {AC02E7D0-19D2-4ffe-90E4-DD7285E1515D}
static const GUID EMPGDMX_EXTERNAL_DURATION = { 0xac02e7d0, 0x19d2, 0x4ffe, { 0x90, 0xe4, 0xdd, 0x72, 0x85, 0xe1, 0x51, 0x5d } };

/*!
@brief This parameter allows specifying the maximum threshold value for the PTS discontinuity detection. The value is specified in 100 ns. units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8       </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  15000000    </dd></dl>
@hideinitializer
*/
// {FE8522C7-E8E6-44a2-B61A-DF72ECA6DB45}
static const GUID EMPGDMX_PTSJUMPTHRESHOLD = { 0xfe8522c7, 0xe8e6, 0x44a2, { 0xb6, 0x1a, 0xdf, 0x72, 0xec, 0xa6, 0xdb, 0x45 } };

/*!
@brief This parameter contains the type of the detected stream after connecting to the source.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4       </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Unknown stream type.
- 1 – System Stream.
- 2 – Program Stream.
- 3 – Transport Stream.
- 4 – MPEG1/2 video stream.
- 5 – PVA stream.
- 6 – MPEG1/2 audio stream.
- 7 – AC-3 stream.
- 8 – AVC video stream.
- 9 – MPEG-4 video stream.
- 10 – VC-1 video stream.
- 11 – H.263 video stream.
- 12 – MP4 stream.
- 13 – AAC stream.
- 14 – MPEG-1 video stream.
- 15 – MPEG-1 audio stream.
- 16 – DIV3 stream.
- 17 – DIV4 stream.
- 18 – DIV5 stream.
- 19 – VideoSoft AVC stream.
- 20 – PCM stream.
- 21 – DVD subpicture stream.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  unspecified  </dd></dl>
<dl><dt>Read only </dt>   <dd>  yes       </dd></dl>
@hideinitializer
*/
// {59D26E5F-6C2A-4406-B9B8-19B6A1C87EE1}
static const GUID EMPGDMX_STREAMTYPE = { 0x59d26e5f, 0x6c2a, 0x4406, { 0xb9, 0xb8, 0x19, 0xb6, 0xa1, 0xc8, 0x7e, 0xe1 } };

/*!
@brief This parameter contains the time of base PTS of stream. The value is specified in 100 ns. units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8       </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  _I64_MAX    </dd></dl>
@hideinitializer
*/
// {84512624-A6CE-4537-B0D7-2D10BA8DE6C1}
static const GUID EMPGDMX_BASETIME = { 0x84512624, 0xa6ce, 0x4537, { 0xb0, 0xd7, 0x2d, 0x10, 0xba, 0x8d, 0xe6, 0xc1 } };

/*!
@brief Specify the mode of use and create index. The mode should be changed BEFORE input
pin is connected. Loading, building and saving the Index occurs during InputPin connection process.
@note Index manipulating may consume a lot of resources and take some time until it is done.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4       </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Do not use index.
- 1 – Load present index.
- 2 – Load present index, if index is not present then parse stream.
- 3 – As 2 and store index.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0    </dd></dl>
@hideinitializer
*/
// {8CD97D7F-6606-4acd-8732-3F7A84862ED2}
static const GUID EMPGDMX_INDEX_MODE = { 0x8cd97d7f, 0x6606, 0x4acd, { 0x87, 0x32, 0x3f, 0x7a, 0x84, 0x86, 0x2e, 0xd2 } };

/*!
@brief Default index name generated as full stream name with additional “.inx”. This parameter is only for reference in current release and could not be changed.
@details
<dl><dt>Type:     </dt>   <dd>  VT_BSTR   </dd></dl>
<dl><dt>Read only </dt>   <dd>  yes       </dd></dl>
@hideinitializer
*/
// {B64E15C7-57AB-40d9-BD91-5A7FD029F071}
static const GUID EMPGDMX_INDEX_LOAD = { 0xb64e15c7, 0x57ab, 0x40d9, { 0xbd, 0x91, 0x5a, 0x7f, 0xd0, 0x29, 0xf0, 0x71 } };

/*!
@brief Default index name generated as full stream name with additional “.inx”. This parameter is only for reference in current release and could not be changed.
@details
<dl><dt>Type:     </dt>   <dd>  VT_BSTR   </dd></dl>
<dl><dt>Read only </dt>   <dd>  yes       </dd></dl>
@hideinitializer
*/
// {97C096E0-3B2E-4b56-A030-2164D3B940F9}
static const GUID EMPGDMX_INDEX_SAVE = { 0x97c096e0, 0x3b2e, 0x4b56, { 0xa0, 0x30, 0x21, 0x64, 0xd3, 0xb9, 0x40, 0xf9 } };

/*!
@brief This parameter allows to manually specify stream types and ignore appropriate values taken from PMT.
@details
The replacement values are specified as an array of stream PIDs and stream types.

The array should have the following format: { stream PID, stream type, stream PID, stream type, ... }
<dl><dt>Type:     </dt>   <dd>  VT_ARRAY of VT_UI2  </dd></dl>
@hideinitializer
*/
// {2177BB43-C806-4f9d-930D-25ECEF4B8EF7}
static const GUID EMPGDMX_TS_TYPE_TABLE = { 0x2177bb43, 0xc806, 0x4f9d, { 0x93, 0xd, 0x25, 0xec, 0xef, 0x4b, 0x8e, 0xf7 } };

/*!
@brief Some H.264/AVC streams have no IDR frames at all or only one IDR frame per stream (e.g. AVCHD streams from Sony camcorders). The demuxer tries to find IDR frames during navigation. Since the only correct point to start decoding is an IDR frame, and the decoder should get all the data starting from an IDR to correctly decode the requested frame, the navigation for such specific streams takes too much time (demuxer should parse stream from one IDR to another and decoder should decode all frames starting from IDR.).

In case you are sure that there is no “long term” reference used during encoding, IDR search can be replaced by I-frame search. If the long term reference is used and we only search for an I-frame (not IDR) then some frames will be decoded with errors.

So this option is intended to give the user the ability to choose the appropriate behavior of the demuxer.
@details
<dl><dt>Type:     </dt>   <dd>  VT_BOOL   </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Search for IDR frames in H.264 streams (default).
- 1 – Search for an I-slice instead of an IDR-slice.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0    </dd></dl>
@hideinitializer
*/
// {51A02F2A-5BA6-456e-A653-4AA923EDBF0E}
static const GUID EMPGDMX_DISABLE_IDR_SEARCH = { 0x51a02f2a, 0x5ba6, 0x456e, { 0xa6, 0x53, 0x4a, 0xa9, 0x23, 0xed, 0xbf, 0xe } };

/*!
@brief If H.264 IDR searching is enabled, this specifies the action to take if an IDR frame is not found.

Meant to be used with the EMPGDMX_SEEK_DATA_LIMIT setting to limit search range for IDR frames.

@details
<dl><dt>Type:     </dt>   <dd>  VT_UI4   </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Abort search (default).
- 1 – Fallback to search for an I-slice instead of an IDR-slice.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0    </dd></dl>
@hideinitializer
*/
// {4BC3DB6A-75A1-4B1A-B860-13BDD8170C9E}
static const GUID EMPGDMX_IDR_SEARCH_MODE = { 0x4bc3db6a, 0x75a1, 0x4b1a, { 0xb8, 0x60, 0x13, 0xbd, 0xd8, 0x17, 0xc, 0x9e } };

/*!
@brief The MediaTime can be any value that defines the absolute position in the stream, in contrast to the timestamps that are usually used on media samples. It is relative to the start of the stream (i.e. relative to zero). When the parameter is enabled (1), the byte offset for MediaTime is used. The PTS values from PES packets are used for MediaTime without any changes.
@details
<dl><dt>Type:     </dt>   <dd>  VT_BOOL   </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Do not use byte offset for MediaTime.
- 1 – Use byte offset for MediaTime.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0    </dd></dl>
@hideinitializer
*/
// {5DA61BAD-AF60-4005-9EDA-1C68B2AF4C25}
static const GUID EMPGDMX_DIRECT_PTS = { 0x5da61bad, 0xaf60, 0x4005, { 0x9e, 0xda, 0x1c, 0x68, 0xb2, 0xaf, 0x4c, 0x25 } };
/*!
@brief The first video PID belongs to the first audio PID if there is no PAT/PMT present.
@details
<dl><dt>Type:     </dt>   <dd>  VT_BOOL   </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Off: The first video PID does not belong to the first audio PID.
- 1 – On: The first video PID belongs to the first audio PID.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  1    </dd></dl>
@hideinitializer
*/
// {D48B8185-5E4F-43B7-9BB8-D7FA65DCBACF}
static const GUID EMPGDMX_ES_DETECTION = { 0xd48b8185, 0x5e4f, 0x43b7, { 0x9b, 0xb8, 0xd7, 0xfa, 0x65, 0xdc, 0xba, 0xcf } };
/*!
@brief Enables or disables elementary streams detection.
@details
<dl><dt>Type:     </dt>   <dd>  VT_BOOL   </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 – Off: Detect only MPEG-2 container formats (transport stream, program stream, system stream etc). 
- 1 – On: Enable ES detection in addition to containers.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  1    </dd></dl>
@hideinitializer
*/
// {9ED04885-CBF6-4643-A495-14C247BFCF62}
static const GUID EMPGDMX_DIRECT_TS_DETECTION = { 0x9ed04885, 0xcbf6, 0x4643, { 0xa4, 0x95, 0x14, 0xc2, 0x47, 0xbf, 0xcf, 0x62 } };
/*!
@brief This parameter contains stream time information. Stream time information is represented as an array of StreamTimeInfo structures, which contain stream PID, current graph timestamp and a timestamp taken from stream.
@details 
<dl><dt>Type:     </dt>         <dd>  VT_ARRAY of VT_RECORD  </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes                    </dd></dl>
@hideinitializer
*/
// {2D8B96C2-CCB3-42EA-953E-BB682358B52F}
static const GUID EMPGDMX_STREAMTIMES = { 0x2d8b96c2, 0xccb3, 0x42ea, { 0x95, 0x3e, 0xbb, 0x68, 0x23, 0x58, 0xb5, 0x2f } };

#endif //#if !defined(__PROPID_PULLDEMUXERH25320031526_)
