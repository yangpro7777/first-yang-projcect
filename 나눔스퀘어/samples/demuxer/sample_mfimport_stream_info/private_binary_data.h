//--------------------------------------------------------------------------
// File: private_binary_data.h
//
// Desc: Private binary data representation
//
// Copyright (c) 2014, MainConcept AG.  All rights reserved.
//--------------------------------------------------------------------------


#if !defined (__MC_PRIVATE_BINARY_DATA_INCLUDED__)
#define __MC_PRIVATE_BINARY_DATA_INCLUDED__

#include "mctypes.h"
#include "mfimport.h"

namespace PrivateBinaryData
{
    //!< Timecode media is used to store time code data in QuickTime movies.
    struct QTMP4Timecode
    {
        static const uint32_t tmcd_header = 0x746d6364;

        static bool Recognize(mfi_stream_info_t & stream_info);

        QTMP4Timecode(mfi_stream_info_t & stream_info);

        uint32_t hour;               //!< Number of hours in the start timecode.
        uint8_t minute;             //!< Number of minutes in the start timecode.
        uint8_t second;             //!< Number of seconds in the start timecode.
        uint8_t frame;              //!< Number of frames in the start timecode.
        uint8_t negative;           //!< If bit is set to 1, the timecode record value is negative.
        uint8_t drop_flag;          //!< Indicates whether the timecode is drop frame.
        uint8_t wraps_24h;          //!< Indicates whether the timecode wraps after 24 hours.
        uint32_t time_scale;         
        uint32_t frame_duration;
        uint32_t number_of_frames;
    };

}

#endif // #if !defined (__MC_PRIVATE_BINARY_DATA_INCLUDED__)