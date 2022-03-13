//--------------------------------------------------------------------------
// File: private_binary_data.cpp
//
// Desc: Private binary data representation
//
// Copyright (c) 2014, MainConcept AG.  All rights reserved.
//--------------------------------------------------------------------------

#include "private_binary_data.h"

using namespace PrivateBinaryData;

bool QTMP4Timecode::Recognize(mfi_stream_info_t & stream_info)
{
    if( stream_info.format.cbExtraBytes < 8 || !stream_info.format.pbExtraData )
        return false;
    return tmcd_header == *((const uint32_t*)(stream_info.format.pbExtraData));
}

QTMP4Timecode::QTMP4Timecode(mfi_stream_info_t & stream_info)
{
	mc_private_format_t *private_format = (mc_private_format_t *)stream_info.format.pFormat;
    if( private_format )
    {
		uint8_t * pTimeMeta = private_format->reserved;
		uint8_t * pTimeCode = stream_info.format.pbExtraData;
		drop_flag = !!(pTimeMeta[0] & 0x01);
		wraps_24h = !!(pTimeMeta[0] & 0x02);

        time_scale         = *((const uint32_t*)(pTimeMeta+4));
        frame_duration     = *((const uint32_t*)(pTimeMeta+8));
        number_of_frames   = *((const uint32_t*)(pTimeMeta+12));

        /* Assume Counter flag is set to 1 in tmcd track even if it is not, because most of streams have counter timecode format. */
        if( 1 /*pTimeMeta[0] & 0x0008*/) // counter mode
        {
            uint32_t frame_counter = (pTimeCode[4] << 24) | (pTimeCode[5] << 16) | (pTimeCode[6] << 8) | (pTimeCode[7]);

            uint32_t time_sec = frame_counter / number_of_frames;
            frame = frame_counter % number_of_frames;
            hour = time_sec / 3600;
            time_sec %= 3600;
            minute = time_sec / 60;
            second = time_sec % 60;
            negative = 0;
        }
        else {
            hour = pTimeCode[4];
            negative = (pTimeCode[5] & 0x80) >> 7;
            minute = pTimeCode[5] & 0x7F;
            second = pTimeCode[6];
            frame = pTimeCode[7];
        }
    }
}
