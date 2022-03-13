/*!
 * @file demuxpush_mp2_mc.h
 * @brief Property GUIDs for MainConcept MP2 push demuxer parameters.
 * File: demuxpush_mp2_mc.h
 *
 * Desc: Property GUIDs for MainConcept MP2 push demuxer parameters.
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 * 
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUID                           Value Type    Available range Default Note

// EMPGPDMX_INITIAL_PARSING_DONE   VT_I4        Read only       0	    Checks the process of the Demultiplexer initialization
// EMPGPDMX_ENABLE_TIME_ADJUST     VT_I4        0,1	            1	    Enables filter graph to use the reference clock provided by Demultiplexer
// EMPGPDMX_LATENCY_VALUE          VT_I4        0-_UI32_MAX     17	    Latency value in 30 ms units
// EMPGPDMX_TIME_ADJUST_MODE	   VT_UI4       0,1             0	    0 - Generate clock, 1 - modify timestamps
// EMPGPDMX_TIME_DELTA_STREAM      VT_I8        Read only       0       Maximum delta between streams in 100 ns units
// EMPGPDMX_TIME_DELTA_CLOCK       VT_I8        Read only       0       Current delta between input stream time and generated clock time in 100 ns units
// EMPGPDMX_PARSE_BUFFER_SIZE      VT_UI4       0-_UI32_MAX     20971520 Size of internal buffer for data in bytes
// EMPGPDMX_INIT_BUFFER_SIZE       VT_UI4       0-_UI32_MAX     102400  Minimal size of data used to detect stream type
// EMPGPDMX_DISCONTINUITY_GENERATE_DISABLE      VT_I4   0,1    	0	    Disable generate discontinuity to decoders when found data drop in input stream
// EMPGPDMX_DISCONTINUITY_GENERATE_FLUSHDATA    VT_I4	0,1	    0       Flush all data in queue to decoders when generated discontinuity
// EMPGPDMX_STREAMTYPE             VT_I4        Read only       0       This parameter contains the type of the detected stream after connecting to the source. Described in MC_COMMON::STREAM_TYPE.
// EMPGPDMX_BUFFERS_USED           VT_UI4       Read only       0       Count of used internal blocks
// EMPGPDMX_BUFFERS_COUNT          VT_UI4       Read only       0       Count of allocated internal blocks
// EMPGPDMX_GENERATE_PSIOUTPUT     VT_I4        0,1             0       Switch demultiplexer to manual pin creation and map PAT table data to otput pin named PSI
// EMPGPDMX_SIMPLE_TS_DETECT       VT_I4        0,1             0       Switch demultiplexer to simple detecting transport stream, only on PAT/PMT information
// EMPGPDMX_DISABLE_DESTROY_PINS   VT_I4        0,1             0       Disable destroy automatic generated pins on pin manual creation.
// EMPGPDMX_PTSJUMPTHRESHOLD       VT_I8        0-_I64_MAX      100000000    This parameter allows specifying the maximum threshold value for the PTS discontinuity detection. The value is specified in 100 ns. units.      
// EMPGPDMX_LASTSTREAMTIME         VT_SAFEARRAY							{ LONGLONG , LONGLONG } = last time on output and last time from PTS
// EMPGPDMX_TS_TYPE_TABLE          VT_SAFEARRAY							{ USHORT ,USHORT } = original type and type for replacement

#if !defined(__PropID_DemuxerPush_h_10_7_2005_17_17)
#define __PropID_DemuxerPush_h_10_7_2005_17_17

#include "common_mc.h"

/*!
@brief This parameter indicates the status of the stream detecting process.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4  </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Stream type is not detected.
 - 1 - Stream type is detected.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0      </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes    </dd></dl>
@hideinitializer
*/
// {D7366D60-EBCE-4c69-8BAD-3D1235E3B4B3}
static const GUID EMPGPDMX_INITIAL_PARSING_DONE = { 0xd7366d60, 0xebce, 0x4c69, { 0x8b, 0xad, 0x3d, 0x12, 0x35, 0xe3, 0xb4, 0xb3 } };

