cmake_minimum_required(VERSION 3.4)
project(sample_dec_aac)

include(samples.cmake)
detect_arch()


add_definitions(-D_FILE_OFFSET_BITS=64)

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
)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

find_package(
    dec_aac
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
        dec_aac
)
