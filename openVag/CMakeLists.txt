project(openVag)

set(Sources
	main.cpp
	OpenVag.h
	OpenVag.cpp
	IRModel.h
	IRModel.cpp
	parseIRModel.cpp
	parseIRModel.h
	XMLNodeWrapper.cpp
	XMLNodeWrapper.h
	GraphLayout.cpp
	GraphLayout.h
	openVag_default_ini.h
	commands/CommandCenter.cpp
	commands/CommandCenter.h
	commands/ComposedCommand.cpp
	commands/ComposedCommand.h
	commands/ICommand.cpp
	commands/ICommand.h
	commands/InsertEdge.cpp
	commands/InsertEdge.h
	commands/InsertLayer.cpp
	commands/InsertLayer.h
	commands/DeleteLayer.cpp
	commands/DeleteLayer.h
	commands/DeleteInputPort.cpp
	commands/DeleteInputPort.h
	commands/DeleteOutputPort.cpp
	commands/DeleteOutputPort.h
	commands/DeleteEdge.cpp
	commands/DeleteEdge.h
	commands/InsertInputPort.cpp
	commands/InsertInputPort.h
	commands/InsertOutputPort.cpp
	commands/InsertOutputPort.h
	commands/ModifyEdge.cpp
	commands/ModifyEdge.h
	commands/SetLayerAttributes.cpp
	commands/SetLayerAttributes.h
	Canvas/showCanvas.cpp
	Canvas/showCanvas.h
	Canvas/beginCreate.cpp
	Canvas/beginCreate.h
	Canvas/contextMenu.cpp
	Canvas/contextMenu.h
	Canvas/drawEdges.cpp
	Canvas/drawEdges.h
	Canvas/drawLayerNodes.cpp
	Canvas/drawLayerNodes.h
	Find/showFind.cpp
	Find/showFind.h
	Properties/showProperties.cpp
	Properties/showProperties.h
	Properties/fillEdgeProperties.cpp
	Properties/fillEdgeProperties.h
	Properties/fillLayerProperties.cpp
	Properties/fillLayerProperties.h
	Resources/openVag.rc
	utils/MapValueIterator.h
	misc/debugger/openVag.natstepfilter
	Resources/openVag.rc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(openVag WIN32 ${Sources})

find_package(tinyxml2 REQUIRED)
find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(ImGuiFileDialog REQUIRED)

target_link_libraries(openVag PRIVATE imgui imgui_node_editor)
target_link_libraries(openVag PRIVATE d3d12.lib d3dcompiler.lib dxgi.lib)
target_link_libraries(openVag PRIVATE tinyxml2)
target_link_libraries(openVag PRIVATE ImGuiFileDialog)
target_link_libraries(openVag PRIVATE cmake_git_version_tracking)

install(TARGETS openVag DESTINATION ${CMAKE_INSTALL_PREFIX})