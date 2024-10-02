project(openVag)

set(Sources
	main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(openVag ${Sources})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

target_link_libraries(openVag PRIVATE imgui imgui_node_editor)
target_link_libraries(openVag PRIVATE d3d12.lib d3dcompiler.lib dxgi.lib)