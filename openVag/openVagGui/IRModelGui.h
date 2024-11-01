#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>
#include <memory>

#include "tinyxml2.h"

class EdgeGui;
class LayerNodeGui;

class LayerPortGui {
public:
	LayerPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort, std::shared_ptr<LayerNodeGui> parent) : pinId_gui(pinId_gui), xmlPort(xmlPort), parent(parent) {}

	const char* getXmlId() const { const auto id = xmlPort->Attribute("id"); return id ? id : ""; }
	ax::NodeEditor::PinId pinId_gui;
	std::shared_ptr<LayerNodeGui> parent;
	tinyxml2::XMLElement* xmlPort;
};

class LayerInputPortGui : public LayerPortGui {
public:
	LayerInputPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort, std::shared_ptr<LayerNodeGui> parent) : LayerPortGui(pinId_gui, xmlPort, parent) {}
};

class LayerOutputPortGui : public LayerPortGui {
public:
	LayerOutputPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort, std::shared_ptr<LayerNodeGui> parent) : LayerPortGui(pinId_gui, xmlPort, parent) {}
	std::vector<std::shared_ptr<EdgeGui>> vecEdgeGui;
};

class EdgeGui {
public:
	EdgeGui(ax::NodeEditor::LinkId linkId, std::shared_ptr<LayerOutputPortGui> outputPort, std::shared_ptr<LayerInputPortGui> inputPort) : linkId(linkId), outputPort(outputPort), inputPort(inputPort) {}
	std::shared_ptr<EdgeGui> PreviousSibling();
	std::shared_ptr<LayerOutputPortGui> Parent();
	ax::NodeEditor::LinkId linkId;
	std::shared_ptr<LayerOutputPortGui> outputPort;
	std::shared_ptr<LayerInputPortGui> inputPort;
};

class LayerNodeGui {
public:
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
