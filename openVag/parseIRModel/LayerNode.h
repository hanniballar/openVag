#pragma once

#include <map>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>

class LayerNode;
class LayerPort {
public:
	LayerPort(const std::string& id, std::weak_ptr<LayerNode> parent) : id(id), parent(parent) {}
	std::string id;
	std::weak_ptr<LayerNode> parent;
};

class LayerNode {
public:
	LayerNode(const std::string& layerID, const std::string& name, const std::string& type) : layerID(layerID), name(name), type(type) {}
	LayerNode(const std::string& layerID) : layerID(layerID) {}
	std::string layerID;
	std::string name;
	std::string type;
	std::vector<LayerPort> vecInputPort;
	std::vector<LayerPort> vecOutputPort;
};

class Edges {
public:
	std::map<LayerPort, LayerPort> mapEdge;
};

class IRXmlRep {
public:
	std::vector<LayerNode> vecLayerNode;
	Edges edges;
};