if (TARGET tinyxml2)
    return()
endif()

set (TINYXML2_ROOT_DIR ${CMAKE_SOURCE_DIR}/external/tinyxml2)
set(_tinyxml2_SourceDir ${CMAKE_SOURCE_DIR}/external/tinyxml2)
set(_tinyxml2_BinaryDir ${CMAKE_BINARY_DIR}/external/tinyxml2)

set(_tinyxml2_Sources
    ${TINYXML2_ROOT_DIR}/tinyxml2.cpp
    ${TINYXML2_ROOT_DIR}/tinyxml2.h
)

add_library(tinyxml2 STATIC
    ${_tinyxml2_Sources}
)

target_include_directories(tinyxml2 PUBLIC
    ${TINYXML2_ROOT_DIR}
)

source_group(TREE ${TINYXML2_ROOT_DIR} FILES ${_tinyxml2_Sources})
set_property(TARGET tinyxml2 PROPERTY FOLDER "external")

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    tinyxml2
    REQUIRED_VARS
        TINYXML2_ROOT_DIR
)