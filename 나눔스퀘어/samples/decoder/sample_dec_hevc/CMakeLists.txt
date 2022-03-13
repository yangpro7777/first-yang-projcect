cmake_minimum_required(VERSION 3.4)

include(samples.cmake)
detect_arch()

project(sample_dec_hevc)

add_definitions(-D_FILE_OFFSET_BITS=64)

find_library(
    RT_LIBRARY
       rt
)

if(RT_LIBRARY)
    set(RT_LIBRARY_NAME "${RT_LIBRARY}")
endif()

include_directories(../../common)
include_directories(.)

file(
    GLOB HEADERS
        *.h
        ../../common/*.h
)

file(
    GLOB SAMPLE_COMMON_SOURCES
        ../../common/sample_common_misc.cpp
        ../../common/sample_common_args.cpp
)

file(
    GLOB SOURCES
        *.cpp
)

file(
    GLOB SOURCES_USECASE
        usecase/*.cpp
        usecase/*.c
)

if(WIN32)
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
        -DUNICODE
        -D_UNICODE
    )
else()
    list(
        REMOVE_ITEM SOURCES_USECASE
            "${CMAKE_CURRENT_LIST_DIR}/usecase/basic_d3d9.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/usecase/render_d3d9.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/usecase/render_d3d11.cpp"
    )
endif()

list(
    REMOVE_ITEM SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/latencies.cpp"
)

find_package(
    dec_hevc
    PATHS
        ../../../Modules
    NO_DEFAULT_PATH
    REQUIRED
)

create_sample(
    ${PROJECT_NAME}
    SOURCES
        ${SOURCES}
        ${SAMPLE_COMMON_SOURCES}
        ${HEADERS}
    LIBS
        dec_hevc
        ${RT_LIBRARY_NAME}
)

mc_clone_sample_project(
    ${PROJECT_NAME}
    INSTALL_PATH
        ${INSTALL_PATH}
    SOURCES
        ${SOURCES_USECASE}
)

set_target_properties(
    ${PROJECT_NAME}_basic
    PROPERTIES
        LINKER_LANGUAGE
            CXX
)

if(WIN32)
    target_link_libraries(
        ${PROJECT_NAME}_render_d3d9
        dxva2
        d3d9
    )

    target_link_libraries(
        ${PROJECT_NAME}_render_d3d11
        d3d11
        dxgi
        D3DCompiler
    )

    file(
        GLOB SHADER
            usecase/d3d11.hlsl
    )

    add_custom_command(
            TARGET sample_dec_hevc_render_d3d11 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    ${SHADER}
                    $<TARGET_FILE_DIR:sample_dec_hevc_render_d3d11>
    )
endif()

if(NOT(ENV_NAME STREQUAL "debian_wheezy"))
    file(
        GLOB SOURCES_LATENCIES
            latency/*.cpp
            latencies.cpp
    )

    find_package(Threads REQUIRED)

    create_sample(
        ${PROJECT_NAME}_latencies
        SOURCES
            ${SOURCES_LATENCIES}
            ${SAMPLE_COMMON_SOURCES}
            ${SAMPLE_RC_FILE}
        LIBS
            dec_hevc
            ${RT_LIBRARY_NAME}
            "${CMAKE_THREAD_LIBS_INIT}"
    )
endif()

set_property(TARGET "${PROJECT_NAME}_basic" PROPERTY FOLDER "${PROJECT_NAME}")