/*!
@brief This parameter enables the playback rate adjustment in accordance with the data receiving
rate. The adjustment may be needed to avoid the data buffer underflow or overflow, when
data is received from an external source (network, satellite).
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4  </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Playback rate adjustment is disabled.
 - 1 - Playback rate adjustment is enabled.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  1      </dd></dl>
@hideinitializer
*/
// {1B7AD5DB-0226-44eb-8270-2255FAE8CA3C}
static const GUID EMPGPDMX_ENABLE_TIME_ADJUST = { 0x1b7ad5db, 0x226, 0x44eb, { 0x82, 0x70, 0x22, 0x55, 0xfa, 0xe8, 0xca, 0x3c } };

/*!
@brief This parameter specifies the latency between the data receiving and playback, that may be
needed to avoid the playback jerking. The value is specified in 0.03 ms units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4         </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_UI32_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  17             </dd></dl>
@hideinitializer
*/
// {6AFFB5BE-C0E1-4644-A78A-F2B0F9BAE5B0}
static const GUID EMPGPDMX_LATENCY_VALUE = { 0x6affb5be, 0xc0e1, 0x4644, { 0xa7, 0x8a, 0xf2, 0xb0, 0xf9, 0xba, 0xe5, 0xb0 } };

/*!
@brief This parameter selects the method of the playback rate adjustment. The adjustment can be
performed in two ways: generation of the reference clock which time is adjusted in
accordance with the input data rate; modification of the time stamps on output samples
depending on the input data rate. The first way provides less reaction time but the second
one can be used without special configuration of a filter graph.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Adjustment of the generated reference clock.
 - 1 - Adjustment of the time stamps on output samples.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {A2649B3F-B593-47f2-958D-C022A9E189CB}
static const GUID EMPGPDMX_TIME_ADJUST_MODE = { 0xa2649b3f, 0xb593, 0x47f2, { 0x95, 0x8d, 0xc0, 0x22, 0xa9, 0xe1, 0x89, 0xcb } };

/*!
@brief This parameter indicates the maximum time difference between elementary streams. The
latency specified with the EMPGPDMX_LATENCY_VALUE parameter can not be less than
this parameter value. The parameter value is specified in 100 ns units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX   </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes            </dd></dl>
@hideinitializer
*/
// {85BF0F1F-D16F-4a96-B668-1B378C3FAFEE}
static const GUID EMPGPDMX_TIME_DELTA_STREAM = { 0x85bf0f1f, 0xd16f, 0x4a96, { 0xb6, 0x68, 0x1b, 0x37, 0x8c, 0x3f, 0xaf, 0xee } };

/*!
@brief This parameter indicates the current difference between the time reported by the reference
clock and the outgoing time stamps. The parameter value is specified in 100 ns units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX     </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes            </dd></dl>
@hideinitializer
*/
// {DCAB1D54-B04B-47dd-99B4-72B5A7FE1668}
static const GUID EMPGPDMX_TIME_DELTA_CLOCK = { 0xdcab1d54, 0xb04b, 0x47dd, { 0x99, 0xb4, 0x72, 0xb5, 0xa7, 0xfe, 0x16, 0x68 } };

/*!
@brief This parameter specifies the buffer size for data parsing.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4         </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_UI32_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  20971520       </dd></dl>
@hideinitializer
*/
// {B6F8F0C0-2C9E-475b-8EF3-84004864EBD1}
static const GUID EMPGPDMX_PARSE_BUFFER_SIZE = { 0xb6f8f0c0, 0x2c9e, 0x475b, { 0x8e, 0xf3, 0x84, 0x0, 0x48, 0x64, 0xeb, 0xd1 } };

/*!
@brief This parameter specifies the amount of initial data needed for the stream type detection.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4         </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_UI32_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  102400         </dd></dl>
@hideinitializer
*/
// {D30BD4FA-20BD-4ade-8416-FC193C1FCFF7}
static const GUID EMPGPDMX_INIT_BUFFER_SIZE = { 0xd30bd4fa, 0x20bd, 0x4ade, { 0x84, 0x16, 0xfc, 0x19, 0x3c, 0x1f, 0xcf, 0xf7 } };

