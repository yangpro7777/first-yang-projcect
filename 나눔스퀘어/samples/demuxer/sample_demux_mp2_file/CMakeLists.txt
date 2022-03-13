cmake_minimum_required(VERSION 3.4)

project(sample_demux_mp2_file)

include(samples.cmake)

detect_arch()

add_definitions(-D_FILE_OFFSET_BITS=64)

include_directories(../../bufstream)
include_directories(../../common)
include_directories(.)

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
        ../../bufstream/meta_file.c
        ../../bufstream/buf_file.c
        ../../bufstream/buf_wave_write.c
)

if(WIN32)
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
    )
endif()

find_package(
    demux_mp2
    PATHS
        ../../../Modules
    NO_DEFAULT_PATH
    REQUIRED
)

create_sample(
    ${PROJECT_NAME}
    SOURCES
        ${SOURCES}
        ${HEADERS}
    LIBS
        demux_mp2
)
