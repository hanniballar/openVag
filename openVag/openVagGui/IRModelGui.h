#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>
#include <memory>

#include "IRXmlRep.h"

class LayerPortGuiA {
public:
	LayerPortGuiA(ax::NodeEditor::PinId pinId_gui, std::weak_ptr<LayerPort> layerPort) : pinId_gui(pinId_gui), layerPort(layerPort) {};
	ax::NodeEditor::PinId pinId_gui;
	std::weak_ptr<LayerPort> layerPort;
};

class LayerPortGui {
public:
	LayerPortGui(ax::NodeEditor::PinId pinId_gui) : pinId_gui(pinId_gui) {};
	LayerPortGui(ax::NodeEditor::PinId pinId_gui, std::weak_ptr<LayerPort> layerPort) : pinId_gui(pinId_gui), layerPort(layerPort) {};
	ax::NodeEditor::PinId pinId_gui;
	std::weak_ptr<LayerPort> layerPort;
};

class LayerNodeGui {
public:
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode) : id_gui(id_gui), layerNode(layerNode) {}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode, ImVec2 size, ImVec2 pos) : id_gui(id_gui), layerNode(layerNode), size(size), pos(pos) {}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode, std::vector<std::shared_ptr<LayerPortGui>> vecInputPort, std::vector<std::shared_ptr<LayerPortGui>> vecOutputPort) : id_gui(id_gui), layerNode(layerNode), vecInputPort(vecInputPort), vecOutputPort(vecOutputPort) {}
	ax::NodeEditor::NodeId id_gui;
	ImVec2 size;
	ImVec2 pos;
	std::vector<std::shared_ptr<LayerPortGui>> vecInputPort;
	std::vector<std::shared_ptr<LayerPortGui>> vecOutputPort;
	LayerNode layerNode;
};

class IRModelGui {
public:
	std::vector<LayerNodeGui> vecLayerNodeGui;
};
