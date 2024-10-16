#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>

#include "IRXmlRep.h"

class LayerPortGui {
public:
	LayerPortGui(ax::NodeEditor::PinId pinId_gui, LayerPort* layerPort) : pinId_gui(pinId_gui), layerPort(layerPort) {};
	ax::NodeEditor::PinId pinId_gui;
	LayerPort *layerPort;
};

class LayerNodeGui {
public:
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode) : id_gui(id_gui), layerNode(layerNode) {}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode, ImVec2 size, ImVec2 pos) : id_gui(id_gui), layerNode(layerNode), size(size), pos(pos) {}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode, std::vector<LayerPortGui> vecInputPort, std::vector<LayerPortGui> vecOutputPort) : id_gui(id_gui), layerNode(layerNode), vecInputPort(vecInputPort), vecOutputPort(vecOutputPort) {}
	ax::NodeEditor::NodeId id_gui;
	ImVec2 size;
	ImVec2 pos;
	std::vector<LayerPortGui> vecInputPort;
	std::vector<LayerPortGui> vecOutputPort;
	LayerNode layerNode;
};

class IRModelGui {
public:
	std::vector<LayerNodeGui> vecLayerNodeGui;
};
