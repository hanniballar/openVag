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
static std::shared_ptr<OutputPort> emptyOutputPort;
static std::shared_ptr<InputPort> emptyInputPort;

tinyxml2::XMLElement* Layers::createXmlLayer(std::string xmlId, std::string name, std::string type) const
{
    auto xmlLayer = getXmlElement()->el->GetDocument()->NewElement("layer");
    xmlLayer->SetAttribute("id", xmlId.c_str());
    xmlLayer->SetAttribute("name", name.c_str());
    xmlLayer->SetAttribute("type", type.c_str());

    return xmlLayer;
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

std::shared_ptr<Layer> Layers::insertNewLayer()
{
    auto xmlId = std::to_string(getMaxLayerXmlId() + 1);
    std::string name = std::string("openVagLayer") + xmlId;
    ax::NodeEditor::NodeId id = GetNextId();
    
    return insertNewLayer(id, xmlId, name, std::string("Const"));
}

std::shared_ptr<Layer> Layers::insertNewLayer(ax::NodeEditor::NodeId id, std::string xmlId, std::string name, std::string type)
{
    auto xmlLayer = createXmlLayer(xmlId, name, type);
    xmlElement->el->InsertEndChild(xmlLayer);

    auto layer = std::make_shared<Layer>(id, xmlLayer, getParent()->getLayers());
    assert(mapNodeIdToLayer.find(id) == mapNodeIdToLayer.end());
    mapNodeIdToLayer[id] = layer;

    assert(mapXMLIdToSetLayer.find(xmlId) == mapXMLIdToSetLayer.end());
    mapXMLIdToSetLayer[xmlId].insert(layer);

    return layer;
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

}

void Layers::addLayer(std::shared_ptr<Layer> layer)
{
    assert(mapNodeIdToLayer.find(layer->getId()) == mapNodeIdToLayer.end());
    mapNodeIdToLayer[layer->getId()] = layer;

    if (mapXMLIdToSetLayer.find(layer->getXmlId()) == mapXMLIdToSetLayer.end()) {
        mapXMLIdToSetLayer[layer->getXmlId()] = {};
    }
    mapXMLIdToSetLayer[layer->getXmlId()].insert(layer);

    for (auto& port : layer->getSetInputPort()) addPort(port);
    for (auto& port : layer->getSetOutputPort()) addPort(port);
}

void Layers::addPort(std::shared_ptr<InputPort> port)
{
    assert(mapPinIdToInputPort.find(port->getId()) == mapPinIdToInputPort.end());
    mapPinIdToInputPort[port->getId()] = port;
}

void Layers::removePort(std::shared_ptr<InputPort> port)
{
    assert(mapPinIdToInputPort.find(port->getId()) != mapPinIdToInputPort.end());
    mapPinIdToInputPort.erase(port->getId());
}

void Layers::addPort(std::shared_ptr<OutputPort> port)
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

std::shared_ptr<Network> Layer::getNetwork() {
    return parent->getParent();
}

std::shared_ptr<Edges> Layer::getEdges()
{
    return getNetwork()->getEdges();
}

void Layer::addPort(std::shared_ptr<InputPort> port)
{
    setInputPort.insert(port);
    getParent()->addPort(port);
}

void Layer::addPort(std::shared_ptr<OutputPort> port)
{
    setOutputPort.insert(port);
    getParent()->addPort(port);
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
    auto setToLayersEdges = edges->getSetEdgesToLayers(getId());
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

tinyxml2::XMLElement* Edges::createXmlEdge(std::string from_layer, std::string from_port, std::string to_layer, std::string to_port)
{
    auto xmlEdge = getXmlElement()->el->GetDocument()->NewElement("edge");
    xmlEdge->SetAttribute("from-layer", from_layer.c_str());
    xmlEdge->SetAttribute("from-port", from_port.c_str());
    xmlEdge->SetAttribute("to-layer", to_layer.c_str());
    xmlEdge->SetAttribute("to-port", to_port.c_str());

    return xmlEdge;
}

std::shared_ptr<Edge> Edges::createEdge(ax::NodeEditor::LinkId id_gui, std::string from_layer, std::string from_port, std::string to_layer, std::string to_port, tinyxml2::XMLElement* xmlElement)
{
    auto inputLayer = getLayers()->getLayer(to_layer);
    auto outputLayer = getLayers()->getLayer(from_layer);
    auto inputPort = inputLayer->getInputPort(to_port);
    auto outputPort = outputLayer->getOutputPort(from_port);

    assert(outputPort != nullptr);
    assert(inputPort != nullptr);

    return std::make_shared<Edge>(id_gui, outputPort, inputPort, xmlElement, getParent()->getEdges());
}

std::shared_ptr<Edge> Edges::insertNewEdge(ax::NodeEditor::LinkId id_gui, const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port, size_t xmlPos)
{
    auto xmlEdge = createXmlEdge(from_layer, from_port, to_layer, to_port);

    auto xmlEdges = xmlElement->el;
    if (xmlPos > 0) {
        auto child = xmlEdges->FirstChild();
        for (size_t curPos = 0; curPos < xmlPos - 1; ++curPos) {
            assert(child != nullptr);
            child = child->NextSibling();
        }
        xmlEdges->InsertAfterChild(child, xmlElement->el);
    }
    else {
        xmlEdges->InsertFirstChild(xmlElement->el);
    }

    auto edge = createEdge(id_gui, from_layer, from_port, to_layer, to_port, xmlEdge);
    insertEdge(edge);

    return edge;
}

std::shared_ptr<Edge> Edges::insertNewEdge(ax::NodeEditor::LinkId id_gui, const std::string& from_layer, const std::string& from_port, const std::string& to_layer, const std::string& to_port)
{
    assert (std::find_if(begin(), end(), [&](std::shared_ptr<Edge>& edge) {
        auto res = from_layer == edge->getOutputPort()->getParent()->getXmlId()
            && from_port == edge->getOutputPort()->getXmlId()
            && to_layer == edge->getInputPort()->getParent()->getXmlId()
            && to_port == edge->getInputPort()->getXmlId();
        return res;
        }) == end());

    auto xmlEdge = createXmlEdge(from_layer, from_port, to_layer, to_port);

    xmlElement->el->InsertEndChild(xmlEdge);

    auto edge = createEdge(id_gui, from_layer, from_port, to_layer, to_port, xmlEdge);
    insertEdge(edge);

    return edge;
}

void Edges::insertEdge(std::shared_ptr<Edge> edge)
{
    assert(mapLinkIdToEdge.find(edge->getId()) == mapLinkIdToEdge.end());
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
}

std::shared_ptr<Layers> Edges::getLayers() {
    return parent->getLayers();
}

const std::set<std::shared_ptr<Edge>, EdgeIDLess>& Edges::getSetEdgesToLayers(ax::NodeEditor::NodeId nodeId) const
{
    auto it = mapToLayerIdToSetEdge.find(nodeId);
    return it != mapToLayerIdToSetEdge.end() ? it->second: emptyEdgeSet;
}

const std::set<std::shared_ptr<Edge>, EdgeIDLess>& Edges::getSetEdgesFromLayer(ax::NodeEditor::NodeId nodeId) const
{
    auto it = mapFromLayerIdToSetEdge.find(nodeId);
    return it != mapFromLayerIdToSetEdge.end() ? it->second : emptyEdgeSet;
}

std::shared_ptr<Edge> Edges::getEdge(std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort) const
{
    auto setLayerEdges = getSetEdgesFromLayer(outputPort->getParent()->getId());
    auto it = std::find_if(setLayerEdges.begin(), setLayerEdges.end(), [&](std::shared_ptr<Edge> edge) { return edge->getInputPort() == inputPort; });
    return (it != setLayerEdges.end()) ? *it : nullptr;
}