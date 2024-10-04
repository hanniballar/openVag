#include "IRModel.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <imgui_node_editor.h>
#include <cassert>

#include "tinyxml2.h"
#include "OpenVag.h"

static std::set<std::shared_ptr<Edge>, EdgeIDLess> emptyEdgeSet;
static std::set<std::shared_ptr<Layer>, LayerIDLess> emptyLayerSet;
static std::shared_ptr<Layer> emptyLayer;
static std::shared_ptr<Edge> emptyEdge;
static std::shared_ptr<OutputPort> emptyOutputPort;
static std::shared_ptr<InputPort> emptyInputPort;

static size_t getXmlSiblingPosition(const tinyxml2::XMLNode* node) {
    assert(node != nullptr);
    size_t pos = 0;
    auto prevSibling = node->PreviousSibling();
    while (prevSibling != nullptr) {
        ++pos;
        prevSibling = prevSibling->PreviousSibling();
    }

    return pos;
}

static tinyxml2::XMLNode* getNthXmlSibling(tinyxml2::XMLNode* node, size_t position) {
    assert(node != nullptr);
    size_t curPos = 0;
    auto* sibling = node;
    while (curPos < position) {
        ++curPos;
        sibling = sibling->NextSibling();
        assert(sibling != nullptr);
    }

    return sibling;
}

static void insertNodeAtPosition(tinyxml2::XMLNode* insertThis, tinyxml2::XMLNode* parent, size_t position) {
    assert(parent != nullptr);
    assert(insertThis != nullptr);
    if (position == 0)
        parent->InsertFirstChild(insertThis);
    else {
        auto xmlNode = getNthXmlSibling(parent->FirstChild(), position - 1);
        parent->InsertAfterChild(xmlNode, insertThis);
    }
}

int64_t Layers::getMaxLayerXmlId() const {
    int64_t maxId = 0;
    for (const auto& nodeIdToLayer : mapNodeIdToLayer) {
        std::stringstream ss(nodeIdToLayer.second->getXmlId());
        int64_t xmlIdNum = 0;
        ss >> xmlIdNum;
        maxId = std::max(maxId, xmlIdNum);
    }
    return maxId;
}

void Layers::deleteLayer(const std::shared_ptr<Layer>& layer)
{
    removeLayer(layer);
    getXmlElement()->el->DeleteChild(layer->getXmlElement()->el);
}

void Layers::removeLayer(const std::shared_ptr<Layer>& layer)
{
    for (const auto port : layer->getSetInputPort()) removePort(port);
    for (const auto port : layer->getSetOutputPort()) removePort(port);

    assert(mapNodeIdToLayer.find(layer->getId()) != mapNodeIdToLayer.end());
    mapNodeIdToLayer.erase(layer->getId());

    assert(mapXMLIdToSetLayer.find(layer->getXmlId()) != mapXMLIdToSetLayer.end());
    auto itMapXMLIdToSetLayer = mapXMLIdToSetLayer.find(layer->getXmlId());
    assert(itMapXMLIdToSetLayer->second.find(layer) != itMapXMLIdToSetLayer->second.end());
    itMapXMLIdToSetLayer->second.erase(layer);
    if (itMapXMLIdToSetLayer->second.size() == 0) mapXMLIdToSetLayer.erase(itMapXMLIdToSetLayer);

    layer->resetParent();
}

std::shared_ptr<Layer> Layers::insertNewLayer()
{
    auto xmlId = std::to_string(getMaxLayerXmlId() + 1);
    std::string name = std::string("openVagLayer") + xmlId;

    return insertNewLayer(xmlId, name, std::string("Const"));
}

std::shared_ptr<Layer> Layers::insertNewLayer(std::string xmlId, std::string name, std::string type)
{
    auto layer = std::make_shared<Layer>(getXmlElement()->el->GetDocument(), xmlId, name, type);
    xmlElement->el->InsertEndChild(layer->getXmlElement()->el);
    insertLayer(layer);

    return layer;
}

void Layers::insertLayer(std::shared_ptr<Layer> layer)
{
    assert(mapNodeIdToLayer.find(layer->getId()) == mapNodeIdToLayer.end());
    layer->setParent(shared_from_this());
    mapNodeIdToLayer[layer->getId()] = layer;

    if (mapXMLIdToSetLayer.find(layer->getXmlId()) == mapXMLIdToSetLayer.end()) {
        mapXMLIdToSetLayer[layer->getXmlId()] = {};
    }
    mapXMLIdToSetLayer[layer->getXmlId()].insert(layer);

    for (auto& port : layer->getSetInputPort()) insertPort(port);
    for (auto& port : layer->getSetOutputPort()) insertPort(port);
}

void Layers::insertLayer(std::shared_ptr<Layer> layer, size_t position) {
    insertNodeAtPosition(layer->getXmlElement()->el, getXmlElement()->el, position);
    insertLayer(layer);
}

void Layers::insertPort(std::shared_ptr<InputPort> port)
{
    assert(mapPinIdToInputPort.find(port->getId()) == mapPinIdToInputPort.end());
    mapPinIdToInputPort[port->getId()] = port;
}

void Layers::removePort(std::shared_ptr<InputPort> port)
{
    assert(mapPinIdToInputPort.find(port->getId()) != mapPinIdToInputPort.end());
    mapPinIdToInputPort.erase(port->getId());
}

void Layers::insertPort(std::shared_ptr<OutputPort> port)
{
    assert(mapPinIdToOutputPort.find(port->getId()) == mapPinIdToOutputPort.end());
    mapPinIdToOutputPort[port->getId()] = port;
}

void Layers::removePort(std::shared_ptr<OutputPort> port)
{
    assert(mapPinIdToOutputPort.find(port->getId()) != mapPinIdToOutputPort.end());
    mapPinIdToOutputPort.erase(port->getId());
}

const std::shared_ptr<Layer>& Layers::getLayer(ax::NodeEditor::NodeId id) const
{
    auto it = mapNodeIdToLayer.find(id);
    return (it != mapNodeIdToLayer.end()) ? it->second : emptyLayer;
}

const std::shared_ptr<Layer>& Layers::getLayer(std::string id) const
{
    const auto& set = getSetLayer(id);

    return *(getSetLayer(id).begin());
}

void Layers::changeLayerXmlId(std::shared_ptr<Layer> layer, std::string oldXmlId, std::string newXmlId)
{
    auto it = mapXMLIdToSetLayer.find(oldXmlId);
    assert(it != mapXMLIdToSetLayer.end());
    it->second.erase(layer);
    if (it->second.empty()) mapXMLIdToSetLayer.erase(it);
    if (mapXMLIdToSetLayer.find(newXmlId) == mapXMLIdToSetLayer.end()) {
        mapXMLIdToSetLayer[newXmlId] = {};
    }
    mapXMLIdToSetLayer[newXmlId].insert(layer);
}

const std::set<std::shared_ptr<Layer>, LayerIDLess>& Layers::getSetLayer(std::string id) const
{
    auto it = mapXMLIdToSetLayer.find(id);
    return (it != mapXMLIdToSetLayer.end()) ? it->second : emptyLayerSet;
}

const std::shared_ptr<InputPort>& Layers::getInputPort(ax::NodeEditor::PinId id) const
{
    auto it = mapPinIdToInputPort.find(id);
    return (it != mapPinIdToInputPort.end()) ? it->second : emptyInputPort;
}

const std::shared_ptr<OutputPort>& Layers::getOutputPort(ax::NodeEditor::PinId id) const
{
    auto it = mapPinIdToOutputPort.find(id);
    return (it != mapPinIdToOutputPort.end()) ? it->second : emptyOutputPort;
}

std::shared_ptr<Network> Layer::getNetwork() const {
    return parent->getParent();
}

std::shared_ptr<Edges> Layer::getEdges() const {
    return getNetwork()->getEdges();
}

Layer::Layer(tinyxml2::XMLDocument* xmlDocument, std::string xmlId, std::string name, std::string type) : id(GetNextId())
{
    auto xmlLayerRaw = xmlDocument->NewElement("layer");
    xmlLayerRaw->SetAttribute("id", xmlId.c_str());
    xmlLayerRaw->SetAttribute("name", name.c_str());
    xmlLayerRaw->SetAttribute("type", type.c_str());

    xmlElement = XMLNodeWrapper::make_shared(xmlLayerRaw);
}

Layer::Layer(tinyxml2::XMLElement* xmlLayer, const std::shared_ptr<Layers>& parent) : id(GetNextId()), parent(parent) {
    this->xmlElement = XMLNodeWrapper::make_shared(xmlLayer);
}

std::shared_ptr<XMLNodeWrapper> Layer::getXmlInputElement() const
{
    auto xmlElRaw = getXmlElement()->el;
    auto xmlInputElementRaw = xmlElRaw->FirstChildElement("input");
    if (xmlInputElementRaw == nullptr) {
        xmlInputElementRaw = xmlElRaw->GetDocument()->NewElement("input");
        auto xmlOutputElementRaw = xmlElRaw->FirstChildElement("output");
        if (xmlOutputElementRaw == nullptr)
            xmlElRaw->InsertEndChild(xmlInputElementRaw);
        else {
            auto prev = xmlOutputElementRaw->PreviousSiblingElement();
            if (prev == nullptr) {
                xmlElRaw->InsertFirstChild(xmlInputElementRaw);
            }
            else {
                xmlElRaw->InsertAfterChild(prev, xmlInputElementRaw);
            }
        }
    }

    return XMLNodeWrapper::make_shared(xmlInputElementRaw);
}

std::shared_ptr<XMLNodeWrapper> Layer::getXmlOutputElement() const
{
    auto xmlElRaw = getXmlElement()->el;
    auto xmlOutputElementRaw = xmlElRaw->FirstChildElement("output");
    if (xmlOutputElementRaw == nullptr) {
        xmlOutputElementRaw = xmlElRaw->GetDocument()->NewElement("output");
        auto xmlInputElementRaw = xmlElRaw->FirstChildElement("input");
        if (xmlInputElementRaw == nullptr)
            xmlElRaw->InsertEndChild(xmlOutputElementRaw);
        else xmlElRaw->InsertAfterChild(xmlInputElementRaw, xmlOutputElementRaw);
    }

    return XMLNodeWrapper::make_shared(xmlOutputElementRaw);
}

size_t Layer::getXmlPosition() const {
    return getXmlSiblingPosition(getXmlElement()->el);
}

std::set<std::shared_ptr<Edge>, EdgeIDLess> Layer::getSetEdges() const
{
    std::set<std::shared_ptr<Edge>, EdgeIDLess> allEdges = getEdges()->getSetEdgesFromLayer(getId());
    auto incomingEdges = getEdges()->getSetEdgesToLayer(getId());
    allEdges.insert(incomingEdges.begin(), incomingEdges.end());
    
    return allEdges;
}

std::shared_ptr<InputPort> Layer::insertNewInputPort() {
    auto xmlId = std::to_string(getMaxPortXmlId() + 1);
    auto inputPort = std::make_shared<InputPort>(getXmlElement()->el->GetDocument(), xmlId);
    getXmlInputElement()->el->InsertEndChild(inputPort->getXmlElement()->el);
    insertPort(inputPort);
    return inputPort;
}

std::shared_ptr<OutputPort> Layer::insertNewOutputPort() {
    auto xmlId = std::to_string(getMaxPortXmlId() + 1);
    auto outputPort = std::make_shared<OutputPort>(getXmlElement()->el->GetDocument(), xmlId);
    getXmlOutputElement()->el->InsertEndChild(outputPort->getXmlElement()->el);
    insertPort(outputPort);
    return outputPort;
}

void Layer::insertPort(std::shared_ptr<InputPort> port)
{
    port->setParent(shared_from_this());
    setInputPort.insert(port);
    getParent()->insertPort(port);
}

void Layer::insertPort(std::shared_ptr<OutputPort> port)
{
    port->setParent(shared_from_this());
    setOutputPort.insert(port);
    getParent()->insertPort(port);
}

void Layer::insertPort(std::shared_ptr<InputPort> port, size_t position)
{
    insertNodeAtPosition(port->getXmlElement()->el, getXmlInputElement()->el, position);
    port->setParent(shared_from_this());
    insertPort(port);
}

void Layer::insertPort(std::shared_ptr<OutputPort> port, size_t position)
{
    insertNodeAtPosition(port->getXmlElement()->el, getXmlOutputElement()->el, position);
    port->setParent(shared_from_this());
    insertPort(port);
}

void Layer::removePort(const std::shared_ptr<InputPort>& port)
{
    setInputPort.erase(port);
    getParent()->removePort(port);
    port->resetParent();
}

void Layer::deletePort(const std::shared_ptr<InputPort>& port)
{
    removePort(port);
    auto xmlInputElementRaw = getXmlInputElement()->el;
    xmlInputElementRaw->DeleteChild(port->getXmlElement()->el);
    if (xmlInputElementRaw->NoChildren()) {
        xmlInputElementRaw->Parent()->DeleteChild(xmlInputElementRaw);
    }

}

void Layer::removePort(const std::shared_ptr<OutputPort>& port)
{
    setOutputPort.erase(port);
    getParent()->removePort(port);
    port->resetParent();
}

void Layer::deletePort(const std::shared_ptr<OutputPort>& port)
{
    removePort(port);
    auto xmlOutputElementRaw = getXmlOutputElement()->el;
    xmlOutputElementRaw->DeleteChild(port->getXmlElement()->el);
    if (xmlOutputElementRaw->NoChildren()) {
        xmlOutputElementRaw->Parent()->DeleteChild(xmlOutputElementRaw);
    }
}

std::shared_ptr<InputPort> Layer::getInputPort(std::string xmlId)
{
    for (auto& port : setInputPort) if (xmlId == std::string(port->getXmlId())) return port;

    return {};
}

std::shared_ptr<OutputPort> Layer::getOutputPort(std::string xmlId)
{
    for (auto& port : setOutputPort) if (xmlId == std::string(port->getXmlId())) return port;

    return {};
}

std::set<std::shared_ptr<Layer>, LayerIDLess> Layer::getInputLayers()
{
    auto edges = getEdges();
    auto setToLayersEdges = edges->getSetEdgesToLayer(getId());
    std::set<std::shared_ptr<Layer>, LayerIDLess> setInputLayers;
    for (const auto& edge : setToLayersEdges)
        if (edge->getFromLayer()->getId().Get() != getId().Get())
            setInputLayers.insert(edge->getFromLayer());
    
    return setInputLayers;
}

std::set<std::shared_ptr<Layer>, LayerIDLess> Layer::getOutputLayers()
{
    auto edges = getEdges();
    auto setFromLayersEdges = edges->getSetEdgesFromLayer(getId());
    std::set<std::shared_ptr<Layer>, LayerIDLess> setOutputLayers;
    for (const auto& edge : setFromLayersEdges)
        if (edge->getToLayer()->getId().Get() != getId().Get())
            setOutputLayers.insert(edge->getToLayer());

    return setOutputLayers;
}

int64_t Layer::getMaxPortXmlId() const {
    int64_t maxId = -1;

    for (const auto& port : getSetInputPort()) {
        std::stringstream ss(port->getXmlId());
        int64_t xmlIdNum = 0;
        ss >> xmlIdNum;
        maxId = std::max(maxId, xmlIdNum);
    }
    for (const auto& port : getSetOutputPort()) {
        std::stringstream ss(port->getXmlId());
        int64_t xmlIdNum = 0;
        ss >> xmlIdNum;
        maxId = std::max(maxId, xmlIdNum);
    }
    return maxId;
}

static void deleteAllAttributes(tinyxml2::XMLElement* xmlElementRaw) {
    for (const auto* attr = xmlElementRaw->FirstAttribute();
        attr != nullptr;
        xmlElementRaw->DeleteAttribute(attr->Name()), attr = xmlElementRaw->FirstAttribute()) {
    }
}

void Layer::modifyAttributes(std::vector<std::pair<std::string, std::string>> vecAttribute)
{
    const auto oldId = std::string(getXmlId());
    auto xmlElementRaw = getXmlElement()->el->ToElement();
    deleteAllAttributes(xmlElementRaw);
    for (const auto& [attrName, attrValue] : vecAttribute) {
        xmlElementRaw->SetAttribute(attrName.c_str(), attrValue.c_str());
    }

    const auto newId = std::string(getXmlId());
    if (oldId != newId) {
        auto layers = getParent();
        layers->changeLayerXmlId(shared_from_this(), oldId, newId);
        auto edges = getEdges();
        auto setToLayersEdges = edges->getSetEdgesToLayer(getId());
        for (auto& edge : setToLayersEdges) {
            edge->modifyAttributes({ { "to-layer", newId} });
        }
        auto setFromLayersEdges = edges->getSetEdgesFromLayer(getId());
        for (auto& edge : setFromLayersEdges) {
            edge->modifyAttributes({ { "from-layer", newId } });
        }
    }
}

std::shared_ptr<Edge> Edges::insertNewEdge(const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port, size_t xmlPos)
{
    auto inputLayer = getLayers()->getLayer(to_layer);
    auto outputLayer = getLayers()->getLayer(from_layer);
    auto inputPort = inputLayer->getInputPort(to_port);
    auto outputPort = outputLayer->getOutputPort(from_port);

    assert(outputPort != nullptr);
    assert(inputPort != nullptr);

    auto edge = std::make_shared<Edge>(getXmlElement()->el->GetDocument(), outputPort, inputPort);
    insertNodeAtPosition(edge->getXmlElement()->el, xmlElement->el, xmlPos);
    insertEdge(edge);

    return edge;
}

std::shared_ptr<Edge> Edges::insertNewEdge(const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port)
{
    assert (std::find_if(begin(), end(), [&](std::shared_ptr<Edge>& edge) {
        auto res = from_layer == edge->getOutputPort()->getParent()->getXmlId()
            && from_port == edge->getOutputPort()->getXmlId()
            && to_layer == edge->getInputPort()->getParent()->getXmlId()
            && to_port == edge->getInputPort()->getXmlId();
        return res;
        }) == end());

    auto inputLayer = getLayers()->getLayer(to_layer);
    auto outputLayer = getLayers()->getLayer(from_layer);
    auto inputPort = inputLayer->getInputPort(to_port);
    auto outputPort = outputLayer->getOutputPort(from_port);

    assert(outputPort != nullptr);
    assert(inputPort != nullptr);

    auto edge = std::make_shared<Edge>(getXmlElement()->el->GetDocument(), outputPort, inputPort);
    xmlElement->el->InsertEndChild(edge->getXmlElement()->el);

    insertEdge(edge);
    return edge;
}

void Edges::insertEdge(std::shared_ptr<Edge> edge)
{
    assert(mapLinkIdToEdge.find(edge->getId()) == mapLinkIdToEdge.end());
    edge->setParent(shared_from_this());
    mapLinkIdToEdge[edge->getId()] = edge;
    if (mapFromLayerIdToSetEdge.find(edge->getFromLayer()->getId()) == mapFromLayerIdToSetEdge.end()) {
        mapFromLayerIdToSetEdge[edge->getFromLayer()->getId()] = {};
    }
    mapFromLayerIdToSetEdge[edge->getFromLayer()->getId()].insert(edge);

    if (mapToLayerIdToSetEdge.find(edge->getToLayer()->getId()) == mapToLayerIdToSetEdge.end()) {
        mapToLayerIdToSetEdge[edge->getToLayer()->getId()] = {};
    }
    mapToLayerIdToSetEdge[edge->getToLayer()->getId()].insert(edge);
}

void Edges::insertEdge(std::shared_ptr<Edge> edge, size_t position)
{
    insertNodeAtPosition(edge->getXmlElement()->el, getXmlElement()->el, position);
    insertEdge(edge);
}

void Edges::deleteEdge(const std::shared_ptr<Edge>& edge)
{
    removeEdge(edge);
    getXmlElement()->el->DeleteChild(edge->getXmlElement()->el);
}

void Edges::removeEdge(const std::shared_ptr<Edge>& edge)
{
    mapToLayerIdToSetEdge[edge->getToLayer()->getId()].erase(edge);
    if (mapToLayerIdToSetEdge[edge->getToLayer()->getId()].size() == 0) mapToLayerIdToSetEdge.erase(edge->getToLayer()->getId());

    mapFromLayerIdToSetEdge[edge->getFromLayer()->getId()].erase(edge);
    if (mapFromLayerIdToSetEdge[edge->getFromLayer()->getId()].size() == 0) mapFromLayerIdToSetEdge.erase(edge->getFromLayer()->getId());

    mapLinkIdToEdge.erase(edge->getId());
    edge->resetParent();
}

std::shared_ptr<Layers> Edges::getLayers() {
    return parent->getLayers();
}

const std::set<std::shared_ptr<Edge>, EdgeIDLess>& Edges::getSetEdgesToLayer(ax::NodeEditor::NodeId nodeId) const
{
    auto it = mapToLayerIdToSetEdge.find(nodeId);
    return it != mapToLayerIdToSetEdge.end() ? it->second: emptyEdgeSet;
}

const std::set<std::shared_ptr<Edge>, EdgeIDLess>& Edges::getSetEdgesFromLayer(ax::NodeEditor::NodeId nodeId) const
{
    auto it = mapFromLayerIdToSetEdge.find(nodeId);
    return it != mapFromLayerIdToSetEdge.end() ? it->second : emptyEdgeSet;
}

const std::shared_ptr<Edge>& Edges::getEdge(std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort) const
{
    auto setLayerEdges = getSetEdgesFromLayer(outputPort->getParent()->getId());
    auto it = std::find_if(setLayerEdges.begin(), setLayerEdges.end(), [&](std::shared_ptr<Edge> edge) { return edge->getInputPort() == inputPort; });
    return (it != setLayerEdges.end()) ? *it : emptyEdge;
}

const std::shared_ptr<Edge>& Edges::getEdge(ax::NodeEditor::LinkId id) const
{
    auto it = mapLinkIdToEdge.find(id);
    return (it != mapLinkIdToEdge.end()) ? it->second : emptyEdge;
}

Edge::Edge(tinyxml2::XMLDocument* xmlDocument, const std::shared_ptr<OutputPort>& outputPort, const std::shared_ptr<InputPort>& inputPort) : id (GetNextId()), outputPort(outputPort), inputPort(inputPort)
{
    auto xmlEdgeRaw = xmlDocument->NewElement("edge");
    xmlEdgeRaw->SetAttribute("from-layer", outputPort->getParent()->getXmlId());
    xmlEdgeRaw->SetAttribute("from-port", outputPort->getXmlId());
    xmlEdgeRaw->SetAttribute("to-layer", inputPort->getParent()->getXmlId());
    xmlEdgeRaw->SetAttribute("to-port", inputPort->getXmlId());

    xmlElement = XMLNodeWrapper::make_shared(xmlEdgeRaw);
}

Edge::Edge(std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort, tinyxml2::XMLElement* edge, std::shared_ptr<Edges> parent) : id(GetNextId()), outputPort(outputPort), inputPort(inputPort), parent(parent) {
    this->xmlElement = XMLNodeWrapper::make_shared(edge);
}

size_t Edge::getXmlPosition() const {
    return getXmlSiblingPosition(getXmlElement()->el);
}

void Edge::modifyAttributes(std::map<std::string, std::string> mapAttribute)
{
    const auto parent = getParent();
    parent->removeEdge(shared_from_this());

    std::set<std::string> setAllowedAttribute = { "from-layer", "from-port", "to-layer", "to-port" };

    auto xmlElementRaw = getXmlElement()->el->ToElement();
    for (const auto& [attrName, attrValue] : mapAttribute) {
        assert(setAllowedAttribute.find(attrName) != setAllowedAttribute.end());
        xmlElementRaw->SetAttribute(attrName.c_str(), attrValue.c_str());
    }

    auto from_layer = xmlElementRaw->Attribute("from-layer");
    auto from_port = xmlElementRaw->Attribute("from-port");
    auto to_layer = xmlElementRaw->Attribute("to-layer");
    auto to_port = xmlElementRaw->Attribute("to-port");

    auto inputLayer = parent->getLayers()->getLayer(to_layer);
    auto outputLayer = parent->getLayers()->getLayer(from_layer);
    inputPort = inputLayer->getInputPort(to_port);
    outputPort = outputLayer->getOutputPort(from_port);

    assert(outputPort != nullptr);
    assert(inputPort != nullptr);

    parent->insertEdge(shared_from_this());
}

Port::Port(tinyxml2::XMLDocument* xmlDocument, std::string xmlId) : id(GetNextId())
{
    auto xmlPortRaw = xmlDocument->NewElement("port");
    xmlPortRaw->SetAttribute("id", xmlId.c_str());

    xmlElement = XMLNodeWrapper::make_shared(xmlPortRaw);
}

Port::Port(tinyxml2::XMLElement* xmlElement, std::shared_ptr<Layer> parent) : id(GetNextId()), parent(parent) {
    this->xmlElement = XMLNodeWrapper::make_shared(xmlElement);
}

const std::shared_ptr<Layers>& Port::getLayers() const
{
    return getParent()->getParent();
}

const std::shared_ptr<Network>& Port::getNetwork() const
{
    return getLayers()->getParent();
}

const std::shared_ptr<Edges>& Port::getEdges() const
{
    return getNetwork()->getEdges();
}

size_t Port::getXmlPosition() const
{
    return getXmlSiblingPosition(getXmlElement()->el);
}

bool EdgeIDLess::operator()(const std::shared_ptr<Edge>& lhs, const std::shared_ptr<Edge>& rhs) const
{
    return LinkIdLess()(lhs->getId(), rhs->getId());
}

std::set<std::shared_ptr<Edge>, EdgeIDLess> InputPort::getSetEdges() const
{
    auto edges = getEdges()->getSetEdgesToLayer(getParent()->getId());

    auto it = edges.begin();
    while (it != edges.end()) {
        if ((*it)->getInputPort().get() != this) {
            it = edges.erase(it);
        } else { ++it; }
    }
    return edges;
}

std::set<std::shared_ptr<Edge>, EdgeIDLess> OutputPort::getSetEdges() const
{
    auto edges = getEdges()->getSetEdgesFromLayer(getParent()->getId());

    auto it = edges.begin();
    while (it != edges.end()) {
        if ((*it)->getOutputPort().get() != this) {
            it = edges.erase(it);
        }
        else { ++it; }
    }
    return edges;
}
