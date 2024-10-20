#pragma once

#include <map>
#include <string>
#include <vector>
#include <map>
#include <memory>

class LayerPort {
public:
	LayerPort(const LayerPort& obj) : portID(obj.portID) {}
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
	std::vector<std::shared_ptr<LayerPort>> vecInputPort;
	std::vector<std::shared_ptr<LayerPort>> vecOutputPort;
};

class Edge {
public:
	Edge(std::string from_layer, std::string from_port, std::string to_layer, std::string to_port) : from_layer(from_layer), from_port(from_port), to_layer(to_layer), to_port(to_port) {}
	std::string from_layer;
	std::string from_port;
	std::string to_layer;
	std::string to_port;

	std::string toString() const {
		return std::string("from-layer=\"") + from_layer + "\" from-port=\"" + from_port + " to-layer=\"" + to_layer + " to-port=\"" + to_port;
	}

	bool operator<(const Edge& other) const {
		if (from_layer != other.from_layer) return from_layer < other.from_layer;
		if (from_port != other.from_port) return from_port < other.from_port;
		if (to_layer != other.to_layer) return to_layer < other.to_layer;
		return to_port < other.to_port;
	}

	bool operator==(const Edge& other) const {
		return from_layer == other.from_layer &&
		from_port == other.from_port &&
		to_layer == other.to_layer &&
		to_port == other.to_port;
	}
};

class IRXmlRep {
public:
	std::vector<LayerNode> vecLayerNode;
	std::vector<Edge> vecEdge;
};