/*!
@brief When Demultiplexer detects the data break in the input stream, it generates the discontinuity
flag on output sample. When decoders receive the discontinuity flag and decode the last
received data, they may clear their state to decode new data without reference to the
previous data. This parameter can disable the discontinuity flag generation.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Discontinuity flag is generated.
 - 1 - Discontinuity flag is not generated.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {8516CB77-00A1-4d88-966F-C2E9CE380C33}
static const GUID EMPGPDMX_DISCONTINUITY_GENERATE_DISABLE = { 0x8516cb77, 0xa1, 0x4d88, { 0x96, 0x6f, 0xc2, 0xe9, 0xce, 0x38, 0xc, 0x33 } };

/*!
@brief When Demultiplexer detects the data break in the input stream, it generates the discontinuity
flag on output sample and flushes decoders to prevent the broken data displaying. This
parameter enables or disables the data flush.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Flush is disabled.
 - 1 - Flush is enabled.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {E23CBC75-1626-482b-81F2-8C3F8C2E2897}
static const GUID EMPGPDMX_DISCONTINUITY_GENERATE_FLUSHDATA = { 0xe23cbc75, 0x1626, 0x482b, { 0x81, 0xf2, 0x8c, 0x3f, 0x8c, 0x2e, 0x28, 0x97 } };

/*!
@brief This parameter contains the stream type, when detection process is finished.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 – Unknown stream type
 - 1 – System Stream
 - 2 – Program Stream
 - 3 – Transport Stream
 - 4 – MPEG-1/2 video stream
 - 5 – PVA stream
 - 6 – MPEG-1/2 audio stream
 - 7 – AC-3 stream
 - 8 – AVC video stream
 - 9 – MPEG-4 video stream
 - 10 – VC-1 video stream.
 - 11 – H.263 video stream
 - 12 – MP4 stream
 - 13 – AAC stream
 - 14 – MPEG-1 video stream
 - 15 – MPEG-1 audio stream
 - 16 – DIV3 stream
 - 17 – DIV4 stream
 - 18 – DIV5 stream
 - 19 – VideoSoft AVC stream
 - 20 – PCM stream
 - 21 – DVD subpicture stream
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes            </dd></dl>
@hideinitializer
*/
// {59D26E5F-6C2A-4406-B9B8-19B6A1C87EE1}
static const GUID EMPGPDMX_STREAMTYPE = { 0x59d26e5f, 0x6c2a, 0x4406, { 0xb9, 0xb8, 0x19, 0xb6, 0xa1, 0xc8, 0x7e, 0xe1 } };

/*!
@brief The internal Demultiplexer buffer consists of blocks. This parameter indicates the current
number of used buffer blocks.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4         </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_UI32_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes            </dd></dl>
@hideinitializer
*/
// {B1F0359F-FE96-4bae-B2E7-663E5376F357}
static const GUID EMPGPDMX_BUFFERS_USED = { 0xb1f0359f, 0xfe96, 0x4bae, { 0xb2, 0xe7, 0x66, 0x3e, 0x53, 0x76, 0xf3, 0x57 } };

/*!
@brief The internal Demultiplexer buffer consists of blocks. This parameter indicates the total
number of allocated buffer blocks.
@details
<dl><dt>Type:           </dt>   <dd>  VT_UI4         </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_UI32_MAX  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
<dl><dt>Read only       </dt>   <dd>  yes            </dd></dl>
@hideinitializer
*/
// {33F2DD42-5497-4f86-A642-357E93677111}
static const GUID EMPGPDMX_BUFFERS_COUNT = { 0x33f2dd42, 0x5497, 0x4f86, { 0xa6, 0x42, 0x35, 0x7e, 0x93, 0x67, 0x71, 0x11 } };

