#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>
#include <memory>

#include "tinyxml2.h"
#include "XMLNodeWrapper.h"

class EdgeGui;
class LayerNodeGui;
class IRModelGui;

class LayerPortGui {
public:
	LayerPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort, std::shared_ptr<LayerNodeGui> parent) : pinId_gui(pinId_gui), parent(parent) {
		this->xmlPort = XMLNodeWrapper::make_shared(xmlPort);
	}

	const char* getXmlId() const { const auto id = xmlPort->el->ToElement()->Attribute("id"); return id ? id : ""; }
	ax::NodeEditor::PinId pinId_gui;
	std::shared_ptr<LayerNodeGui> Parent() { return parent; };
	std::shared_ptr<LayerNodeGui> parent;
	std::shared_ptr<XMLNodeWrapper> xmlPort;
};

class LayerInputPortGui : public LayerPortGui {
public:
	LayerInputPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort, std::shared_ptr<LayerNodeGui> parent) : LayerPortGui(pinId_gui, xmlPort, parent) {}
};

class LayerOutputPortGui : public LayerPortGui {
public:
	LayerOutputPortGui(ax::NodeEditor::PinId pinId_gui, tinyxml2::XMLElement* xmlPort, std::shared_ptr<LayerNodeGui> parent) : LayerPortGui(pinId_gui, xmlPort, parent) {}
	void insert(std::ptrdiff_t pos, std::shared_ptr<EdgeGui> addThis);
	void deleteChild(std::shared_ptr<EdgeGui> child);
	std::vector<std::shared_ptr<EdgeGui>> vecEdgeGui;
};

class EdgeGui {
public:
	EdgeGui(ax::NodeEditor::LinkId linkId, std::shared_ptr<LayerOutputPortGui> outputPort, std::shared_ptr<LayerInputPortGui> inputPort, tinyxml2::XMLElement* edge) : linkId(linkId), outputPort(outputPort), inputPort(inputPort) {
		this->xmlElement = XMLNodeWrapper::make_shared(edge);
	}
	std::ptrdiff_t getMyPositionAsChild();
	std::shared_ptr<LayerOutputPortGui> Parent();
	ax::NodeEditor::LinkId linkId;
	std::shared_ptr<LayerOutputPortGui> outputPort;
	std::shared_ptr<LayerInputPortGui> inputPort;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
};

class LayerNodeGui {
public:
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, tinyxml2::XMLElement* xmlLayer, std::shared_ptr<IRModelGui> parent) : id_gui(id_gui), parent(parent) {
		this->xmlLayer = XMLNodeWrapper::make_shared(xmlLayer);
	}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, tinyxml2::XMLElement* xmlLayer, std::vector<std::shared_ptr<LayerInputPortGui>> vecInputPort, std::vector<std::shared_ptr<LayerOutputPortGui>> vecOutputPort, std::shared_ptr<IRModelGui> parent) :
		id_gui(id_gui), vecInputPort(vecInputPort), vecOutputPort(vecOutputPort), parent(parent) {
		this->xmlLayer = XMLNodeWrapper::make_shared(xmlLayer);
	}
	ax::NodeEditor::NodeId id_gui;
	const char* getXmlId() const { const auto id = xmlLayer->el->ToElement()->Attribute("id"); return id ? id : ""; }
	const char* getName() const { const auto name = xmlLayer->el->ToElement()->Attribute("name"); return name ? name : ""; }
	const char* getType() const { const auto type = xmlLayer->el->ToElement()->Attribute("type"); return type ? type : ""; }
 	ImVec2 getSize();
	ImVec2 getPos();
	std::ptrdiff_t getMyPositionAsChild();
	std::shared_ptr<IRModelGui> Parent() { return parent; }
	std::vector<std::shared_ptr<LayerInputPortGui>> vecInputPort;
	std::vector<std::shared_ptr<LayerOutputPortGui>> vecOutputPort;
	std::shared_ptr<XMLNodeWrapper> xmlLayer;
private:
	std::shared_ptr<IRModelGui> parent;
};

class IRModelGui {
public:
	IRModelGui(tinyxml2::XMLElement* xmlElement) {
		this->xmlElement = XMLNodeWrapper::make_shared(xmlElement);
	}
	void deleteChild(std::shared_ptr<LayerNodeGui> child);
	void insert(std::ptrdiff_t pos, std::shared_ptr<LayerNodeGui> addThis);
	std::vector<std::shared_ptr<LayerNodeGui>> vecLayerNodeGui;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
};
