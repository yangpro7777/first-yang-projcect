set(MC_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
function(mc_int_find LIB_NAME DEBUG STATIC WITH_PREFIX)
    string(TOUPPER ${LIB_NAME} LIB_NAME_UPPER)
    string(TOLOWER ${LIB_NAME} LIB_NAME_LOWER)
    if(WITH_PREFIX)
        set(BASE_PREFIX ${WITH_PREFIX}_)
        set(LIB_NAMES ${WITH_PREFIX}_${LIB_NAME_LOWER})
    else()
        string(TOUPPER "demo_" BASE_PREFIX)
        set(LIB_NAMES mc_${LIB_NAME_LOWER} demo_${LIB_NAME_LOWER})
    endif()

    if(DEBUG)
        set(POSTFIX _DEBUG)
    endif()
    if(${LIB_NAME}${POSTFIX}_FOUND)
        return()
    endif()
    
    set(OLD_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    endif()

    set(MC_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

    if(NOT EXISTS "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES}")
        unset(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES CACHE)
    endif()

    find_library(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES
                    NAMES ${LIB_NAMES}
                    PATHS ${MC_ROOT}/lib
                          ${MC_ROOT}/Frameworks
                    NO_CMAKE_FIND_ROOT_PATH
                    NO_DEFAULT_PATH
    )

    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
    endif()

    if(NOT EXISTS "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_RUNTIME_LIBRARIES}")
        unset(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_RUNTIME_LIBRARIES CACHE)
    endif()

    find_library(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_RUNTIME_LIBRARIES
                    NAMES ${LIB_NAMES}
                    PATHS ${MC_ROOT}/bin
                            ${MC_ROOT}/lib
                    NO_CMAKE_FIND_ROOT_PATH
                    NO_DEFAULT_PATH
    )

    if(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES)
        if(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES MATCHES "/([^/]+)\\.framework$")
            set(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_RUNTIME_LIBRARIES "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES}/${CMAKE_MATCH_1}" CACHE STRING "${LIB_NAME} runtime library name." FORCE)
            set(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LOCATION "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_RUNTIME_LIBRARIES}")
        else()
            set(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LOCATION "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES}")
        endif()
        set(${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS ${MC_ROOT}/include 
            CACHE STRING ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS)
    endif()
    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(${LIB_NAME}${POSTFIX}
                                        REQUIRED_VARS 
                                            ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LOCATION
                                            ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS
                                            ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES
    )
    set(VAR_LIST ${LIB_NAME}${POSTFIX}_FOUND
                    ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LOCATION
                    ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS
                    ${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES)

    set(CMAKE_FIND_LIBRARY_SUFFIXES "${OLD_FIND_LIBRARY_SUFFIXES}")

    if(NOT ${LIB_NAME}${POSTFIX}_FOUND)
        message(FATAL_ERROR "Can`t find ${LIB_NAME} library.")
        return()
    endif()
    
    foreach(VAR ${VAR_LIST})
        set(${VAR} "${${VAR}}" PARENT_SCOPE)
    endforeach()
    
    if(WITH_PREFIX)
        set(LIB_FULL_NAME ${WITH_PREFIX}_${LIB_NAME_LOWER})
    else()
        set(LIB_FULL_NAME ${LIB_NAME_LOWER})
    endif()
    if(NOT STATIC)
        set(LIB_FULL_NAME_WPREFIX "${LIB_FULL_NAME}_shared")
        if(NOT TARGET ${LIB_FULL_NAME})
            add_library(${LIB_FULL_NAME} SHARED IMPORTED)
            add_library(${LIB_FULL_NAME_WPREFIX} SHARED IMPORTED)
        endif()
        set_target_properties(${LIB_FULL_NAME} ${LIB_FULL_NAME_WPREFIX} PROPERTIES
                                IMPORTED_LOCATION${POSTFIX} "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_RUNTIME_LIBRARIES}"
                                IMPORTED_IMPLIB${POSTFIX} "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LOCATION}"
        )
    else()
        set(LIB_FULL_NAME_WPREFIX "${LIB_FULL_NAME}_static")
        if(NOT TARGET ${LIB_FULL_NAME})
            add_library(${LIB_FULL_NAME} UNKNOWN IMPORTED)
            add_library(${LIB_FULL_NAME_WPREFIX} UNKNOWN IMPORTED)
        endif()
        set_target_properties(${LIB_FULL_NAME} ${LIB_FULL_NAME_WPREFIX} PROPERTIES
                                    IMPORTED_LOCATION${POSTFIX} "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LOCATION}"
        )
    endif()

    if(NOT STATIC)
        set_target_properties(${LIB_FULL_NAME} ${LIB_FULL_NAME_WPREFIX} PROPERTIES
                                    INTERFACE_INCLUDE_DIRECTORIES "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS}"
        )
    elseif(DEBUG)
        set_property(TARGET ${LIB_FULL_NAME} APPEND PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES "$<$<CONFIG:Debug>:${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS}>"
        )
        set_property(TARGET ${LIB_FULL_NAME_WPREFIX} APPEND PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES "$<$<CONFIG:Debug>:${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS}>"
        )
    else()
        set_property(TARGET ${LIB_FULL_NAME} APPEND PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS}"
        )
        set_property(TARGET ${LIB_FULL_NAME_WPREFIX} APPEND PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_INCLUDE_DIRS}"
        )
    endif()

    get_filename_component(LIB_FULL_NAME_BIN "${${BASE_PREFIX}${LIB_NAME_UPPER}${POSTFIX}_LIBRARIES}" NAME_WE)

    if("${LIB_FULL_NAME_BIN}" MATCHES "^demo")
        set_target_properties(${LIB_FULL_NAME} ${LIB_FULL_NAME_WPREFIX} PROPERTIES
                                    INTERFACE_COMPILE_DEFINITIONS "DEMO_LOGO"
        )
    endif()
endfunction()

function(mc_find_library NAME)
    set(multiValueArgs ADDITIONAL NATIVE)
    set(oneValueArgs DEBUG STATIC WITH_PREFIX)
    cmake_parse_arguments(LIBRARY "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    mc_int_find(${NAME} ${LIBRARY_DEBUG} ${LIBRARY_STATIC} "${LIBRARY_WITH_PREFIX}")
    if(LIBRARY_DEBUG)
        set(POSTFIX _DEBUG)
    endif()
    if(LIBRARY_WITH_PREFIX)
        set(LIB_NAME ${LIBRARY_PREFIX}_${NAME}${POSTFIX})
    else()
        set(LIB_NAME ${NAME}${POSTFIX})
    endif()
    if(NOT ${LIB_NAME}_FOUND)
        return()
    endif()
    
    set(LIBRARIES_LIST "")
    foreach(LIB_NAME ${LIBRARY_ADDITIONAL})
        if(LIBRARY_DEBUG)
            find_package(
                ${LIB_NAME}_DEBUG
                PATHS
                    "${CMAKE_CURRENT_LIST_DIR}/../Modules"
                NO_DEFAULT_PATH
                QUIET
            )
        endif()
        find_package(
            ${LIB_NAME}
            PATHS
                "${CMAKE_CURRENT_LIST_DIR}/../Modules"
            NO_DEFAULT_PATH
            QUIET
        )
        if(${LIB_NAME}_FOUND)
            list(APPEND LIBRARIES_LIST "${LIB_NAME}")
        else()
            find_library(${LIB_NAME}_PATH
                        NAMES mc_${LIB_NAME} demo_${LIB_NAME} ${LIB_NAME}
                        PATHS "${CMAKE_CURRENT_LIST_DIR}/../lib"
                        NO_CMAKE_FIND_ROOT_PATH
                        NO_DEFAULT_PATH
            )
            if(NOT ${LIB_NAME}_PATH)
                message(STATUS "Can`t find path to the library ${LIB_NAME}")
                list(APPEND LIBRARIES_LIST "${LIB_NAME}")
            else()
                list(APPEND LIBRARIES_LIST "${${LIB_NAME}_PATH}")
            endif()
        endif()
    endforeach()
    if(LIBRARIES_LIST)
        set(LIBRARIES_LIST ${LIBRARIES_LIST} ${LIBRARY_NATIVE})
        set_target_properties(${NAME} PROPERTIES
            INTERFACE_LINK_LIBRARIES "${LIBRARIES_LIST};${LIBRARIES_NATIVE}"
        )
    endif()

    set(${LIB_NAME} "${${BASE_PREFIX}_${NAME}}" PARENT_SCOPE)
endfunction()

set(LIBRARY_ADDITIONAL "")
set(DEPS_NATIVE "")
set(_WITH_PREFIX "")

mc_find_library(
    enc_aac
    ADDITIONAL
        ${LIBRARY_ADDITIONAL}
    NATIVE
        ${DEPS_NATIVE}
    DEBUG
        0
    STATIC
        0
    WITH_PREFIX
        "${_WITH_PREFIX}"
)
unset(_WITH_PREFIX)
unset(DEPS_NATIVE)
unset(LIBRARY_ADDITIONAL)
