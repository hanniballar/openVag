#pragma once

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>

#include "utils/MapValueIterator.h"

#include "tinyxml2.h"
#include "XMLNodeWrapper.h"


class Edge;
class Edges;
class Layer;
class IRModel;
class Layers;
class Network;

struct NodeIdLess
{
	bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
	{
		return lhs.Get() < rhs.Get();
	}
};

struct PinIdLess
{
	bool operator()(const ax::NodeEditor::PinId& lhs, const ax::NodeEditor::PinId& rhs) const
	{
		return lhs.Get() < rhs.Get();
	}
};

struct LinkIdLess
{
	bool operator()(const ax::NodeEditor::LinkId& lhs, const ax::NodeEditor::LinkId& rhs) const
	{
		return lhs.Get() < rhs.Get();
	}
};

struct EdgeIDLess
{
	bool operator()(const std::shared_ptr<Edge>& lhs, const std::shared_ptr<Edge>& rhs) const;
};

class Port : public std::enable_shared_from_this<Port> {
public:
	Port(ax::NodeEditor::PinId id_gui, tinyxml2::XMLElement* xmlElement, std::shared_ptr<Layer> parent) : id_gui(id_gui), parent(parent) {
		this->xmlElement = XMLNodeWrapper::make_shared(xmlElement);
	}

	const char* getXmlId() const { const auto id = xmlElement->el->ToElement()->Attribute("id"); return id ? id : ""; }
	const ax::NodeEditor::PinId& getId() const { return id_gui; }

	const std::shared_ptr<Layer>& getParent() const { return parent; }
	const void setParent(const std::shared_ptr<Layer>& _parent) { this->parent = _parent; }
	const std::shared_ptr<Layers>& getLayers() const;
	const std::shared_ptr<Network>& getNetwork() const;
	const std::shared_ptr<Edges>& getEdges() const;
	void resetParent() { parent.reset(); }
	const std::shared_ptr<XMLNodeWrapper>& getXmlElement() const { return xmlElement; }
	size_t getXmlPosition() const;

	virtual std::set<std::shared_ptr<Edge>, EdgeIDLess> getSetEdges() const = 0;
private:
	ax::NodeEditor::PinId id_gui;
	std::shared_ptr<Layer> parent;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
};

struct PortIDLess
{
	bool operator()(const std::shared_ptr<Port>& lhs, const std::shared_ptr<Port>& rhs) const {
		return PinIdLess()(lhs->getId(), rhs->getId());
	}
};

class InputPort : public Port, public std::enable_shared_from_this<InputPort> {
public:
	InputPort(ax::NodeEditor::PinId id_gui, tinyxml2::XMLElement* xmlElement, std::shared_ptr<Layer> parent) : Port(id_gui, xmlElement, parent) {}
	std::set<std::shared_ptr<Edge>, EdgeIDLess> getSetEdges() const override;
};

class OutputPort : public Port, public std::enable_shared_from_this<OutputPort> {
public:
	OutputPort(ax::NodeEditor::PinId id_gui, tinyxml2::XMLElement* xmlElement, std::shared_ptr<Layer> parent) : Port(id_gui, xmlElement, parent) {}
	std::set<std::shared_ptr<Edge>, EdgeIDLess> getSetEdges() const override;
};

class Edge : public std::enable_shared_from_this<Edge> {
public:
	Edge(ax::NodeEditor::LinkId id_gui, std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort, tinyxml2::XMLElement* edge, std::shared_ptr<Edges> parent) : id_gui(id_gui), outputPort(outputPort), inputPort(inputPort), parent(parent) {
		this->xmlElement = XMLNodeWrapper::make_shared(edge);
	}

	const ax::NodeEditor::LinkId getId() const { return id_gui; }
	const std::shared_ptr<Edges>& getParent() const { return parent; }
	void setParent(const std::shared_ptr<Edges>& _parent) { this->parent = _parent; }
	void resetParent() { parent.reset(); }
	const std::shared_ptr<XMLNodeWrapper>& getXmlElement() const { return xmlElement; }
	size_t getXmlPosition() const;

	const std::shared_ptr<OutputPort>& getOutputPort() const { return outputPort; }
	const std::shared_ptr<InputPort>& getInputPort() const { return inputPort; }

	const std::shared_ptr<Layer>& getFromLayer() const { return outputPort->getParent(); }
	const std::shared_ptr<Layer>& getToLayer() const { return inputPort->getParent(); }

private:
	ax::NodeEditor::LinkId id_gui;
	std::shared_ptr<Edges> parent;
	std::shared_ptr<OutputPort> outputPort;
	std::shared_ptr<InputPort> inputPort;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
};

class Edges : public std::enable_shared_from_this<Edges> {
public:
	Edges(tinyxml2::XMLElement* xmlElement, std::shared_ptr<Network> parent) : parent(parent) { 
		this->xmlElement = XMLNodeWrapper::make_shared(xmlElement); 
	}
	int myInt() { return 1; }

