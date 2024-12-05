project(openVagGUI)

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
	commands/CommandCenter.cpp
	commands/CommandCenter.h
	commands/ComposedCommand.cpp
	commands/ComposedCommand.h
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
	commands/InsertEdge.cpp
	commands/InsertEdge.h
	commands/InsertLayer.cpp
	commands/InsertLayer.h
	commands/AddXMLLayer.cpp
	commands/AddXMLLayer.h
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
	utils/MapValueIterator.h
	misc/debugger/openVag.natstepfilter
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(openVagGUI ${Sources})

find_package(tinyxml2 REQUIRED)
find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(ImGuiFileDialog REQUIRED)

target_link_libraries(openVagGUI PRIVATE imgui imgui_node_editor)
target_link_libraries(openVagGUI PRIVATE d3d12.lib d3dcompiler.lib dxgi.lib)
target_link_libraries(openVagGUI PRIVATE tinyxml2)
target_link_libraries(openVagGUI PRIVATE ImGuiFileDialog)
