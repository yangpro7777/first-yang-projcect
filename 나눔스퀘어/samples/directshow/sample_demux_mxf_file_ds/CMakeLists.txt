cmake_minimum_required(VERSION 3.4)
project(sample_demux_mxf_file_ds)

include(samples.cmake)
detect_arch()


add_definitions(-D_FILE_OFFSET_BITS=64)

include_directories(../../../include)
include_directories(../../../include/CLSID)
include_directories(../../../include/ModuleConfig)
add_subdirectory(../../strmbase strmbase)
include_directories(../../common_ds)
include_directories(../../common)

file(
    GLOB HEADERS
        *.h
        ../../common/*.h
)

file(
    GLOB SOURCES
        *.cpp
        ../../common/sample_common_misc.cpp
        ../../common/sample_common_args.cpp
        ../../common_ds/mcdsutils.cpp
)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

create_sample(
    ${PROJECT_NAME}
    SOURCES
        ${SOURCES}
        ${HEADERS}
    LIBS
        strmbase
        winmm
)
