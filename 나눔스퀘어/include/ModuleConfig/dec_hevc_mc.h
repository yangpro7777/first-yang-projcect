/**
\file   dec_hevc_mc.h
\brief  IModuleConfig HEVC/H.265 Decoder parameter's identifiers

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/
#if !defined(__HEVCVD_HEVCDEC_MODULECONFIG_PROPID__)
#define __HEVCVD_HEVCDEC_MODULECONFIG_PROPID__
#include <windef.h>
#include <guiddef.h>
#include <Unknwn.h>

/// namespace EHEVCVD
namespace EHEVCVD
{
    /** \brief  Synchronizing modes. */
    enum SynchronizingMode
    {
        Synchronizing_Auto = 0,      /**< \brief Decide the best way to compute PTS automatically. \hideinitializer */
        Synchronizing_Pts = 1,       /**< \brief Take PTS from the input pin.  \hideinitializer */
        Synchronizing_Vps = 3,       /**< \brief Compute PTS based on VPS and/or SPS in a bitstream. \hideinitializer */
        Synchronizing_Fixed = 5,     /**< \brief Use fixed PTS.  \hideinitializer */
        Synchronizing_Ignore_All = 6 /**< \brief Don't use PTS at all.  \hideinitializer */
    };

    /** \brief  Preview modes. */
    enum PreviewMode
    {
        PreviewMode_OFF = 0, /**< \brief Feature is disabled (default). \hideinitializer */
        PreviewMode_LV1,     /**< \brief De-blocking filter and SAO skipped on non-reference frames. \hideinitializer */
        PreviewMode_LV2,     /**< \brief De-blocking filter and SAO skipped on all frames.  \hideinitializer */
        PreviewMode_LV3,     /**< \brief De-blocking filter and SAO skipped on all frames, motion vectors precision restricted to half pel. \hideinitializer */
        PreviewMode_LV4,     /**< \brief De-blocking filter and SAO skipped on all frames, motion vectors precision restricted to full pel. \hideinitializer */
    };

    /** \brief Deinterlace mode. */
    enum DeinterlacingMode
    {
        Deinterlacing_Auto = 0,                    //!< Behaves as \ref Deinterlacing_Renderer (default). \hideinitializer
        Deinterlacing_IntrafieldInterpolation = 1, /*!< \brief Deinterlacing is done by blending or averaging consecutive fields.
          This is fine for slow motion, when the image hasn't changed between fields. \hideinitializer */
        Deinterlacing_IntrafieldInterpolationTop =
            2, //!< Deinterlacing is done by replacing samples of bottom field with samples of top field (top field stretching). \hideinitializer
        Deinterlacing_IntrafieldInterpolationBottom =
            5, //!< Deinterlacing is done by replacing samples of top field with samples of bottom field (bottom field stretching). \hideinitializer
        Deinterlacing_Renderer = 3, //!< Interpolation is done by renderer. \hideinitializer
        Deinterlacing_Weave = 4,    //!< Fields are combined to frames. No Deinterlacing is applied. \hideinitializer
        Deinterlacing_Off = 6       //!< Fields are not combined to frames. No Deinterlacing is applied. \hideinitializer
    };

    /** \brief HDR mode **/
    enum HdrMode
    {
        HdrMode_PassThrough = 0, //!< Pass-through color data without color conversion. \hideinitializer
        HdrMode_Pq2Sdr = 1,      //!< Perform HDR color conversion for PQ frame to \ref TRANSFER_CHARACTERISTICS_BT709_6 "BT.709". \hideinitializer
        HdrMode_Pq2Hlg = 2, //!< Perform HDR color conversion for PQ frame to \ref TRANSFER_CHARACTERISTICS_BT2100_HLG "BT.2100 HLG" format. \hideinitializer
    };
};

// All parameters can be set with one call through this interface
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    DWORD smp_mode;
    BYTE skip_mode;
    BYTE synchronizing;
    DOUBLE frame_rate;
    BOOL osd;
    BOOL error_concealment;
    INT overlapped_pictures;
    INT max_temporal_layer;
    BYTE preview_mode;
    DWORD hardware_acceleration_mode;
    BOOL logging;
    DWORD deinterlacing_mode;
    LONG hardware_adapter;
    BYTE hdr_mode;

    BSTR LicenseFilePath;
    BSTR LicenseBuffer;
} HEVC_config_t;

#ifdef __cplusplus
}
#endif

/**
Specifies the algorithm to compute PTS (Presentation Time Stamp) of a decoded frame. The option allows to choose timestamps provided either by demuxer or
bitstream VPS/SPS, compute timestamps from GOP structure, or use fixed values for timestamps.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Synchronizing_Auto. The decoder tries to deduce timestamps automatically by sequentially applying different algorithms from 'read PTS from a demuxer'
through 'use a fixed value'.<br>
1 - Synchronizing_Pts. The decoder reads PTS from a demuxer.<br>
3 - Synchronizing_Vps. The decoder reads PTS from bitstream VPS/SPS.<br>
5 - Synchronizing_Fixed. The decoder uses a fixed value for PTS.<br>
6 - Synchronizing_Ignore_All. The decoder doesn't use PTS.<br>
</dd></dl> \hideinitializer */ // {1E09A189-3C37-4715-A73C-BA5C8ADBF55B}
static const GUID EHEVCVD_SYNCHRONIZING = { 0x1e09a189, 0x3c37, 0x4715, { 0xa7, 0x3c, 0xba, 0x5c, 0x8a, 0xdb, 0xf5, 0x5b } };

