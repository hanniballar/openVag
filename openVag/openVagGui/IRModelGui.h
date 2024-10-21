#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>
#include <memory>

#include "tinyxml2.h"

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
	LayerPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort) : pinId_gui(pinId_gui), xmlPort(xmlPort) {};
	ax::NodeEditor::PinId pinId_gui;
	tinyxml2::XMLElement * xmlPort;
};

class LayerInputPortGui : public LayerPortGui {
public:
	LayerInputPortGui(ax::NodeEditor::PinId pinId_gui) : LayerPortGui(pinId_gui) {};
	LayerInputPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* port) : LayerPortGui(pinId_gui, xmlPort) {};
};

class LayerOutputPortGui : public LayerPortGui {
public:
	LayerOutputPortGui(ax::NodeEditor::PinId pinId_gui) : LayerPortGui(pinId_gui) {};
	LayerOutputPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* port) : LayerPortGui(pinId_gui, xmlPort) {};
	std::vector<std::shared_ptr<EdgeGui>> vecEdgeGui;
};


class LayerNodeGui {
public:
	LayerNodeGui(ax::NodeEditor::NodeId id_gui) : id_gui(id_gui) {} //Todo remove
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, tinyxml2::XMLElement* xmlLayer) : id_gui(id_gui), xmlLayer(xmlLayer) {}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, tinyxml2::XMLElement* xmlLayer, std::vector<std::shared_ptr<LayerInputPortGui>> vecInputPort, std::vector<std::shared_ptr<LayerOutputPortGui>> vecOutputPort) :
		id_gui(id_gui), xmlLayer(xmlLayer), vecInputPort(vecInputPort), vecOutputPort(vecOutputPort) {}
	ax::NodeEditor::NodeId id_gui;
	const char* getXmlId() const { const auto id = xmlLayer->Attribute("id"); return id ? id : ""; }
	const char* getName() const { const auto name = xmlLayer->Attribute("name"); return name ? name : ""; }
	const char* getType() const { const auto type = xmlLayer->Attribute("type"); return type ? type : ""; }
 	ImVec2 getSize();
	ImVec2 getPos();
	std::vector<std::shared_ptr<LayerInputPortGui>> vecInputPort;
	std::vector<std::shared_ptr<LayerOutputPortGui>> vecOutputPort;
	tinyxml2::XMLElement* xmlLayer;
};

class IRModelGui {
public:
	std::vector<std::shared_ptr<LayerNodeGui>> vecLayerNodeGui;
};
