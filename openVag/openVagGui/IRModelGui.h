#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>
#include <memory>

#include "IRXmlRep.h"

class LayerNodeGui;
class LayerPortGui;

class EdgeGui {
public:
	EdgeGui(ax::NodeEditor::LinkId linkId, std::shared_ptr<LayerPortGui> outputPort, std::shared_ptr<LayerPortGui> inputPort) : linkId(linkId), outputPort(outputPort), inputPort(inputPort) {}
	ax::NodeEditor::LinkId linkId;
	std::shared_ptr<LayerPortGui> outputPort;
	std::shared_ptr<LayerPortGui> inputPort;
};

class LayerPortGui {
public:
	LayerPortGui(ax::NodeEditor::PinId pinId_gui) : pinId_gui(pinId_gui) {};
	LayerPortGui(ax::NodeEditor::PinId pinId_gui, std::weak_ptr<LayerPort> layerPort) : pinId_gui(pinId_gui), layerPort(layerPort) {};
	ax::NodeEditor::PinId pinId_gui;
	std::weak_ptr<LayerPort> layerPort;
	std::vector<std::shared_ptr<EdgeGui>> vecEdge;
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