	std::shared_ptr<Edge> createEdge(ax::NodeEditor::LinkId id_gui, std::string from_layer, std::string from_port, std::string to_layer, std::string to_port, tinyxml2::XMLElement* edge);
	std::shared_ptr<Edge> insertNewEdge(ax::NodeEditor::LinkId id_gui, const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port, size_t xmlPos);
	std::shared_ptr<Edge> insertNewEdge(ax::NodeEditor::LinkId id_gui, const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port);
	void insertEdge(std::shared_ptr<Edge> edge);
	void insertEdge(std::shared_ptr<Edge> edge, size_t position);
	void deleteEdge(const std::shared_ptr<Edge>& edge);
	void removeEdge(const std::shared_ptr<Edge>& edge);
	
	const std::set<std::shared_ptr<Edge>, EdgeIDLess>& getSetEdgesToLayer(ax::NodeEditor::NodeId toLayerId) const;
	const std::set<std::shared_ptr<Edge>, EdgeIDLess>& getSetEdgesFromLayer(ax::NodeEditor::NodeId fromLayerId) const;
	const std::shared_ptr<Edge>& getEdge(std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort) const;
	const std::shared_ptr<Edge>& getEdge(ax::NodeEditor::LinkId id) const;

	std::shared_ptr<Network> getParent() { return parent; }
	void resetParent() { parent.reset(); }
	const std::shared_ptr<XMLNodeWrapper>& getXmlElement() const { return xmlElement; }
	std::shared_ptr<Layers> getLayers();

	MapValueIterator<std::map<ax::NodeEditor::LinkId, std::shared_ptr<Edge>, LinkIdLess>> begin() { return mapLinkIdToEdge.begin(); }
	MapValueIterator<std::map<ax::NodeEditor::LinkId, std::shared_ptr<Edge>, LinkIdLess>> end() { return mapLinkIdToEdge.end(); }
private:
	std::shared_ptr<Network> parent;
	std::shared_ptr<XMLNodeWrapper> xmlElement;

	std::map<ax::NodeEditor::LinkId, std::shared_ptr<Edge>, LinkIdLess> mapLinkIdToEdge;
	std::map<ax::NodeEditor::NodeId, std::set<std::shared_ptr<Edge>, EdgeIDLess>, NodeIdLess> mapFromLayerIdToSetEdge;
	std::map<ax::NodeEditor::NodeId, std::set<std::shared_ptr<Edge>, EdgeIDLess>, NodeIdLess> mapToLayerIdToSetEdge;

	tinyxml2::XMLElement* createXmlEdge(std::string from_layer, std::string from_port, std::string to_layer, std::string to_port);
};

struct LayerIDLess;

class Layer : public std::enable_shared_from_this<Layer> {
public:
	Layer(ax::NodeEditor::NodeId id_gui, tinyxml2::XMLElement* xmlLayer, const std::shared_ptr<Layers>& parent) : id_gui(id_gui), parent(parent) {
		this->xmlElement = XMLNodeWrapper::make_shared(xmlLayer);
	}

	const ax::NodeEditor::NodeId getId() const { return id_gui; }
	const std::shared_ptr<Layers>& getParent() const { return parent; }
	void setParent(const std::shared_ptr<Layers>& _parent) { this->parent = _parent; }
	void resetParent() { parent.reset(); }
	const std::shared_ptr<XMLNodeWrapper>& getXmlElement() const { return xmlElement; }
	std::shared_ptr<XMLNodeWrapper> getXmlInputElement() const;
	std::shared_ptr<XMLNodeWrapper> getXmlOutputElement() const;
	size_t getXmlPosition() const;

	std::shared_ptr<Network> getNetwork() const;
	std::shared_ptr<Edges> getEdges() const;
	
	const char* getXmlId() const { const auto id = xmlElement->el->ToElement()->Attribute("id"); return id ? id : ""; }
	const char* getName() const { const auto name = xmlElement->el->ToElement()->Attribute("name"); return name ? name : ""; }
	const char* getType() const { const auto type = xmlElement->el->ToElement()->Attribute("type"); return type ? type : ""; }

	std::set<std::shared_ptr<OutputPort>, PortIDLess> getSetOutputPort() const { return setOutputPort; }
	std::set<std::shared_ptr<InputPort>, PortIDLess > getSetInputPort() const { return setInputPort; }
	std::set<std::shared_ptr<Edge>, EdgeIDLess> getSetEdges() const;

	void insertPort(std::shared_ptr<InputPort> port);
	void insertPort(std::shared_ptr<InputPort> port, size_t position);
	void insertPort(std::shared_ptr<OutputPort> port);
	void insertPort(std::shared_ptr<OutputPort> port, size_t position);
	void removePort(const std::shared_ptr<InputPort>& port);
	void deletePort(const std::shared_ptr<InputPort>& port);
	void removePort(const std::shared_ptr<OutputPort>& port);
	void deletePort(const std::shared_ptr<OutputPort>& port);

	std::shared_ptr<InputPort> getInputPort(std::string xmlId);
	std::shared_ptr<OutputPort> getOutputPort(std::string xmlId);

	std::set<std::shared_ptr<Layer>, LayerIDLess> getInputLayers();
	std::set<std::shared_ptr<Layer>, LayerIDLess> getOutputLayers();
private:
	ax::NodeEditor::NodeId id_gui;
	std::shared_ptr<Layers> parent;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
	
