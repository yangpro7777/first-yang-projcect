cmake_minimum_required(VERSION 3.4)
project(sample_demux_mp2_file_ds)

include(samples.cmake)
detect_arch()

add_definitions(-D_FILE_OFFSET_BITS=64)

file(
    GLOB SOURCES
        *.cpp
	../../common/sample_common_args.cpp
	../../common/sample_common_misc.cpp
	../../common_ds/mcdsutils.cpp
)

include_directories(
	../../common_ds
	../../common
	../../strmbase
	../../../include/CLSID
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

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
		strmbase
)