/*!
@brief This parameter switches Demultiplexer to work with manually created output pins. The pin
named “PSI” is generated and receives the PSI section of transport stream. The output pins
creation via the IMpeg2Demultiplexer, IMPEG2PIDMap and IMPEG2StreamIdMap interfaces
is possible in this mode.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - PSI output mode is switched off.
 - 1 - PSI output mode is switched on.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {C51E0176-243C-4915-9FDA-BCB4D5BC2B84}
static const GUID EMPGPDMX_GENERATE_PSIOUTPUT = { 0xc51e0176, 0x243c, 0x4915, { 0x9f, 0xda, 0xbc, 0xb4, 0xd5, 0xbc, 0x2b, 0x84 } };

/*!
@brief This parameter switches on/off the simplified method to detect elementary streams
multiplexed into the transport stream. If simplified detection method is enabled, the output pin
is created only with major and minor types specified.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Complete detection method is enabled.
 - 1 - Simplified detection method is enabled.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {16B4149A-235F-4289-9209-57A8E4DE926E}
static const GUID EMPGPDMX_SIMPLE_TS_DETECT = { 0x16b4149a, 0x235f, 0x4289, { 0x92, 0x9, 0x57, 0xa8, 0xe4, 0xde, 0x92, 0x6e } };

/*!
@brief Disable destroy automatic generated pins on pin manual creation.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Disable destroy automatic generated pins.
 - 1 - Enable destroy automatic generated pins.
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {1A1E89D0-6695-47a0-9EA4-5FF4CED52B55}
static const GUID EMPGPDMX_DISABLE_DESTROY_PINS = { 0x1a1e89d0, 0x6695, 0x47a0, { 0x9e, 0xa4, 0x5f, 0xf4, 0xce, 0xd5, 0x2b, 0x55 } };

/*!
@brief Pin specific value. Use IModuleConfig of specified output pin. Set this value to skip
parsing data for stream associate with this pin to specified stream offset.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX   </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  _I64_MAX       </dd></dl>
@hideinitializer
*/
// {D8CE730A-7C4F-4ca8-ACE2-D27D977DC95E}
static const GUID EMPGPDMX_PINOUT_DATAPARSEOFFSET = { 0xd8ce730a, 0x7c4f, 0x4ca8, { 0xac, 0xe2, 0xd2, 0x7d, 0x97, 0x7d, 0xc9, 0x5e } };

/*!
@brief If this parameter is set then for TS stream types 0x06 and 0x15 automatic generate output
pin to pass all private data to it.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Disable.
 - 1 - Enable
    </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0              </dd></dl>
@hideinitializer
*/
// {294741DF-23D5-4d24-B856-24B98A109D47}
static const GUID EMPGPDMX_ENABLE_AUTODETECT_PRIVATE = { 0x294741df, 0x23d5, 0x4d24, { 0xb8, 0x56, 0x24, 0xb9, 0x8a, 0x10, 0x9d, 0x47 } };

/*!
@brief This parameter allows specifying the maximum threshold value for the PTS discontinuity
detection. The value is specified in 100 ns. units.
@details
<dl><dt>Type:           </dt>   <dd>  VT_I8          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  0-_I64_MAX   </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  25000000       </dd></dl>
@hideinitializer
*/
// {FE8522C7-E8E6-44a2-B61A-DF72ECA6DB45}
static const GUID EMPGPDMX_PTSJUMPTHRESHOLD = { 0xfe8522c7, 0xe8e6, 0x44a2, { 0xb6, 0x1a, 0xdf, 0x72, 0xec, 0xa6, 0xdb, 0x45 } };

/*!
@brief Pair of last stream time and PTS.
@details
<dl><dt>Type:           </dt>   <dd>  VT_ARRAY of VT_I8  </dd></dl>
Sample use:
@verbatim
CComVariant var;
ModuleConfig->GetValue(&EMPGPDMX_LASTSTREAMTIME, &var);
LONGLONG llValTS;
LONGLONG llValPTS;
long i(0);
hr = SafeArrayGetElement(var.parray, &i, &llValTS);
i = 1;
hr = SafeArrayGetElement(var.parray, &i, &llValPTS);
@endverbatim
@hideinitializer
*/
// {D94DEE96-AD5B-4847-961C-AC046BEC74C3}
static const GUID EMPGPDMX_LASTSTREAMTIME = { 0xd94dee96, 0xad5b, 0x4847, { 0x96, 0x1c, 0xac, 0x4, 0x6b, 0xec, 0x74, 0xc3 } };

