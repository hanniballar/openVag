#include "IRXmlRep.h"
#include "tinyxml2.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cassert>

using namespace tinyxml2;

class ParseIRModelException : public std::exception {
private:
    std::string message;
public:
    ParseIRModelException(const char* msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

LayerPort parsePort(XMLElement* port) {
    assert(port != nullptr);
    auto portID = port->Attribute("id");
    if (portID == nullptr) {
        std::string msg = "Port on line " + port->GetLineNum() + std::string(" does not contain an 'id' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    return LayerPort(portID);
}

std::vector<LayerPort> parseLayerPorts(XMLElement* ports) {
    if (ports == nullptr) {
        return {};
    }

    std::vector<LayerPort> vecLayerPort;
    XMLElement* port = ports->FirstChildElement("port");
    while (port != nullptr) {
        vecLayerPort.push_back(parsePort(port));
        port = port->NextSiblingElement();
    }

    return vecLayerPort;
}

LayerNode parseLayer(XMLElement* layer) {
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

    LayerNode layerNode(layerID, name, type);
    auto inputs = layer->FirstChildElement("input");
    layerNode.vecInputPort = parseLayerPorts(inputs);

    auto outputs = layer->FirstChildElement("output");
    layerNode.vecOutputPort = parseLayerPorts(outputs);
    return layerNode;
}

std::vector<LayerNode> parseLayers(XMLElement* layers) {
    if (layers == nullptr) {
        return {};
    }

    std::vector<LayerNode> vecLayerNode;
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

IRXmlRep parseNet(XMLElement* net) {
    IRXmlRep irXmlRep;
    XMLElement* layers = net->FirstChildElement("layers");
    irXmlRep.vecLayerNode = parseLayers(layers);
    XMLElement* edges = net->FirstChildElement("edges");
    irXmlRep.vecEdge = parseEdges(edges);

    return irXmlRep;
}

IRXmlRep parseIRModel(const char* xmlContent, size_t nBytes) {
    XMLDocument doc;
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

IRXmlRep parseIRModel(const std::string& fileName) {
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