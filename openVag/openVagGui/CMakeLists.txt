project(openVagGUI)

set(Sources
	main.cpp
	OpenVag.h
	OpenVag.cpp
	createLayerNodeGui.cpp
	createLayerNodeGui.h
	IRModelGui.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(openVagGUI ${Sources})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

target_link_libraries(openVagGUI PRIVATE imgui imgui_node_editor)
target_link_libraries(openVagGUI PRIVATE d3d12.lib d3dcompiler.lib dxgi.lib)
target_link_libraries(openVagGUI PRIVATE parseIRModel)