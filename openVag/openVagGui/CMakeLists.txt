project(openVag)

set(Sources
	main.cpp
	OpenVag.h
	OpenVag.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(${CMAKE_PROJECT_NAME} ${Sources})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui imgui_node_editor)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE d3d12.lib d3dcompiler.lib dxgi.lib)