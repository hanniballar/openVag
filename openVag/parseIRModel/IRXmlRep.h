#pragma once

#include <map>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

class LayerPort {
public:
	LayerPort(const std::string& portID) : portID(portID) {}
	std::string portID;
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

struct Edge {
	Edge(std::string from_layer, std::string from_port, std::string to_layer, std::string to_port) : from_layer(from_layer), from_port(from_port), to_layer(to_layer), to_port(to_port) {}
	std::string from_layer;
	std::string from_port;
	std::string to_layer;
	std::string to_port;
};

class IRXmlRep {
public:
	std::vector<LayerNode> vecLayerNode;
	std::vector<Edge> vecEdge;
};