	std::set<std::shared_ptr<InputPort>, PortIDLess> setInputPort;
	std::set<std::shared_ptr<OutputPort>, PortIDLess> setOutputPort;
	
};

struct LayerIDLess
{
	bool operator()(const std::shared_ptr<Layer>& lhs, const std::shared_ptr<Layer>& rhs) const {
		return NodeIdLess()(lhs->getId(), rhs->getId());
	}
};

class Layers : public std::enable_shared_from_this<Layers> {
public:
	Layers(tinyxml2::XMLElement* xmlElement, std::shared_ptr<Network> parent) : parent(parent) { this->xmlElement = XMLNodeWrapper::make_shared(xmlElement); }
	const std::shared_ptr<Network>& getParent() const { return parent; }
	void resetParent() { parent.reset(); }

	std::shared_ptr<Layer> insertNewLayer();
	std::shared_ptr<Layer> insertNewLayer(ax::NodeEditor::NodeId id, std::string xmlId, std::string name, std::string type);
	void deleteLayer(const std::shared_ptr<Layer>& layer);
	void removeLayer(const std::shared_ptr<Layer>& layer);

	void insertLayer(std::shared_ptr<Layer> layer);
	void insertLayer(std::shared_ptr<Layer> layer, size_t position);
	void insertPort(std::shared_ptr<InputPort> port);
	void removePort(std::shared_ptr<InputPort> port);
	void insertPort(std::shared_ptr<OutputPort> port);
	void removePort(std::shared_ptr<OutputPort> port);

	const std::shared_ptr<Layer>& getLayer(ax::NodeEditor::NodeId id) const;
	const std::shared_ptr<Layer>& getLayer(std::string id) const;
	
	const std::shared_ptr<InputPort>& getInputPort(ax::NodeEditor::PinId id) const;
	const std::shared_ptr<OutputPort>& getOutputPort(ax::NodeEditor::PinId id) const;

	MapValueIterator<std::map<ax::NodeEditor::NodeId, std::shared_ptr<Layer>, NodeIdLess>> begin() { return mapNodeIdToLayer.begin(); }
	MapValueIterator<std::map<ax::NodeEditor::NodeId, std::shared_ptr<Layer>, NodeIdLess>> end() { return mapNodeIdToLayer.end(); }

	const std::shared_ptr<XMLNodeWrapper>& getXmlElement() const { return xmlElement; }

private:
	const std::set<std::shared_ptr<Layer>, LayerIDLess>& getSetLayer(std::string id) const;
	std::shared_ptr<Network> parent;
	std::shared_ptr<XMLNodeWrapper> xmlElement;

	std::map<ax::NodeEditor::NodeId, std::shared_ptr<Layer>, NodeIdLess> mapNodeIdToLayer;
	std::map<std::string, std::set<std::shared_ptr<Layer>, LayerIDLess>> mapXMLIdToSetLayer;
	std::map<ax::NodeEditor::PinId, std::shared_ptr<InputPort>, PinIdLess> mapPinIdToInputPort;
	std::map<ax::NodeEditor::PinId, std::shared_ptr<OutputPort>, PinIdLess> mapPinIdToOutputPort;

	tinyxml2::XMLElement* createXmlLayer(std::string id, std::string name, std::string type) const;
	int64_t getMaxLayerXmlId() const;
};

class Network : public std::enable_shared_from_this<Network> {
public:
	Network(tinyxml2::XMLElement* xmlElement, std::shared_ptr<IRModel> parent) : parent(parent) { this->xmlElement = XMLNodeWrapper::make_shared(xmlElement); }
	const std::shared_ptr<IRModel>& getParent() const { return parent; }
	void resetParent() { parent.reset(); }

	void setLayers(std::shared_ptr<Layers> layers) { this->layers = layers; }
	const std::shared_ptr<Layers>& getLayers() const { return layers; }

	void setEdges(std::shared_ptr<Edges> edges) { this->edges = edges; }
	const std::shared_ptr<Edges>& getEdges() const { return edges; }

	const std::shared_ptr<XMLNodeWrapper>& getXmlElement() const { return xmlElement; }
private:
	std::shared_ptr<IRModel> parent;
	std::shared_ptr<XMLNodeWrapper> xmlElement;

	std::shared_ptr<Layers> layers;
	std::shared_ptr<Edges> edges;
};

class IRModel : public std::enable_shared_from_this<IRModel> {
public:
	IRModel(std::shared_ptr<tinyxml2::XMLDocument> xmlDocument) : xmlDocument(xmlDocument) {}
	void setNetwork(std::shared_ptr<Network> network) { this->network = network; }
	const std::shared_ptr<Network>& getNetwork() const { return network; }
	const std::shared_ptr<tinyxml2::XMLDocument>& getXMLDocument() const { return xmlDocument; }

	tinyxml2::XMLError saveToFile(const char* filePath) const { return getXMLDocument()->SaveFile(filePath); };
private:
	std::shared_ptr<Network> network;
	std::shared_ptr<tinyxml2::XMLDocument> xmlDocument;
};