/*!
@brief This parameter allows to manually specify stream types and ignore appropriate values taken from PMT.
@details
The replacement values are specified as an array of stream PIDs and stream types.

The array should have the following format: { stream PID, stream type, stream PID, stream type, ... }
<dl><dt>Type:     </dt>   <dd>  VT_ARRAY of VT_UI2  </dd></dl>
@hideinitializer
*/
// {B4464B71-4E86-495d-B2E0-2D4FDB7EAA13}
static const GUID EMPGPDMX_TS_TYPE_TABLE = { 0xb4464b71, 0x4e86, 0x495d, { 0xb2, 0xe0, 0x2d, 0x4f, 0xdb, 0x7e, 0xaa, 0x13 } };

/*!
@brief This parameter allows to override LiveSources in graph.
@details
When LiverSource is added into filter graph audio renderer uses specific mode. This mode can cause problems with slow resynchronization.
To avoid such issues this parameter can be used to hide LiveSource and to force audio renderer use regular mode.
<dl><dt>Type:     </dt>   <dd>  VT_I4          </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
 - 0 - Override LiveSources in graph.
 - 1 - Do not override LiveSource in graph.
</dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0       </dd></dl>
@hideinitializer
*/
// {83DEDBD7-16F0-47e8-B65A-D9ED8A1AEF73}
static const GUID EMPGPDMX_NO_LIVESRC  = { 0x83dedbd7, 0x16f0, 0x47e8, { 0xb6, 0x5a, 0xd9, 0xed, 0x8a, 0x1a, 0xef, 0x73 } };

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
// {813467FF-2384-470c-AF3E-28AD6D66329E}
static const GUID EMPGPDMX_DIRECT_PTS = { 0x813467ff, 0x2384, 0x470c, { 0xaf, 0x3e, 0x28, 0xad, 0x6d, 0x66, 0x32, 0x9e } };

/*!
@brief This parameter contains stream time information. Stream time information is represented as an array of StreamTimeInfo structures, which contain stream PID, current graph timestamp and a timestamp taken from stream.
@details 
<dl><dt>Type:     </dt>         <dd>  VT_ARRAY of VT_RECORD  </dd></dl>
<dl><dt>Read only </dt>         <dd>  yes                    </dd></dl>
@hideinitializer
*/
// {9BA4154C-45E6-475F-9B7B-B6180363CEB7}
static const GUID EMPGPDMX_STREAMTIMES = { 0x9ba4154c, 0x45e6, 0x475f, { 0x9b, 0x7b, 0xb6, 0x18, 0x3, 0x63, 0xce, 0xb7 } };

/*!
@brief This parameter enables broken PMT support.
@details 
<dl><dt>Type:     </dt>         <dd>  VT_I4   </dd></dl>
<dl><dt>Available range:</dt>   <dd>  
- 0 - Disables broken PMT support.
- 1 - Enables broken PMT support.
  </dd></dl>
<dl><dt>Default value:  </dt>   <dd>  0       </dd></dl>
@hideinitializer
*/
// {063A5458-69ED-4F79-8592-906CD6954471}
static const GUID EMPGPDMX_FRAGMENT_TS_PMT_UPDATES = { 0x63a5458, 0x69ed, 0x4f79, { 0x85, 0x92, 0x90, 0x6c, 0xd6, 0x95, 0x44, 0x71 } };


/*!
@brief Set CA decryptor
*/

// {3FF9D6F5-86DE-4864-82A8-55C359640F72}
static const GUID EMPGPDMX_CA_DECRYPTOR = { 0x3ff9d6f5, 0x86de, 0x4864,{ 0x82, 0xa8, 0x55, 0xc3, 0x59, 0x64, 0x0f, 0x72 } };



#endif //#if !defined(__PropID_DemuxerPush_h_10_7_2005_17_17)