/**
Specifies the frame rate for the decoder if "Rate interpretation" is equal to RateMode_Fixed.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_R8 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0.01 - 9999999.99
</dd></dl> \hideinitializer */ // {FA9C88BC-82B3-4377-894C-3D152D9644C1}
static const GUID EHEVCVD_FrameRate = { 0xfa9c88bc, 0x82b3, 0x4377, { 0x89, 0x4c, 0x3d, 0x15, 0x2d, 0x96, 0x44, 0xc1 } };

/**
Decoder skips or simplifies some steps in picture decoding.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - Feature is disabled (default).<br>
1 - De-blocking filter and SAO skipped on non-reference frames.<br>
2 - De-blocking filter and SAO skipped on all frames.<br>
3 - De-blocking filter and SAO skipped on all frames, motion vectors precision restricted to half pel.<br>
4 - De-blocking filter and SAO skipped on all frames, motion vectors precision restricted to full pel.<br>
</dd></dl> \hideinitializer */ // {145E2990-A046-4363-9857-7FCF608CE252}
static const GUID EH265VD_PreviewMode = { 0x145e2990, 0xa046, 0x4363, { 0x98, 0x57, 0x7f, 0xcf, 0x60, 0x8c, 0xe2, 0x52 } };

/**
Specifies the number of pictures decoded in parallel. See \ref DEC_HEVC_OVERLAPPING "Overlapping" for more details.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
range: -1 - 127<br>
-1 - Auto <br>
</dd></dl> \hideinitializer */
static const GUID EH265VD_OverlappedPictures = { 0x9CF1A330, 0xE72B, 0x4A6D, { 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x56 } };

/**
Specifies the highest number of temporal layer that should be used during decoding.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
0 - \ref TEMPORAL_LAYERS_COUNT - 1 <br>
</dd></dl> \hideinitializer */
static const GUID EH265VD_MaxTemporalLayer = { 0x9CF1A330, 0xE72B, 0x4A6D, { 0xBB, 0xE8, 0x19, 0x95, 0x95, 0x94, 0x45, 0x59 } };

/**
\ref EHEVCVD::DeinterlacingMode "Deinterlacing mode", it can be switched on the fly but there are some side effects, refer to the appropriate
\ref DEC_HEVC_INTERLACED_CONTENT_LIMITATION "section" for more details.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_UI4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
EHEVCVD::Deinterlacing_Auto - \copybrief EHEVCVD::Deinterlacing_Auto <br>
EHEVCVD::Deinterlacing_IntrafieldInterpolation - \copybrief EHEVCVD::Deinterlacing_IntrafieldInterpolation <br>
EHEVCVD::Deinterlacing_IntrafieldInterpolationTop - \copybrief EHEVCVD::Deinterlacing_IntrafieldInterpolationTop <br>
EHEVCVD::Deinterlacing_Renderer - \copybrief EHEVCVD::Deinterlacing_Renderer <br>
EHEVCVD::Deinterlacing_Weave - \copybrief EHEVCVD::Deinterlacing_Weave <br>
EHEVCVD::Deinterlacing_IntrafieldInterpolationBottom - \copybrief EHEVCVD::Deinterlacing_IntrafieldInterpolationBottom <br>
EHEVCVD::Deinterlacing_Off - \copybrief EHEVCVD::Deinterlacing_Off <br>
</dd></dl> \hideinitializer */ // {3F76078A-3FDE-4EAC-9D37-B84D5FDAB2EE}
static const GUID EH265VD_DeinterlacingMode = { 0x3f76078a, 0x3fde, 0x4eac, { 0x9d, 0x37, 0xb8, 0x4d, 0x5f, 0xda, 0xb2, 0xee } };

/**
Specifies the hardware adapter for hardware acceleration.
\details <dl><dt><b>  Type: </b></dt><dd>
VT_I4 </dd></dl>
<dl><dt><b>  Available Values: </b></dt><dd>
-1 - Auto.<br>
0+ values specifies hardware adapter.
</dd></dl> \hideinitializer */// {C24861DD-F2F6-4ED6-813E-50F6D27975C1}
static const GUID EH265VD_HardwareAdapter = { 0xc24861dd, 0xf2f6, 0x4ed6, { 0x81, 0x3e, 0x50, 0xf6, 0xd2, 0x79, 0x75, 0xc1 } };

/**
\hideinitializer
\details
Specifies the HDR conversion mode.
<dl><dt>Type</dt><dd>VT_UI4</dd></dl>
<dl>
<dt>Available Values</dt>
<dd>
EHEVCVD::HdrMode_PassThrough - \copybrief EHEVCVD::HdrMode_PassThrough <br>
EHEVCVD::HdrMode_Pq2Sdr - \copybrief EHEVCVD::HdrMode_Pq2Sdr <br>
EHEVCVD::HdrMode_Pq2Hlg - \copybrief EHEVCVD::HdrMode_Pq2Hlg <br>
</dd>
</dl>
**/
// {F0B741FD-128D-46A6-A867-D73C5555A2A8}
static const GUID EH265VD_HdrMode = { 0xf0b741fd, 0x128d, 0x46a6, { 0xa8, 0x67, 0xd7, 0x3c, 0x55, 0x55, 0xa2, 0xa8 } };

#endif //#ifndef __HEVCVD_HEVCDEC_MODULECONFIG_PROPID__
