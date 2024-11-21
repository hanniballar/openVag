project(openVagGUI)

find_package(tinyxml2 REQUIRED)

set(Sources
	main.cpp
	OpenVag.h
	OpenVag.cpp
	beginCreate.cpp
	beginCreate.h
	drawLayerNodes.cpp
	drawLayerNodes.h
	IRModel.h
	IRModel.cpp
	drawEdges.cpp
	drawEdges.h
	parseIRModel.cpp
	parseIRModel.h
	XMLNodeWrapper.cpp
	XMLNodeWrapper.h
	GraphLayout.cpp
	GraphLayout.h
	commands/CommandCenter.cpp
	commands/CommandCenter.h
	commands/ICommand.cpp
	commands/ICommand.h
	commands/RemoveXMLElement.cpp
	commands/RemoveXMLElement.h
	commands/ModifyAttributeXMLElement.cpp
	commands/ModifyAttributeXMLElement.h
	commands/AddAttributeXMLElement.cpp
	commands/AddAttributeXMLElement.h
	commands/DeleteAttributeXMLElement.cpp
	commands/DeleteAttributeXMLElement.h
	commands/AddEdge.cpp
	commands/AddEdge.h
	commands/AddLayer.cpp
	commands/AddLayer.h
	commands/AddXMLLayer.cpp
	commands/AddXMLLayer.h
	commands/DeleteLayer.cpp
	commands/DeleteLayer.h
	misc/debugger/openVag.natstepfilter
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(openVagGUI ${Sources})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

target_link_libraries(openVagGUI PRIVATE imgui imgui_node_editor)
target_link_libraries(openVagGUI PRIVATE d3d12.lib d3dcompiler.lib dxgi.lib)
target_link_libraries(openVagGUI PRIVATE tinyxml2)