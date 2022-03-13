set(MC_CMAKE_MODULES_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL MC_CMAKE_MODULES_PATH)

if(NOT COMMAND sign_binary)
    function(sign_binary TARGET_NAME)
    endfunction()
endif()

function(apply_rpath_patch PROJECT_NAME)
    if(APPLE AND NOT IOS)
        add_custom_command(TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND bash -c "otool -vL \"$<TARGET_FILE:${PROJECT_NAME}>\" | grep -Eo \"@loader_path/../../../../Frameworks/([a-zA-Z0-9_\/\.]+)\" | sed -E 's/^.{36}//g' | xargs -n1 -I{} install_name_tool -change \"@loader_path/../../../../Frameworks/{}\" \"@rpath/{}\" \"$<TARGET_FILE:${PROJECT_NAME}>\""
            VERBATIM
        )
    endif()
endfunction()

macro(mc_clone_sample_project BASE_SAMPLE)

    set(oneValueArgs INSTALL_PATH RC_FILE)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(SAMPLE "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    if(NOT SAMPLE_RC_FILE)
        file(GLOB SAMPLE_RC_FILE ${BASE_SAMPLE}.rc)
    endif()

    set_property(TARGET "${BASE_SAMPLE}" PROPERTY FOLDER "${BASE_SAMPLE}")

    get_property(ICON_FILE TARGET ${BASE_SAMPLE} PROPERTY MACOSX_BUNDLE_ICON_FILE)
    get_property(BUNDLE_COPYRIGHT TARGET ${BASE_SAMPLE} PROPERTY MACOSX_BUNDLE_COPYRIGHT)
    get_property(BUNDLE_SHORT_VERSION_STRING TARGET ${BASE_SAMPLE} PROPERTY MACOSX_BUNDLE_SHORT_VERSION_STRING)
    get_property(BUNDLE_LONG_VERSION_STRING TARGET ${BASE_SAMPLE} PROPERTY MACOSX_BUNDLE_LONG_VERSION_STRING)
    get_property(BUNDLE_VERSION TARGET ${BASE_SAMPLE} PROPERTY MACOSX_BUNDLE_BUNDLE_VERSION)
    get_property(LIST_LIBRARIES TARGET ${BASE_SAMPLE} PROPERTY LINK_LIBRARIES)
    get_property(LIBRARY_PATH TARGET ${BASE_SAMPLE} PROPERTY INSTALL_RPATH)
    get_property(BASE_SOURCES TARGET ${BASE_SAMPLE} PROPERTY SOURCES)

    foreach(FILE ${BASE_SOURCES})
        if(FILE MATCHES ".*icns")
            set(SAMPLE_FILE_LOGO "${FILE}")
        endif()
    endforeach()

    foreach(FILE ${SAMPLE_SOURCES})
        get_filename_component(FILE_NAME "${FILE}" NAME_WE)
        set(SAMPLE_NAME "${BASE_SAMPLE}_${FILE_NAME}")

        add_executable(
            ${SAMPLE_NAME}
            MACOSX_BUNDLE
            ${FILE}
            ${SAMPLE_FILE_LOGO}
            ${SAMPLE_RC_FILE}
        )

        target_link_libraries(${SAMPLE_NAME} ${LIST_LIBRARIES})

        if(MSVC)
            set_property(TARGET "${SAMPLE_NAME}" PROPERTY FOLDER "${BASE_SAMPLE}")
        endif()

        target_compile_definitions(${SAMPLE_NAME}
                PRIVATE MC_VERSION_FILENAME="${SAMPLE_NAME}.exe"
                MC_VERSION_INTERNALNAME="${SAMPLE_NAME}"
                )
        set_target_properties(${SAMPLE_NAME} PROPERTIES
                                MACOSX_BUNDLE_ICON_FILE "${ICON_FILE}"
                                MACOSX_BUNDLE_COPYRIGHT "${BUNDLE_COPYRIGHT}"
                                MACOSX_BUNDLE_SHORT_VERSION_STRING "${BUNDLE_SHORT_VERSION_STRING}"
                                MACOSX_BUNDLE_LONG_VERSION_STRING "${BUNDLE_LONG_VERSION_STRING}"
                                MACOSX_BUNDLE_BUNDLE_VERSION "${BUNDLE_VERSION}"
                                MACOSX_BUNDLE_BUNDLE_NAME "${SAMPLE_NAME}"
                                INSTALL_RPATH "${LIBRARY_PATH}"
        )
        apply_rpath_patch("${SAMPLE_NAME}")

        sign_binary(${SAMPLE_NAME})

        if(SAMPLE_INSTALL_PATH)
            install(TARGETS ${SAMPLE_NAME}
                        RUNTIME DESTINATION ${SAMPLE_INSTALL_PATH}
                        BUNDLE DESTINATION ${SAMPLE_INSTALL_PATH}
                   )
        endif()

    endforeach()
endmacro()

macro(detect_arch)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(BUILD_OUTPUT_ROOT ${CMAKE_BINARY_DIR}/build)
    set(CMAKE_CONFIGURATION_TYPES "Debug" "Release" CACHE STRING "" FORCE)

    if(NOT DETECTED_ARCH)
        if(NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
            set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug")
        endif()

        if(APPLE)
            set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING CMAKE_SHARED_LINKER_FLAGS FORCE)
            set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING CMAKE_EXE_LINKER_FLAGS FORCE)
        else()
            set(CMAKE_SHARED_LINKER_FLAGS "/machine:x64 /nologo" CACHE STRING CMAKE_SHARED_LINKER_FLAGS FORCE)
            set(CMAKE_EXE_LINKER_FLAGS " /machine:x64 /LARGEADDRESSAWARE /nologo" CACHE STRING CMAKE_EXE_LINKER_FLAGS FORCE)
        endif()

        set(CMAKE_CXX_FLAGS "/Zi /Zc:wchar_t /DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP /FS /nologo  ${PLATFORM}" CACHE STRING CMAKE_CXX_FLAGS FORCE)
        set(CMAKE_C_FLAGS "/Zi /Zc:wchar_t /DWIN32 /D_WINDOWS /W3 /EHsc /MP /FS /nologo  ${PLATFORM}" CACHE STRING CMAKE_C_FLAGS FORCE)

        set(CMAKE_C_FLAGS_DEBUG "/MTd /Od" CACHE STRING CMAKE_C_FLAGS_DEBUG FORCE)
        set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Od" CACHE STRING CMAKE_CXX_FLAGS_DEBUG FORCE)

        set(CMAKE_C_FLAGS_RELEASE "/MT /GS- /c -O2 -Oi -Ot -Oy /Zi /DNDEBUG" CACHE STRING CMAKE_C_FLAGS_RELEASE FORCE)
        set(CMAKE_CXX_FLAGS_RELEASE "/MT /GS- -O2 -Oi -Ot -Oy /Zi /DNDEBUG" CACHE STRING CMAKE_CXX_FLAGS_RELEASE FORCE)

        set(DETECTED_ARCH ON CACHE INTERNAL DETECTED_ARCH)
    endif()
    foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${TYPE} BUILD_TYPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE} ${BUILD_OUTPUT_ROOT}/bin)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILD_TYPE} ${BUILD_OUTPUT_ROOT}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILD_TYPE} ${BUILD_OUTPUT_ROOT}/lib)
        if(WIN32) # it only affects builds for windows
            set(CMAKE_PDB_OUTPUT_DIRECTORY_${BUILD_TYPE} ${BUILD_OUTPUT_ROOT}/pdb)
        endif()
    endforeach()
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    if(NOT ("OFF" STREQUAL ""))
        set(CMAKE_CXX_EXTENSIONS OFF)
    endif()

    # While CentOS 7 is still our officially supported Linux platform, ensure the
    # samples are buildable with that system's default compiler (gcc 4.8.5)
    set(CMAKE_CXX_STANDARD 11)

    if("")
        set(CMAKE_OSX_DEPLOYMENT_TARGET "")
    endif()
