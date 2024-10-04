cmake_minimum_required(VERSION 3.12)
project(openVag)

# Enable solution folders in Visual Studio and Folders in Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)

set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

cmake_policy(SET CMP0079 NEW) #Attempt to add link library "imgui" to target "ImGuiFileDialog" which is not built in this directory.

add_subdirectory(external/cmake-git-version-tracking)
add_subdirectory(openVag)