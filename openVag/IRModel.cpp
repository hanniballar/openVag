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

const std::shared_ptr<IRModel>& Layers::getIRModel() const
{
    return getNetwork()->getParent();
}

void Layers::deleteLayer(const std::shared_ptr<Layer>& layer)
{
    removeLayer(layer);
    getXmlElement()->DeleteChild(layer->getXmlElement());
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

    getIRModel()->sendEventModifyIR();
}

void Layers::insertLayer(std::shared_ptr<Layer> layer)
{
    assert(mapNodeIdToLayer.find(layer->getId()) == mapNodeIdToLayer.end());
    {
        const auto xmlLayers = getXmlElement();
        const auto xmlLayer = layer->getXmlElement();
        if (xmlLayer->Parent() != xmlLayers) {
            xmlLayers->InsertEndChild(xmlLayer);
        }
    }
    layer->setParent(shared_from_this());
    mapNodeIdToLayer[layer->getId()] = layer;

    if (mapXMLIdToSetLayer.find(layer->getXmlId()) == mapXMLIdToSetLayer.end()) {
        mapXMLIdToSetLayer[layer->getXmlId()] = {};
    }
    mapXMLIdToSetLayer[layer->getXmlId()].insert(layer);

    for (auto& port : layer->getSetInputPort()) insertPort(port);
    for (auto& port : layer->getSetOutputPort()) insertPort(port);

    getIRModel()->sendEventModifyIR();
}

void Layers::insertLayer(std::shared_ptr<Layer> layer, size_t position) {
    insertNodeAtPosition(layer->getXmlElement(), getXmlElement(), position);
    insertLayer(layer);
}

void Layers::insertPort(std::shared_ptr<InputPort> port)
{
    assert(mapPinIdToInputPort.find(port->getId()) == mapPinIdToInputPort.end());
    mapPinIdToInputPort[port->getId()] = port;

    getIRModel()->sendEventModifyIR();
}

void Layers::removePort(std::shared_ptr<InputPort> port)
{
    assert(mapPinIdToInputPort.find(port->getId()) != mapPinIdToInputPort.end());
    mapPinIdToInputPort.erase(port->getId());

    getIRModel()->sendEventModifyIR();
}

void Layers::insertPort(std::shared_ptr<OutputPort> port)
{
    assert(mapPinIdToOutputPort.find(port->getId()) == mapPinIdToOutputPort.end());
    mapPinIdToOutputPort[port->getId()] = port;

    getIRModel()->sendEventModifyIR();
}

void Layers::removePort(std::shared_ptr<OutputPort> port)
{
    assert(mapPinIdToOutputPort.find(port->getId()) != mapPinIdToOutputPort.end());
    mapPinIdToOutputPort.erase(port->getId());

    getIRModel()->sendEventModifyIR();
}

const std::shared_ptr<Layer>& Layers::getLayer(ax::NodeEditor::NodeId id) const
{
    auto it = mapNodeIdToLayer.find(id);
    return (it != mapNodeIdToLayer.end()) ? it->second : emptyLayer;
}

const std::shared_ptr<Layer>& Layers::getLayer(std::string id) const
{
    const auto& set = getSetLayer(id);
    if (set.empty()) return emptyLayer;
    return *(set.begin());
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

    getIRModel()->sendEventModifyIR();
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

const std::shared_ptr<Network>& Layer::getNetwork() const {
    return getLayers()->getParent();
}

const std::shared_ptr<IRModel>& Layer::getIRModel() const {
    return getNetwork()->getParent();
}

const std::shared_ptr<Edges>& Layer::getEdges() const {
    return getNetwork()->getEdges();
}

Layer::Layer(tinyxml2::XMLElement* xmlElement) : id(GetNextId()), xmlElement(xmlElement) {}

tinyxml2::XMLElement* Layer::getXmlInputElement() const
{
    auto xmlElRaw = getXmlElement();
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

    return xmlInputElementRaw;
}

tinyxml2::XMLElement* Layer::getXmlOutputElement() const
{
    auto xmlElRaw = getXmlElement();
    auto xmlOutputElementRaw = xmlElRaw->FirstChildElement("output");
    if (xmlOutputElementRaw == nullptr) {
        xmlOutputElementRaw = xmlElRaw->GetDocument()->NewElement("output");
        auto xmlInputElementRaw = xmlElRaw->FirstChildElement("input");
        if (xmlInputElementRaw == nullptr)
            xmlElRaw->InsertEndChild(xmlOutputElementRaw);
        else xmlElRaw->InsertAfterChild(xmlInputElementRaw, xmlOutputElementRaw);
    }

    return xmlOutputElementRaw;
}

size_t Layer::getXmlPosition() const {
    return getXmlSiblingPosition(getXmlElement());
}

std::set<std::shared_ptr<Edge>, EdgeIDLess> Layer::getSetEdge() const
{
    std::set<std::shared_ptr<Edge>, EdgeIDLess> allEdges = getEdges()->getSetEdgeFromLayer(getId());
    auto incomingEdges = getEdges()->getSetEdgeToLayer(getId());
    allEdges.insert(incomingEdges.begin(), incomingEdges.end());
    
    return allEdges;
}

std::shared_ptr<InputPort> Layer::insertNewInputPort() {
    auto xmlId = std::to_string(getMaxPortXmlId() + 1);
    auto inputPort = std::make_shared<InputPort>(getXmlElement()->GetDocument(), xmlId);
    getXmlInputElement()->InsertEndChild(inputPort->getXmlElement());
    insertPort(inputPort);
    return inputPort;
}

std::shared_ptr<OutputPort> Layer::insertNewOutputPort() {
    auto xmlId = std::to_string(getMaxPortXmlId() + 1);
    auto outputPort = std::make_shared<OutputPort>(getXmlElement()->GetDocument(), xmlId);
    getXmlOutputElement()->InsertEndChild(outputPort->getXmlElement());
    insertPort(outputPort);
    return outputPort;
}

void Layer::insertPort(std::shared_ptr<InputPort> port)
{
    port->setParent(shared_from_this());
    setInputPort.insert(port);
    if (getParent() != nullptr) {
        getParent()->insertPort(port);
    }
}

void Layer::insertPort(std::shared_ptr<OutputPort> port)
{
    port->setParent(shared_from_this());
    setOutputPort.insert(port);
    if (getParent() != nullptr) {
        getParent()->insertPort(port);
    }
}

void Layer::insertPort(std::shared_ptr<InputPort> port, size_t position)
{
    insertNodeAtPosition(port->getXmlElement(), getXmlInputElement(), position);
    port->setParent(shared_from_this());
    insertPort(port);
}

void Layer::insertPort(std::shared_ptr<OutputPort> port, size_t position)
{
    insertNodeAtPosition(port->getXmlElement(), getXmlOutputElement(), position);
    port->setParent(shared_from_this());
    insertPort(port);
}

void Layer::removePort(const std::shared_ptr<InputPort>& port)
{
    setInputPort.erase(port);
    getParent()->removePort(port);
    port->resetParent();

    getIRModel()->sendEventModifyIR();
}

void Layer::deletePort(const std::shared_ptr<InputPort>& port)
{
    removePort(port);
    auto xmlInputElementRaw = getXmlInputElement();
    xmlInputElementRaw->DeleteChild(port->getXmlElement());
    if (xmlInputElementRaw->NoChildren()) {
        xmlInputElementRaw->Parent()->DeleteChild(xmlInputElementRaw);
    }
}

void Layer::removePort(const std::shared_ptr<OutputPort>& port)
{
    setOutputPort.erase(port);
    getParent()->removePort(port);
    port->resetParent();

    getIRModel()->sendEventModifyIR();
}

void Layer::deletePort(const std::shared_ptr<OutputPort>& port)
{
    removePort(port);
    auto xmlOutputElementRaw = getXmlOutputElement();
    xmlOutputElementRaw->DeleteChild(port->getXmlElement());
    if (xmlOutputElementRaw->NoChildren()) {
        xmlOutputElementRaw->Parent()->DeleteChild(xmlOutputElementRaw);
    }
}

std::shared_ptr<InputPort> Layer::getInputPort(std::string xmlId) const
{
    for (auto& port : setInputPort) {
        if (xmlId == std::string(port->getXmlId())) {
            return port;
        }
    }

    return {};
}

std::shared_ptr<OutputPort> Layer::getOutputPort(std::string xmlId) const
{
    for (auto& port : setOutputPort) if (xmlId == std::string(port->getXmlId())) return port;

    return {};
}

std::set<std::shared_ptr<Layer>, LayerIDLess> Layer::getInputLayers() const
{
    auto edges = getEdges();
    auto setToLayersEdges = edges->getSetEdgeToLayer(getId());
    std::set<std::shared_ptr<Layer>, LayerIDLess> setInputLayers;
    for (const auto& edge : setToLayersEdges)
        if (edge->getFromLayer()->getId().Get() != getId().Get())
            setInputLayers.insert(edge->getFromLayer());
    
    return setInputLayers;
}

std::set<std::shared_ptr<Layer>, LayerIDLess> Layer::getOutputLayers() const
{
    auto edges = getEdges();
    auto setFromLayersEdges = edges->getSetEdgeFromLayer(getId());
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

void Layer::changeXmlId(std::string newId) {
    if (newId.empty()) return;
    const auto oldId = std::string(getXmlId());

    if (oldId != newId) {
        getXmlElement()->SetAttribute("id", newId.c_str());
        auto layers = getParent();
        if (layers != nullptr) {
            layers->changeLayerXmlId(shared_from_this(), oldId, newId);
            auto edges = getEdges();
            auto setToLayersEdges = edges->getSetEdgeToLayer(getId());
            for (auto& edge : setToLayersEdges) {
                edge->setAttributes({ { "to-layer", newId} });
            }
            auto setFromLayersEdges = edges->getSetEdgeFromLayer(getId());
            for (auto& edge : setFromLayersEdges) {
                edge->setAttributes({ { "from-layer", newId } });
            }
        }
    }
}

void Layer::setAttributes(std::vector<std::pair<std::string, std::string>> vecAttribute)
{
    std::string changeId;
    for (const auto& [attrName, attrValue] : vecAttribute) {
        if (attrName == "id") { changeId = attrValue; }
    }
    changeXmlId(changeId);
    vecAttribute.erase(std::remove_if(vecAttribute.begin(), vecAttribute.end(), [&](const std::pair<std::string, std::string>& val) {
        return val.first == "id";
        }), vecAttribute.end());

    auto xmlElementRaw = getXmlElement();
    const std::string xmlId = getXmlId();
    deleteAllAttributes(xmlElementRaw);
    getXmlElement()->SetAttribute("id", xmlId.c_str());
    for (const auto& [attrName, attrValue] : vecAttribute) {
        xmlElementRaw->SetAttribute(attrName.c_str(), attrValue.c_str());
    }

    getIRModel()->sendEventModifyIR();
}

const char* Layer::getAttributteValue(std::string attribute) const
{
    return getXmlElement()->Attribute(attribute.c_str());
}

std::shared_ptr<Edge> Edges::insertNewEdge(const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port, size_t xmlPos)
{
    auto inputLayer = getLayers()->getLayer(to_layer);
    assert(inputLayer != nullptr);
    auto outputLayer = getLayers()->getLayer(from_layer);
    assert(outputLayer != nullptr);
    auto inputPort = inputLayer->getInputPort(to_port);
    assert(inputLayer != nullptr);
    auto outputPort = outputLayer->getOutputPort(from_port);
    assert(outputPort != nullptr);


    auto edge = std::make_shared<Edge>(getXmlElement()->GetDocument(), outputPort, inputPort);
    insertNodeAtPosition(edge->getXmlElement(), getXmlElement(), xmlPos);
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
    assert(inputLayer != nullptr);
    auto outputLayer = getLayers()->getLayer(from_layer);
    assert(outputLayer != nullptr);
    auto inputPort = inputLayer->getInputPort(to_port);
    assert(inputPort != nullptr);
    auto outputPort = outputLayer->getOutputPort(from_port);
    assert(outputPort != nullptr);

    auto edge = std::make_shared<Edge>(getXmlElement()->GetDocument(), outputPort, inputPort);
    getXmlElement()->InsertEndChild(edge->getXmlElement());

    insertEdge(edge);
    return edge;
}

void Edges::insertEdge(std::shared_ptr<Edge> edge) {
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

    getIRModel()->sendEventModifyIR();
}

void Edges::insertEdge(std::shared_ptr<Edge> edge, size_t position) {
    insertNodeAtPosition(edge->getXmlElement(), getXmlElement(), position);
    insertEdge(edge);
}

void Edges::deleteEdge(const std::shared_ptr<Edge>& edge) {
    removeEdge(edge);
    getXmlElement()->DeleteChild(edge->getXmlElement());
}

void Edges::removeEdge(const std::shared_ptr<Edge>& edge) {
    mapToLayerIdToSetEdge[edge->getToLayer()->getId()].erase(edge);
    if (mapToLayerIdToSetEdge[edge->getToLayer()->getId()].size() == 0) mapToLayerIdToSetEdge.erase(edge->getToLayer()->getId());

    mapFromLayerIdToSetEdge[edge->getFromLayer()->getId()].erase(edge);
    if (mapFromLayerIdToSetEdge[edge->getFromLayer()->getId()].size() == 0) mapFromLayerIdToSetEdge.erase(edge->getFromLayer()->getId());

    mapLinkIdToEdge.erase(edge->getId());
    edge->resetParent();

    getIRModel()->sendEventModifyIR();
}

std::shared_ptr<Layers> Edges::getLayers() const {
    return parent->getLayers();
}

const std::shared_ptr<IRModel>& Edges::getIRModel() const
{
    return getNetwork()->getParent();
}

const std::set<std::shared_ptr<Edge>, EdgeIDLess>& Edges::getSetEdgeToLayer(ax::NodeEditor::NodeId nodeId) const
{
    auto it = mapToLayerIdToSetEdge.find(nodeId);
    return it != mapToLayerIdToSetEdge.end() ? it->second: emptyEdgeSet;
}

const std::set<std::shared_ptr<Edge>, EdgeIDLess>& Edges::getSetEdgeFromLayer(ax::NodeEditor::NodeId nodeId) const
{
    auto it = mapFromLayerIdToSetEdge.find(nodeId);
    return it != mapFromLayerIdToSetEdge.end() ? it->second : emptyEdgeSet;
}

const std::shared_ptr<Edge>& Edges::getEdge(std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort) const
{
    auto setLayerEdges = getSetEdgeFromLayer(outputPort->getParent()->getId());
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
    xmlElement = xmlDocument->NewElement("edge");
    xmlElement->SetAttribute("from-layer", outputPort->getParent()->getXmlId());
    xmlElement->SetAttribute("from-port", outputPort->getXmlId());
    xmlElement->SetAttribute("to-layer", inputPort->getParent()->getXmlId());
    xmlElement->SetAttribute("to-port", inputPort->getXmlId());
}

Edge::Edge(std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort, tinyxml2::XMLElement* xmlElement, std::shared_ptr<Edges> parent) : id(GetNextId()), outputPort(outputPort), inputPort(inputPort), xmlElement(xmlElement), parent(parent) {}

const std::shared_ptr<Network>& Edge::getNetwork() const {
    return parent->getParent();
}

const std::shared_ptr<IRModel>& Edge::getIRModel() const {
    return getNetwork()->getParent();
}

size_t Edge::getXmlPosition() const {
    return getXmlSiblingPosition(getXmlElement());
}

void Edge::setAttributes(std::map<std::string, std::string> mapAttribute) {
    const auto parent = getParent();
    parent->removeEdge(shared_from_this());

    std::set<std::string> setAllowedAttribute = { "from-layer", "from-port", "to-layer", "to-port" };

    auto xmlElementRaw = getXmlElement();
    for (const auto& [attrName, attrValue] : mapAttribute) {
        assert(setAllowedAttribute.find(attrName) != setAllowedAttribute.end());
        xmlElementRaw->SetAttribute(attrName.c_str(), attrValue.c_str());
    }

    auto from_layer = xmlElementRaw->Attribute("from-layer");
    auto from_port = xmlElementRaw->Attribute("from-port");
    auto to_layer = xmlElementRaw->Attribute("to-layer");
    auto to_port = xmlElementRaw->Attribute("to-port");

    auto inputLayer = parent->getLayers()->getLayer(to_layer);
    assert(inputLayer != nullptr);
    auto outputLayer = parent->getLayers()->getLayer(from_layer);
    assert(outputLayer != nullptr);
    inputPort = inputLayer->getInputPort(to_port);
    assert(inputPort != nullptr);
    outputPort = outputLayer->getOutputPort(from_port);
    assert(outputPort != nullptr);

    parent->insertEdge(shared_from_this());
}

Port::Port(tinyxml2::XMLDocument* xmlDocument, std::string xmlId) : id(GetNextId()) {
    xmlElement = xmlDocument->NewElement("port");
    xmlElement->SetAttribute("id", xmlId.c_str());
}

Port::Port(tinyxml2::XMLElement* xmlElement, std::shared_ptr<Layer> parent) : id(GetNextId()), xmlElement(xmlElement), parent(parent) {}

const std::shared_ptr<Layers>& Port::getLayers() const {
    return getParent()->getParent();
}

const std::shared_ptr<Network>& Port::getNetwork() const {
    return getLayers()->getParent();
}

const std::shared_ptr<Edges>& Port::getEdges() const {
    return getNetwork()->getEdges();
}

const std::shared_ptr<IRModel>& Port::getIRModel() const {
    return getNetwork()->getParent();
}

size_t Port::getXmlPosition() const {
    return getXmlSiblingPosition(getXmlElement());
}

std::vector<std::string> Port::getVecDim()
{
    std::vector<std::string> vecDim;
    auto xmlPort = getXmlElement();
    for (auto child = xmlPort->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        if (child->Value() != std::string("dim")) continue;
        vecDim.push_back(child->GetText());
    }
    return vecDim;
}

void Port::setVecDim(std::vector<std::string> vecDim, std::vector<size_t> vecDimPos)
{
    auto xmlElement = getXmlElement();
    for (auto* child = xmlElement->FirstChildElement();
        child != nullptr;
        ) {
        if (child->Value() == std::string("dim")) {
            auto nextNode = child->NextSiblingElement();
            xmlElement->DeleteChild(child);
            child = nextNode;
        }
        else {
            child = child->NextSiblingElement();
        }
    }

    size_t dimPos = 0;
    for (size_t dimNr = 0; dimNr < vecDim.size(); ++dimNr) {
        dimPos = dimNr < vecDimPos.size() ? vecDimPos[dimNr] : ++dimPos;
        auto xmlDim = xmlElement->GetDocument()->NewElement("dim");
        xmlDim->SetText(vecDim[dimNr].c_str());
        insertNodeAtPosition(xmlDim, xmlElement, dimPos);
    }

    getIRModel()->sendEventModifyIR();
}

void Port::setAttributes(std::vector<std::pair<std::string, std::string>> vecAttribute) {
    const auto oldId = std::string(getXmlId());
    auto xmlElementRaw = getXmlElement();
    deleteAllAttributes(xmlElementRaw);
    for (const auto& [attrName, attrValue] : vecAttribute) {
        xmlElementRaw->SetAttribute(attrName.c_str(), attrValue.c_str());
    }

    const auto newId = std::string(getXmlId());
    if (oldId != newId) {
        auto setEdge = getSetEdge();
        for (auto& edge : setEdge) {
            modifyEdgeAttributesAfterIdChange(edge);
        }
    }

    getIRModel()->sendEventModifyIR();
}

bool EdgeIDLess::operator()(const std::shared_ptr<Edge>& lhs, const std::shared_ptr<Edge>& rhs) const {
    return LinkIdLess()(lhs->getId(), rhs->getId());
}

std::set<std::shared_ptr<Edge>, EdgeIDLess> InputPort::getSetEdge() const {
    auto edges = getEdges()->getSetEdgeToLayer(getParent()->getId());

    auto it = edges.begin();
    while (it != edges.end()) {
        if ((*it)->getInputPort().get() != this) {
            it = edges.erase(it);
        } else { ++it; }
    }
    return edges;
}

void InputPort::modifyEdgeAttributesAfterIdChange(std::shared_ptr<Edge> edge)
{
    edge->setAttributes({ { "to-port", getXmlId()}});
}

std::set<std::shared_ptr<Edge>, EdgeIDLess> OutputPort::getSetEdge() const {
    auto edges = getEdges()->getSetEdgeFromLayer(getParent()->getId());

    auto it = edges.begin();
    while (it != edges.end()) {
        if ((*it)->getOutputPort().get() != this) {
            it = edges.erase(it);
        }
        else { ++it; }
    }
    return edges;
}

void OutputPort::modifyEdgeAttributesAfterIdChange(std::shared_ptr<Edge> edge) {
    edge->setAttributes({ { "from-port", getXmlId()} });
}

static std::vector<std::string> nonConnectedPorts(std::shared_ptr<Layers> layers, std::shared_ptr<Edges> edges) {
    std::vector<std::string> vecWarningMsg;
    for (const auto& layer : *layers) {
        const auto setInputPort = layer->getSetInputPort();
        if (setInputPort.size()) {
            const auto setEdgesToLayer = edges->getSetEdgeToLayer(layer->getId());
            for (const auto& port : setInputPort) {
                if (std::find_if(setEdgesToLayer.begin(), setEdgesToLayer.end(), [&](const auto& edge) { return edge->getInputPort() == port; }) == setEdgesToLayer.end()) {
                    vecWarningMsg.push_back(std::string("Input Port (Layer id: ") + layer->getXmlId() + " port id: " + port->getXmlId() + ") is not connected to a edge. Line: " + std::to_string(port->getXmlElement()->GetLineNum()));
                }
            }
        }
        const auto setOutputPort = layer->getSetOutputPort();
        if (setOutputPort.size()) {
            const auto setEdgesFromLayer = edges->getSetEdgeFromLayer(layer->getId());
            for (const auto& port : setOutputPort) {
                if (std::find_if(setEdgesFromLayer.begin(), setEdgesFromLayer.end(), [&](const auto& edge) { return edge->getOutputPort() == port; }) == setEdgesFromLayer.end()) {
                    vecWarningMsg.push_back(std::string("Output Port (Layer id: ") + layer->getXmlId() + " port id: " + port->getXmlId() + ") is not connected to a edge. Line: " + std::to_string(port->getXmlElement()->GetLineNum()));
                }
            }
        }
    }

    return vecWarningMsg;
}

static std::vector<std::string> validateLayers(tinyxml2::XMLElement* xmlLayers) {
    std::vector<std::string> vecWarningMsg;
    std::set<std::string> setXMLLayerID;

    tinyxml2::XMLElement* xmlLayer = xmlLayers->FirstChildElement("layer");
    for (; xmlLayer != nullptr; xmlLayer = xmlLayer->NextSiblingElement()) {
        const auto id = xmlLayer->Attribute("id");
        if (id == nullptr) {
            vecWarningMsg.push_back("Layer without attribute: 'id' found on line: " + std::to_string(xmlLayer->GetLineNum()));
            continue;
        }
        if (setXMLLayerID.insert(id).second == false) {
            vecWarningMsg.push_back("Layer with duplicate id: " + std::string(id) + " found on line: " + std::to_string(xmlLayer->GetLineNum()));
            continue;
        }
        const auto name = xmlLayer->Attribute("name");
        if (name == nullptr) {
            vecWarningMsg.push_back("Layer id: " + std::string(id) + " without attribute: 'name' found on line: " + std::to_string(xmlLayer->GetLineNum()));
            continue;
        }
        const auto type = xmlLayer->Attribute("type");
        if (type == nullptr) {
            vecWarningMsg.push_back("Layer id " + std::string(id) + " without attribute: 'type' found on line : " + std::to_string(xmlLayer->GetLineNum()));
            continue;
        }
    }
    return vecWarningMsg;
}

static std::vector<std::string> validateEdges(tinyxml2::XMLElement* xmlEdges) {
    std::vector<std::string> vecWarningMsg;

    struct XmlEdgeRaw {
        XmlEdgeRaw(tinyxml2::XMLElement* xmlElement) : xmlElement(xmlElement) {
            fromLayer = xmlElement->Attribute("from-layer");
            fromPort = xmlElement->Attribute("from-port");
            toLayer = xmlElement->Attribute("to-layer");
            toPort = xmlElement->Attribute("to-port");
        };
        tinyxml2::XMLElement* xmlElement;
        std::string fromLayer;
        std::string fromPort;
        std::string toLayer;
        std::string toPort;
    };

    struct XmlEdgeRawLess
    {
        bool operator()(const XmlEdgeRaw& lhs, const XmlEdgeRaw& rhs) const
        {
            if (lhs.fromLayer != rhs.fromLayer) return std::string(lhs.fromLayer) < std::string(rhs.fromLayer);
            if (lhs.fromPort != rhs.fromPort) return std::string(lhs.fromPort) < std::string(rhs.fromPort);
            if (lhs.toLayer != rhs.toLayer) return std::string(lhs.toLayer) < std::string(rhs.toLayer);
            return std::string(lhs.toPort) < std::string(rhs.toPort);
        }
    };

    std::set<XmlEdgeRaw, XmlEdgeRawLess> setXmlEdge;

    tinyxml2::XMLElement* xmlEdge = xmlEdges->FirstChildElement("edge");
    for (; xmlEdge != nullptr; xmlEdge = xmlEdge->NextSiblingElement()) {
        auto from_layer = xmlEdge->Attribute("from-layer");
        if (from_layer == nullptr) {
            vecWarningMsg.push_back("Edge without attribute: 'from-layer' found on line: " + std::to_string(xmlEdge->GetLineNum()));
            continue;
        }

        auto from_port = xmlEdge->Attribute("from-port");
        if (from_port == nullptr) {
            vecWarningMsg.push_back("Edge without attribute: 'from-port' found on line: " + std::to_string(xmlEdge->GetLineNum()));
            continue;
        }

        auto to_layer = xmlEdge->Attribute("to-layer");
        if (to_layer == nullptr) {
            vecWarningMsg.push_back("Edge without attribute: 'to-layer' found on line: " + std::to_string(xmlEdge->GetLineNum()));
            continue;
        }

        auto to_port = xmlEdge->Attribute("to-port");
        if (to_port == nullptr) {
            vecWarningMsg.push_back("Edge without attribute: 'to-port' found on line: " + std::to_string(xmlEdge->GetLineNum()));
            continue;
        }
        auto xmlEdgeRaw = XmlEdgeRaw(xmlEdge);
        if (setXmlEdge.insert(xmlEdgeRaw).second == false) {
            vecWarningMsg.push_back(std::string("Duplicated edge ") + xmlEdgeRaw.fromLayer + ":" + xmlEdgeRaw.fromPort + "->" + xmlEdgeRaw.toLayer + ":" + xmlEdgeRaw.toPort + " found on line: " + std::to_string(xmlEdge->GetLineNum()));
        }
    }

    return vecWarningMsg;
}

static std::vector<std::string> validateEdgeDimensions(const std::shared_ptr<Edges>& edges) {
    std::vector<std::string> vecWarningMsg;

    for (const auto& edge : *edges) {
        const auto& inputPort = edge->getInputPort(); //SeNe remvoe
        const auto& outputPort = edge->getOutputPort(); //SeNe remvoe
        const auto inputPortDims = edge->getInputPort()->getVecDim();
        const auto outputPortDims = edge->getOutputPort()->getVecDim();
        if (inputPortDims != outputPortDims) {
            vecWarningMsg.push_back(std::string("Edge ") + outputPort->getParent()->getXmlId() + ":" + outputPort->getXmlId() + "->" + inputPort->getParent()->getXmlId() + ":" + inputPort->getXmlId() + " is connecting two ports of different dimensions. Line: " + std::to_string(edge->getXmlElement()->GetLineNum()));
        }
    }

    return vecWarningMsg;
}

std::vector<std::string> IRModel::validate()
{
    std::vector<std::string> vecWarningMsg;
    auto concat = [](std::vector<std::string>& dest, const std::vector<std::string>& src) { dest.insert(dest.end(), src.begin(), src.end()); };
    concat(vecWarningMsg, nonConnectedPorts(getNetwork()->getLayers(), getNetwork()->getEdges()));
    concat(vecWarningMsg, validateLayers(getNetwork()->getLayers()->getXmlElement()));
    concat(vecWarningMsg, validateEdges(getNetwork()->getEdges()->getXmlElement()));
    concat(vecWarningMsg, validateEdgeDimensions(getNetwork()->getEdges()));
    return vecWarningMsg;
}

template <typename T>
bool are_functions_equal(const std::function<T>& f1, const std::function<T>& f2) {
    return f1.target_type() == f2.target_type() && f1.target<T>() == f2.target<T>();
}

bool IRModel::registerHandlerModifyIR(std::function<void(void)> handler) {
    const auto it = std::find_if(vecModifyIRHandler.begin(), vecModifyIRHandler.end(), [&handler](const auto& f) { return are_functions_equal(f, handler); });
    if (it != vecModifyIRHandler.end()) return false;
    vecModifyIRHandler.push_back(handler);
    return true;
}

bool IRModel::unregisterHandlerModifyIR(std::function<void(void)> handler) {
    const auto it = std::find_if(vecModifyIRHandler.begin(), vecModifyIRHandler.end(), [&handler](const auto& f) { return are_functions_equal(f, handler); });
    if (it == vecModifyIRHandler.end()) return false;
    vecModifyIRHandler.erase(it);
    return true;
}
