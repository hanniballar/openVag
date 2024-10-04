#include "IRXmlRep.h"
#include "tinyxml2.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

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

IRXmlRep parseIRModel(const std::string& fileName) {
    std::ifstream file("fileName");
    if (!file) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlContent = buffer.str();

    return parseIRModel(xmlContent.c_str());
}

LayerPort parsePort(XMLElement* port) {

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
    auto inputs = layer->FirstChildElement("input");
    auto outputs = layer->FirstChildElement("output");
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

IRXmlRep parseNet(XMLElement* net) {
    IRXmlRep irXmlRep;
    XMLElement* layers = net->FirstChildElement("layers");
    irXmlRep.vecLayerNode = parseLayers(layers)
 

}

IRXmlRep parseIRModel(const char* xmlContent, size_t nBytes) {
    XMLDocument doc;
    if (doc.Parse(xmlContent) != XML_SUCCESS) {
        std::cerr << "Failed to parse XML" << std::endl;
        return {};
    }

    XMLElement* root = doc.RootElement();
    if (root == nullptr) {
        std::cerr << "No root element" << std::endl;
        return {};
    }
    XMLElement* net = root->FirstChildElement("net");
    if (net == nullptr) {
        std::cerr << "No net element" << std::endl;
        return {};
    }

    return parseNet(net);
}