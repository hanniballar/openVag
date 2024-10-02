if (TARGET imgui)
    return()
endif()

set(_imgui_SourceDir ${CMAKE_SOURCE_DIR}/external/imgui)
set(_imgui_BinaryDir ${CMAKE_BINARY_DIR}/external/imgui)

message(STATUS "_imgui_SourceDir = ${_imgui_SourceDir} _imgui_BinaryDir = ${_imgui_BinaryDir}")
add_subdirectory(${_imgui_SourceDir} ${_imgui_BinaryDir})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui
    REQUIRED_VARS
        _imgui_SourceDir
)