endmacro()

macro(add_lib PROJECT_NAME BASE_LIB)
    if(WIN32)
        get_property(LIB_PATH TARGET ${BASE_LIB} PROPERTY IMPORTED_LOCATION)
        if(LIB_PATH)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                                     COMMAND ${CMAKE_COMMAND} -E copy
                                     "${LIB_PATH}"
                                     $<TARGET_FILE_DIR:${PROJECT_NAME}>
            )
        endif()
    endif()
endmacro()


function(mc_fixup_bundle PROJECT_NAME)
    get_property(LIST_LIBRARIES TARGET ${PROJECT_NAME} PROPERTY LINK_LIBRARIES)

    foreach(LIB ${LIST_LIBRARIES})
        if(TARGET ${LIB})
            set(DIRS "${DIRS}\;$<TARGET_FILE_DIR:${LIB}>")
        endif()
    endforeach()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} 
        ARGS -DAPP=$<TARGET_FILE:${PROJECT_NAME}> -DDIRS=\"${DIRS}\" -P "${MC_CMAKE_MODULES_PATH}/mc_fixup_bundle.cmake"
        VERBATIM
    )
endfunction()

macro(create_sample PROJECT_NAME)
    set(oneValueArgs FILE_LOGO RC_FILE)
    set(multiValueArgs SOURCES LIBS)
    cmake_parse_arguments(SAMPLE "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    if(NOT SAMPLE_RC_FILE)
        file(GLOB SAMPLE_RC_FILE ${PROJECT_NAME}.rc)
    endif()
    if(NOT SAMPLE_FILE_LOGO)
        file(GLOB SAMPLE_FILE_LOGO ../../common/mcui.icns)
    endif()

    add_executable(${PROJECT_NAME}
        MACOSX_BUNDLE
        ${SAMPLE_SOURCES}
        ${SAMPLE_FILE_LOGO}
        ${SAMPLE_RC_FILE}
    )

    target_compile_definitions(${PROJECT_NAME}
        PRIVATE MC_VERSION_FILENAME="${PROJECT_NAME}.exe"
        MC_VERSION_INTERNALNAME="${PROJECT_NAME}"
    )

    target_link_libraries(${PROJECT_NAME} ${SAMPLE_LIBS})

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY
            VS_STARTUP_PROJECT ${PROJECT_NAME}
    )

    if(UNIX)
        list(GET SAMPLE_LIBS 0 BASE_LIB)
        get_property(BASE_LIB TARGET ${BASE_LIB} PROPERTY IMPORTED_LOCATION)
        file(GLOB LIBRARY_PATH RELATIVE ${CMAKE_BINARY_DIR} ${BASE_LIB})
        get_filename_component(LIBRARY_PATH ${LIBRARY_PATH} DIRECTORY)
        get_filename_component(LIBRARY_PATH_ABS ${BASE_LIB} DIRECTORY)
        if(APPLE)
            set(LIBRARY_PATH "@executable_path/../../../${LIBRARY_PATH}/..")
            set(LIBRARY_PATH_ABS "${LIBRARY_PATH_ABS}/..")

            set_target_properties(${PROJECT_NAME} PROPERTIES
                                MACOSX_BUNDLE_ICON_FILE mcui.icns
                                MACOSX_BUNDLE_COPYRIGHT "MainConcept GmbH"
                                MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
                              )
            set_source_files_properties("${FILE_LOGO}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        endif()
        set(LIBRARY_PATH "${LIBRARY_PATH};${LIBRARY_PATH_ABS};.")
        set_target_properties(${PROJECT_NAME} PROPERTIES
                                 INSTALL_RPATH "${LIBRARY_PATH}"
                             )
        apply_rpath_patch("${PROJECT_NAME}")
    else()
        mc_fixup_bundle(${PROJECT_NAME})
    endif()
endmacro()
