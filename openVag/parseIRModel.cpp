#include "parseIRModel.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <map>
#include <set>
#include <filesystem>

#include "IRModel.h"
#include "tinyxml2.h"
#include "OpenVag.h"

using namespace tinyxml2;
std::shared_ptr<IRModel> parseIRModel(std::shared_ptr<XMLDocument> doc);

class ParseIRModelException : public std::exception {
private:
    std::string message;
public:
    ParseIRModelException(const std::string& msg) : message(msg) {}
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

std::shared_ptr<Layer> parseLayer(std::string xmlString, tinyxml2::XMLDocument* xmlDocument)
{
    tinyxml2::XMLDocument tmpDoc;
    if (tmpDoc.Parse(xmlString.c_str()) != tinyxml2::XML_SUCCESS) { return {}; }
    auto xmlElement = tmpDoc.RootElement();
    auto value = xmlElement->Value();
    if (xmlElement == nullptr
        || xmlElement->Value() != std::string("layer")) return {};
    auto xmlLayerNode = xmlElement->DeepClone(xmlDocument);
    return parseLayer(xmlLayerNode->ToElement());
}

std::shared_ptr<Layer> parseLayer(XMLElement* xmlLayer) {
    assert(xmlLayer != nullptr);
    auto layerID = xmlLayer->Attribute("id");
    if (layerID == nullptr) {
        std::string msg = "Layer on line " + xmlLayer->GetLineNum() + std::string(" does not contain an 'id' attribute");
        throw ParseIRModelException(msg);
    }

    auto layer = std::make_shared<Layer>(xmlLayer);
    auto inputs = xmlLayer->FirstChildElement("input");
    parsePorts<InputPort>(inputs, layer);
    auto outputs = xmlLayer->FirstChildElement("output");
    parsePorts<OutputPort>(outputs, layer);

    return layer;
}

std::shared_ptr<Layers> parseLayers(XMLElement* xmlLayers, std::shared_ptr<Network> parent) {
    if (xmlLayers == nullptr) {
        xmlLayers = parent->getXmlElement()->GetDocument()->NewElement("layers");
        parent->getXmlElement()->InsertFirstChild(xmlLayers);
    }
    std::shared_ptr<Layers> layers = std::make_shared<Layers>(xmlLayers, parent);
    XMLElement* xmlLayer = xmlLayers->FirstChildElement("layer");
    while (xmlLayer != nullptr) {
        auto layer = parseLayer(xmlLayer);
        assert(layer != nullptr);
        layers->insertLayer(layer);

        xmlLayer = xmlLayer->NextSiblingElement();
    }

    return layers;
}

std::shared_ptr<Edge> parseEdge(XMLElement* edge, std::shared_ptr<Edges> parent) {
    assert(edge != nullptr);

    auto from_layer = edge->Attribute("from-layer");
    if (from_layer == nullptr) {
        return {};
    }

    auto from_port = edge->Attribute("from-port");
    if (from_port == nullptr) {
        return {};
    }

    auto to_layer = edge->Attribute("to-layer");
    if (to_layer == nullptr) {
        return {};
    }

    auto to_port = edge->Attribute("to-port");
    if (to_port == nullptr) {
        return {};
    }

    auto inputLayer = parent->getLayers()->getLayer(to_layer);
    if (inputLayer == nullptr) return {};
    auto outputLayer = parent->getLayers()->getLayer(from_layer);
    if (outputLayer == nullptr) return {};
    auto inputPort = inputLayer->getInputPort(to_port);
    if (inputPort == nullptr) return {};
    auto outputPort = outputLayer->getOutputPort(from_port);
    if (outputPort == nullptr) return {};

    return std::make_shared<Edge>(outputPort, inputPort, edge, parent);
}

std::shared_ptr<Edges> parseEdges(XMLElement* xmlEdges, std::shared_ptr<Network> parent) {
    if (xmlEdges == nullptr) {
        xmlEdges = parent->getXmlElement()->GetDocument()->NewElement("edges");
        parent->getXmlElement()->InsertFirstChild(xmlEdges);
    }
    std::shared_ptr<Edges> edges = std::make_shared<Edges>(xmlEdges, parent);
    XMLElement* edge = xmlEdges->FirstChildElement("edge");
    while (edge != nullptr) {
        const auto edgeGui = parseEdge(edge, edges);
        if (edgeGui) {
            edges->insertEdge(edgeGui);
        }
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

std::shared_ptr<IRModel> parseSimplifiedIRModel(const std::shared_ptr<XMLDocument>& doc) {
    auto workingDoc = doc;
    std::shared_ptr<IRModel> irModelGui = std::make_shared<IRModel>(workingDoc);

    XMLElement* root = workingDoc->RootElement();
    if (std::string(root->Value()) != "net") {
        std::shared_ptr<XMLDocument> newDoc = std::make_shared<XMLDocument>();
        XMLElement* net = newDoc->NewElement("net");
        newDoc->InsertFirstChild(net);
        if (std::string(root->Value()) != "layers") {
            XMLElement* layers = net->GetDocument()->NewElement("layers");
            net->InsertEndChild(layers);
            XMLElement* edges = net->GetDocument()->NewElement("edges");
            net->InsertEndChild(edges);
            for (XMLElement* xmlElement = root; xmlElement != nullptr; xmlElement = xmlElement->NextSiblingElement()) {
                if (std::string(xmlElement->Value()) == "layer") {
                    auto newXmlElement = xmlElement->DeepClone(layers->GetDocument());
                    layers->InsertEndChild(newXmlElement);
                } else if (std::string(xmlElement->Value()) == "edge") {
                    auto newXmlElement = xmlElement->DeepClone(layers->GetDocument());
                    edges->InsertEndChild(newXmlElement);
                }
            }
            workingDoc = newDoc;
        }
        else {
            //SeNe: Todo
            throw std::runtime_error("Not yet implemented");
        }
    }
    else {
        //SeNe: Todo
        throw std::runtime_error("Not yet implemented");
    }

    return parseIRModel(workingDoc);
}

std::shared_ptr<IRModel> parseSimplifiedIRModel(const char* xmlContent, size_t nBytes) {
    std::shared_ptr<XMLDocument> doc = std::make_shared<XMLDocument>();
    if (doc->Parse(xmlContent, nBytes) != XML_SUCCESS) {
        return {};
    }

    return parseSimplifiedIRModel(doc);
}

std::shared_ptr<IRModel> parseIRModel(std::shared_ptr<XMLDocument> doc) {
    XMLElement* net = doc->RootElement();
    if (net == nullptr) {
        throw ParseIRModelException("No net xml element");
    }

    if (std::string(net->Value()) != "net") {
        const auto irModelGui = parseSimplifiedIRModel(doc);
        if (irModelGui == nullptr) {
            throw ParseIRModelException("IR should start with net element");
        }
        return irModelGui;
    }

    std::shared_ptr<IRModel> irModelGui = std::make_shared<IRModel>(doc);
    irModelGui->setNetwork(parseNet(net, irModelGui));
    return irModelGui;
}

std::shared_ptr<IRModel> parseIRModel(const char* xmlContent, size_t nBytes) {
    std::shared_ptr<XMLDocument> doc = std::make_shared<XMLDocument>();
    if (doc->Parse(xmlContent, nBytes) != XML_SUCCESS) {
        throw ParseIRModelException("Failed to parse XML");
    }

    return parseIRModel(doc);
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
    std::filesystem::path filePath = fileName;
    const auto f = std::filesystem::absolute(filePath).string();
    std::ifstream file(filePath);
    if (!file) {
        throw ParseIRModelException(std::string("Failed to open file: ") + fileName);
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlContent = buffer.str();

    return parseIRModel(xmlContent.c_str(), xmlContent.size());
}
