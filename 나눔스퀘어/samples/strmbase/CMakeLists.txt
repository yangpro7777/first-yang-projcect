project(strmbase)

add_definitions(
    -DUNICODE
    -D_UNICODE
)

file(
    GLOB SOURCES
        *.cpp
)

file(
    GLOB HEADERS
        *.h
)

include_directories(.)

add_library(
    ${PROJECT_NAME}
    STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(
    ${PROJECT_NAME}

    Comctl32
    winmm
    msimg32
    strmiids
    mfuuid
)

if(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 19.0.0.0)
    target_link_libraries(
        ${PROJECT_NAME}
        legacy_stdio_definitions
    )
endif()

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

set_property(
    TARGET
        "${PROJECT_NAME}"
    PROPERTY
        FOLDER
            "common"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
              COMPILE_FLAGS /wd4996
)

set(STRMBASE_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL STRMBASE_PATH)
function(strmbase_install)
    install(
        DIRECTORY
            ${STRMBASE_PATH}/
        DESTINATION
            ${INSTALL_PACK_DIR}/samples/strmbase
        PATTERN "*.svn" EXCLUDE
    )
endfunction()
