#include "parseIRModel.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <map>
#include <set>

#include "IRModelGui.h"
#include "tinyxml2.h"
#include "OpenVag.h"

using namespace tinyxml2;

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

class ParseIRModelException : public std::exception {
private:
    std::string message;
public:
    ParseIRModelException(const char* msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

template <class LayerPortGuiType>
std::shared_ptr<LayerPortGuiType> parsePort(XMLElement* port) {
    assert(port != nullptr);
    auto portID = port->Attribute("id");
    if (portID == nullptr) {
        std::string msg = "Port on line " + port->GetLineNum() + std::string(" does not contain an 'id' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    ax::NodeEditor::PinId id_gui = GetNextId();
    return std::make_shared<LayerPortGuiType>(id_gui, port);
}

template <class LayerPortGuiType>
std::vector<std::shared_ptr<LayerPortGuiType>> parseLayerPorts(XMLElement* ports) {
    if (ports == nullptr) {
        return {};
    }

    std::vector<std::shared_ptr<LayerPortGuiType>> vecLayerPort;
    XMLElement* port = ports->FirstChildElement("port");
    while (port != nullptr) {
        vecLayerPort.push_back(parsePort<LayerPortGuiType>(port));
        port = port->NextSiblingElement();
    }

    return vecLayerPort;
}

std::shared_ptr<LayerNodeGui> parseLayer(XMLElement* layer) {
    auto layerID = layer->Attribute("id");
    if (layerID == nullptr) {
        std::string msg = "Layer on line " + layer->GetLineNum() + std::string(" does not contain an 'id' attribute");
        throw ParseIRModelException(msg.c_str());
    }
    auto name = layer->Attribute("name");
    if (name == nullptr) {
        std::string msg = "Layer on line " + layer->GetLineNum() + std::string(" does not contain an 'name' attribute");
        throw ParseIRModelException(msg.c_str());
    }
    auto type = layer->Attribute("type");
    if (type == nullptr) {
        std::string msg = "Layer on line " + layer->GetLineNum() + std::string(" does not contain an 'type' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    auto layerNodeGui = std::make_shared<LayerNodeGui>(GetNextId(), layer);
    auto inputs = layer->FirstChildElement("input");
    {
        layerNodeGui->vecInputPort = parseLayerPorts<LayerInputPortGui>(inputs);
        for (auto& port : layerNodeGui->vecInputPort) { port->parent = layerNodeGui; }
    }

    auto outputs = layer->FirstChildElement("output");
    {
        layerNodeGui->vecOutputPort = parseLayerPorts<LayerOutputPortGui>(outputs);
        for (auto& port : layerNodeGui->vecOutputPort) { port->parent = layerNodeGui; }
    }
    return layerNodeGui;
}

std::vector<std::shared_ptr<LayerNodeGui>> parseLayers(XMLElement* layers) {
    if (layers == nullptr) {
        return {};
    }

    std::vector<std::shared_ptr<LayerNodeGui>> vecLayerNode;
    XMLElement* layer = layers->FirstChildElement("layer");
    while (layer != nullptr) {
        vecLayerNode.push_back(parseLayer(layer));

        layer = layer->NextSiblingElement();
    }

    return vecLayerNode;
}

Edge parseEdge(XMLElement* edge) {
    assert(edge != nullptr);

   auto from_layer = edge->Attribute("from-layer");
    if (from_layer == nullptr) {
        std::string msg = "Edge on line " + edge->GetLineNum() + std::string(" does not contain an 'from-layer' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    auto from_port = edge->Attribute("from-port");
    if (from_port == nullptr) {
        std::string msg = "Edge on line " + edge->GetLineNum() + std::string(" does not contain an 'from-port' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    auto to_layer = edge->Attribute("to-layer");
    if (to_layer == nullptr) {
        std::string msg = "Edge on line " + edge->GetLineNum() + std::string(" does not contain an 'to-layer' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    auto to_port = edge->Attribute("to-port");
    if (to_port == nullptr) {
        std::string msg = "Edge on line " + edge->GetLineNum() + std::string(" does not contain an 'to-port' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    return Edge(from_layer, from_port, to_layer, to_port);
}

std::vector<Edge> parseEdges(XMLElement* edges) {
    if (edges == nullptr) {
        return {};
    }
    std::vector<Edge> vecEdgeRaw;

    XMLElement* edge = edges->FirstChildElement("edge");
    while (edge != nullptr) {
        vecEdgeRaw.push_back(parseEdge(edge));

        edge = edge->NextSiblingElement();
    }

    return vecEdgeRaw;
}

bool processEdges(std::vector<std::shared_ptr<LayerNodeGui>>& vecLayerNodeGui, const std::vector<Edge> vecEdge) {
    struct Port {
        Port(std::string layerID, std::string portID) : layerID(layerID), portID(portID) {}

        std::string layerID;
        std::string portID;

        bool operator<(const Port& other) const {
            if (this->layerID != other.layerID) return this->layerID < other.layerID;
            if (this->portID != other.portID) return this->portID < other.portID;

            return false;
        }

        std::string toString() {
            return this->layerID + ", " + this->portID;
        }
    };
    std::map<std::string, std::shared_ptr<LayerNodeGui>> mapXMLLayerIDtoGuiLayer;
    std::map<Port, std::shared_ptr<LayerInputPortGui>> mapInputPorttoLayerPort;
    std::map<Port, std::shared_ptr<LayerOutputPortGui>> mapOutputPorttoLayerPort;

    for (auto& layerNodeGui : vecLayerNodeGui) {
        if (mapXMLLayerIDtoGuiLayer.count(layerNodeGui->getXmlId())) {
            std::cerr << "Duplicate layer id " << layerNodeGui->getXmlId() << std::endl;
        }
        else {
            mapXMLLayerIDtoGuiLayer[layerNodeGui->getXmlId()] = layerNodeGui;
            for (auto& layerPort : layerNodeGui->vecInputPort) {
                Port port(layerNodeGui->getXmlId(), layerPort->getXmlId());
                if (mapInputPorttoLayerPort.count(port)) {
                    std::cerr << "Duplicate port: " << layerNodeGui->getXmlId() << std::endl;
                }
                else {
                    mapInputPorttoLayerPort[port] = layerPort;
                }
            }
            for (auto& layerPort : layerNodeGui->vecOutputPort) {
                Port port(layerNodeGui->getXmlId(), layerPort->getXmlId());
                if (mapOutputPorttoLayerPort.count(port)) {
                    std::cerr << "Duplicate port: " << layerNodeGui->getXmlId() << std::endl;
                }
                else {
                    mapOutputPorttoLayerPort[port] = layerPort;
                }
            }
        }
    }

    std::set<Edge> setEdge;
    for (auto& edge : vecEdge) {
        if (setEdge.count(edge)) {
            std::cerr << "Error: Duplicated edge: " << edge.toString() << std::endl;
            continue;
        }
        else {
            setEdge.insert(edge);
        }
        Port outputPort(edge.from_layer, edge.from_port);
        Port inputPort(edge.to_layer, edge.to_port);

        auto inputLayerPort = mapInputPorttoLayerPort[inputPort];
        auto outputLayerPort = mapOutputPorttoLayerPort[outputPort];
        ax::NodeEditor::LinkId linkID = GetNextId();
        std::shared_ptr<EdgeGui> edgeGui = std::make_shared<EdgeGui>(linkID, outputLayerPort, inputLayerPort);

        outputLayerPort->vecEdgeGui.push_back(edgeGui);
    }

    return true;
}

IRModelGui parseNet(XMLElement* net) {
    IRModelGui irModelGui;
    XMLElement* layers = net->FirstChildElement("layers");
    irModelGui.vecLayerNodeGui = parseLayers(layers);
    XMLElement* edges = net->FirstChildElement("edges");
    processEdges(irModelGui.vecLayerNodeGui, parseEdges(edges));
    return irModelGui;
}

static XMLDocument doc;
IRModelGui parseIRModel(const char* xmlContent, size_t nBytes) {
    if (doc.Parse(xmlContent) != XML_SUCCESS) {
        std::cerr << "Failed to parse XML" << std::endl;
        return {};
    }

    XMLElement* net = doc.RootElement();
    if (net == nullptr) {
        std::cerr << "No root element" << std::endl;
        return {};
    }

    if (std::string(net->Value()) != "net") {
        std::cerr << "IR should start with net element" << std::endl;
        return {};
    }

    return parseNet(net);
}

IRModelGui parseIRModel(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlContent = buffer.str();

    return parseIRModel(xmlContent.c_str(), xmlContent.size());
}