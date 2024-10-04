#include "parseIRModel.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <map>
#include <set>

#include "IRModel.h"
#include "tinyxml2.h"
#include "OpenVag.h"

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

template <class LayerPortGuiType>
std::shared_ptr<LayerPortGuiType> parsePort(XMLElement* port, std::shared_ptr<Layer> parent) {
    assert(port != nullptr);
    auto portID = port->Attribute("id");
    if (portID == nullptr) {
        std::string msg = "Port on line " + port->GetLineNum() + std::string(" does not contain an 'id' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    return std::make_shared<LayerPortGuiType>(port, parent);
}

template <class LayerPortGuiType>
void parsePorts(XMLElement* ports, std::shared_ptr<Layer> parent) {
    if(ports == nullptr) return;
    auto xmlElement = ports->FirstChildElement("port");
    while (xmlElement != nullptr) {
        parent->insertPort(parsePort<LayerPortGuiType>(xmlElement, parent));
        xmlElement = xmlElement->NextSiblingElement();
    }
}

void parseLayer(XMLElement* xmlLayer, std::shared_ptr<Layers> parent) {
    assert(xmlLayer != nullptr);
    auto layerID = xmlLayer->Attribute("id");
    if (layerID == nullptr) {
        std::string msg = "Layer on line " + xmlLayer->GetLineNum() + std::string(" does not contain an 'id' attribute");
        throw ParseIRModelException(msg.c_str());
    }
    auto name = xmlLayer->Attribute("name");
    if (name == nullptr) {
        std::string msg = "Layer on line " + xmlLayer->GetLineNum() + std::string(" does not contain an 'name' attribute");
        throw ParseIRModelException(msg.c_str());
    }
    auto type = xmlLayer->Attribute("type");
    if (type == nullptr) {
        std::string msg = "Layer on line " + xmlLayer->GetLineNum() + std::string(" does not contain an 'type' attribute");
        throw ParseIRModelException(msg.c_str());
    }

    auto layerGui = std::make_shared<Layer>(xmlLayer, parent);
    parent->insertLayer(layerGui);
    auto inputs = xmlLayer->FirstChildElement("input");
    parsePorts<InputPort>(inputs, layerGui);
    auto outputs = xmlLayer->FirstChildElement("output");
    parsePorts<OutputPort>(outputs, layerGui);
}

std::shared_ptr<Layers> parseLayers(XMLElement* xmlLayers, std::shared_ptr<Network> parent) {
    if (xmlLayers == nullptr) {
        xmlLayers = parent->getXmlElement()->el->GetDocument()->NewElement("layers");
        parent->getXmlElement()->el->InsertFirstChild(xmlLayers);
    }
    std::shared_ptr<Layers> layers = std::make_shared<Layers>(xmlLayers, parent);
    XMLElement* layer = xmlLayers->FirstChildElement("layer");
    while (layer != nullptr) {
        parseLayer(layer, layers);

        layer = layer->NextSiblingElement();
    }

    return layers;
}

std::shared_ptr<Edge> parseEdge(XMLElement* edge, std::shared_ptr<Edges> parent) {
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

    auto inputLayer = parent->getLayers()->getLayer(to_layer);
    auto outputLayer = parent->getLayers()->getLayer(from_layer);
    auto inputPort = inputLayer->getInputPort(to_port);
    auto outputPort = outputLayer->getOutputPort(from_port);

    assert(outputPort != nullptr);
    assert(inputPort != nullptr);

    return std::make_shared<Edge>(outputPort, inputPort, edge, parent);
}

std::shared_ptr<Edges> parseEdges(XMLElement* xmlEdges, std::shared_ptr<Network> parent) {
    if (xmlEdges == nullptr) {
        xmlEdges = parent->getXmlElement()->el->GetDocument()->NewElement("edges");
        parent->getXmlElement()->el->InsertFirstChild(xmlEdges);
    }
    std::shared_ptr<Edges> edges = std::make_shared<Edges>(xmlEdges, parent);
    XMLElement* edge = xmlEdges->FirstChildElement("edge");
    while (edge != nullptr) {
        edges->insertEdge(parseEdge(edge, edges));
        edge = edge->NextSiblingElement();
    }
    return edges;
}

static std::shared_ptr<Network> parseNet(XMLElement* net, std::shared_ptr<IRModel> parent) {
    std::shared_ptr<Network> network = std::make_shared<Network>(net, parent);
    XMLElement* layers = net->FirstChildElement("layers");
    network->setLayers(parseLayers(layers, network));
    XMLElement* edges = net->FirstChildElement("edges");
    network->setEdges(parseEdges(edges, network));
    return network;
}

std::shared_ptr<IRModel> parseIRModel(const char* xmlContent, size_t nBytes) {
    std::shared_ptr<XMLDocument> doc = std::make_shared<XMLDocument>();
    if (doc->Parse(xmlContent) != XML_SUCCESS) {
        std::cerr << "Failed to parse XML" << std::endl;
        return {};
    }

    XMLElement* net = doc->RootElement();
    if (net == nullptr) {
        std::cerr << "No net element" << std::endl;
        return {};
    }

    if (std::string(net->Value()) != "net") {
        std::cerr << "IR should start with net element" << std::endl;
        return {};
    }

    std::shared_ptr<IRModel> irModelGui = std::make_shared<IRModel>(doc);
    irModelGui->setNetwork(parseNet(net, irModelGui));
    return irModelGui;
}

std::shared_ptr<IRModel> parseIRModel() {
    std::shared_ptr<XMLDocument> doc = std::make_shared<XMLDocument>();
    tinyxml2::XMLElement* net = doc->NewElement("net");
    doc->InsertFirstChild(net);
    net->SetAttribute("name", "openVag.xml");
    net->SetAttribute("version", "10");
    tinyxml2::XMLElement* layers = doc->NewElement("layers");
    net->InsertFirstChild(layers);
    tinyxml2::XMLElement* edges = doc->NewElement("edges");
    net->InsertEndChild(edges);

    std::shared_ptr<IRModel> irModelGui = std::make_shared<IRModel>(doc);
    irModelGui->setNetwork(parseNet(net, irModelGui));
    return irModelGui;
}

std::shared_ptr<IRModel> parseIRModel(const std::string& fileName) {
    if (fileName.empty()) return parseIRModel();
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
