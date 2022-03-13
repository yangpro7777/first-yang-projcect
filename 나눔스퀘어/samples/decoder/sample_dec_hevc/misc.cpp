/******************************************************************************
 Created: 2014/12/11
 File name: dec_hevc_misc.cpp
 Purpose: miscellaneous functions used by the sample decoder

 Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.
 Unauthorized reproduction or distribution of any portion is prohibited by law.
 *******************************************************************************/

#include "misc.h"

#if defined(_WIN32)
#include <windows.h> //time
#include <fcntl.h>
#include <io.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__native_client__) || defined(__QNX__)
#include <time.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

const std::map<std::string, hevc_decoding_toolset_t>& enumerateDecodingToolsets()
{
    static std::map<std::string, hevc_decoding_toolset_t> toolsets;
    static bool initialized = false;
    if (!initialized) {
#if defined(_WIN32)
        toolsets.insert(std::pair<std::string, hevc_decoding_toolset_t>("d3d9", HEVCVD_DECODING_TOOLSET_D3D9));
        toolsets.insert(std::pair<std::string, hevc_decoding_toolset_t>("d3d11", HEVCVD_DECODING_TOOLSET_D3D11));
#endif
        initialized = true;
    }

    return toolsets;
}

// Wall clock time
uint64_t time_get_count()
{
#if defined(_WIN32)
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.QuadPart;
#elif defined(__APPLE__)
    return mach_absolute_time();
#elif defined(__linux__)
    struct timespec time = { 0 };
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec * NANO_SECONDS_IN_SECOND + time.tv_nsec;
#else
    return 0;
#endif
}

// Time frequency
uint64_t time_get_freq()
{
#if defined(_WIN32)
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    return li.QuadPart;
#elif defined(__APPLE__)
    mach_timebase_info_data_t info = { 0 };
    mach_timebase_info(&info);
    return NANO_SECONDS_IN_SECOND * info.denom / info.numer;
#elif defined(__linux__)
    return NANO_SECONDS_IN_SECOND;
#else
    return 1;
#endif
}

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif
