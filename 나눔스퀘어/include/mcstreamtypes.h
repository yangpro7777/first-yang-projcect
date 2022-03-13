/**
 * @file	mcstreamtypes.h
 * @brief stream types reported by mpegin/mfimport
 *
 * @verbatim
 * ----------------------------------------------------------------------------
 * File: mcstreamtypes.h
 *
 * Desc: stream types reported by mpegin/mfimport
 *
 * Copyright (c) 2021 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 *
 * ----------------------------------------------------------------------------
 * @endverbatim
 **/
 
#if !defined (__MC_STREAMTYPE_INCLUDED__)
#define __MC_STREAMTYPE_INCLUDED__


// stream types reported by mpegin
#define STREAM_ELEMENTARY      0       // audio or video elementary stream
#define STREAM_MPEG1_PROGRAM   1       // mpeg-1 program stream
#define STREAM_MPEG2_PROGRAM   2       // mpeg-2 program stream
#define STREAM_MPEG2_TRANSPORT 3       // mpeg-2 transport stream
#define STREAM_PVA             4       // pva stream
#define STREAM_MXF_XDCAM_HD    5       // XDCAM HD MXF stream
#define STREAM_MXF_XDCAM_IMX   6       // XDCAM IMX MXF stream

#define STREAM_MP4             7       // MP4 stream
#define STREAM_DMF             8       // DMF stream
#define STREAM_MXF_XDCAM_DV    9
#define STREAM_MXF_GENERIC     10
#define STREAM_MXF_IKEGAMI     11
#define STREAM_MXF_DV          16      // deprecaited
#define STREAM_MXF_JPEG2000    17      // deprecaited
#define STREAM_MXF_P2_DVCPRO   18
#define STREAM_MXF_P2_AVCI     19
#define STREAM_ASF             20      // ASF stream
#define STREAM_MXF_XDCAM_PROXY 21      // XDCAM proxy stream
#define STREAM_QT              22      // QT stream
#endif
