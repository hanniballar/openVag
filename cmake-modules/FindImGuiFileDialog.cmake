if (TARGET ImGuiFileDialog)
    return()
endif()

set(_ImGuiFileDialog_SourceDir ${CMAKE_SOURCE_DIR}/external/ImGuiFileDialog)
set(_ImGuiFileDialog_BinaryDir ${CMAKE_BINARY_DIR}/external/ImGuiFileDialog)

add_subdirectory(${_ImGuiFileDialog_SourceDir} ${_ImGuiFileDialog_BinaryDir})

target_link_libraries(ImGuiFileDialog PUBLIC imgui)

set_property(TARGET ImGuiFileDialog PROPERTY FOLDER "external")
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)


find_package_handle_standard_args(
    ImGuiFileDialog
    REQUIRED_VARS
        _ImGuiFileDialog_SourceDir
)

