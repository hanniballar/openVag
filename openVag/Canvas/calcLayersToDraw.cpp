#include "calcLayersToDraw.h"
#include <functional>

template <typename T, typename Predicate>
void remove_if(std::unordered_set<T>& set, Predicate pred) {
    for (auto it = set.begin(); it != set.end(); ) {
        if (pred(*it)) { it = set.erase(it); }
        else { ++it; }
    }
}

static std::unordered_set<std::shared_ptr<Layer>> getSetSelectedLayers(const std::shared_ptr<Layers>& layers) {
    auto selectedObjectCount = ax::NodeEditor::GetSelectedObjectCount();
    std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
    vecSelectedNodeId.resize(selectedObjectCount);
    int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
    vecSelectedNodeId.resize(nodeCount);
    std::unordered_set<std::shared_ptr<Layer>> setSelectedLayers;
    for (const auto& nodeId : vecSelectedNodeId) {
        const auto layer = layers->getLayer(nodeId);
        if (layer == nullptr) continue;
        setSelectedLayers.insert(layer);
    }

    return setSelectedLayers;
}

static std::unordered_set<std::shared_ptr<Layer>> getSetLayersConnectedBySelectedEdges(const std::shared_ptr<Edges>& edges) {
    auto selectedObjectCount = ax::NodeEditor::GetSelectedObjectCount();
    std::vector<ax::NodeEditor::LinkId> vecSelectedLinkId;
    vecSelectedLinkId.resize(selectedObjectCount);
    int linkCount = ax::NodeEditor::GetSelectedLinks(vecSelectedLinkId.data(), static_cast<int>(vecSelectedLinkId.size()));
    vecSelectedLinkId.resize(linkCount);
    std::unordered_set<std::shared_ptr<Layer>> setLayersConnectedBySelectedEdges;
    for (const auto& linkid : vecSelectedLinkId) {
        auto edge = edges->getEdge(linkid);
        if (edge == nullptr) continue;
        setLayersConnectedBySelectedEdges.insert(edge->getFromLayer());
        setLayersConnectedBySelectedEdges.insert(edge->getToLayer());
    }

    return setLayersConnectedBySelectedEdges;
}

static bool isConstNodeVisible(const std::shared_ptr<Layer>& layer, const std::unordered_set<std::shared_ptr<Layer>>& setSelectedLayers, std::unordered_set<std::shared_ptr<Layer>> setLayersConnectedBySelectedEdges) {
    if (std::string(layer->getType()) != "Const") return true;
    if (setLayersConnectedBySelectedEdges.find(layer) != setLayersConnectedBySelectedEdges.end()) return true;
    if (setSelectedLayers.find(layer) != setSelectedLayers.end()) return true;
    const auto outputLayers = layer->getOutputLayers();
    for (const auto& outlayer : outputLayers) {
        if (setSelectedLayers.find(outlayer) != setSelectedLayers.end()) return true;
    }
    if (outputLayers.empty()) return true;
    return false;
}

static std::unordered_set<std::shared_ptr<Layer>> getLayersInCanvas(const std::shared_ptr<Layers>& layers) {
    const auto vecNodeIdInCanvas = ax::NodeEditor::FindNodesInRect(ax::NodeEditor::GetCanvasView(), true);
    std::unordered_set<std::shared_ptr<Layer>> vecLayerInCanvas;
    for (const auto nodeId : vecNodeIdInCanvas) {
        const auto layer = layers->getLayer(nodeId);
        if (layer == nullptr) continue;
        vecLayerInCanvas.insert(layer);
    }

    return vecLayerInCanvas;
}

static bool isLayerInCanvas(const std::shared_ptr<Layer>& layer) {
    auto pos = ax::NodeEditor::GetNodePosition(layer->getId());
    auto size = ax::NodeEditor::GetNodeSize(layer->getId());
    ImRect layerImRect{ pos.x, pos.y, pos.x + size.x, pos.y + size.y };
    auto canvasView = ax::NodeEditor::GetCanvasView();
    bool res = layerImRect.Overlaps(canvasView);
    return layerImRect.Overlaps(canvasView);
}

static bool isLayerConnectedToLayerInCanvas(std::shared_ptr<Layer> layer, const std::unordered_set<std::shared_ptr<Layer>>& setLayerInCanvas) {
    if (setLayerInCanvas.find(layer) != setLayerInCanvas.end()) return true;
    for (const auto& outLayer : layer->getOutputLayers()) {
        if (setLayerInCanvas.find(outLayer) != setLayerInCanvas.end()) return true;
    }
    for (const auto& inLayer : layer->getInputLayers()) {
        if (setLayerInCanvas.find(inLayer) != setLayerInCanvas.end()) return true;
    }

    return false;
}

std::unordered_set<std::shared_ptr<Layer>> calcLayersToDraw(const std::shared_ptr<Layers>& layers, bool forceDrawAllNodes) {
    if (forceDrawAllNodes) return std::unordered_set<std::shared_ptr<Layer>>(layers->begin(), layers->end());
    std::unordered_set<std::shared_ptr<Layer>> setLayerToDraw;

    auto setSelectedLayers = getSetSelectedLayers(layers);
    auto setLayersConnectedBySelectedEdges = getSetLayersConnectedBySelectedEdges(layers->getNetwork()->getEdges());
    std::unordered_set<std::shared_ptr<Layer>> setVisibleLayersInCanvas;
    for (const auto& layer : *layers) {
        if (isConstNodeVisible(layer, setSelectedLayers, setLayersConnectedBySelectedEdges) == false) continue;
        setLayerToDraw.insert(layer);
    }
    std::unordered_set<std::shared_ptr<Layer>> setLayerInCanvas = getLayersInCanvas(layers);

    remove_if(setLayerToDraw, [&](const std::shared_ptr<Layer>& layer) {
        return isLayerConnectedToLayerInCanvas(layer, setLayerInCanvas) == false;
        });

    setLayerToDraw.insert(setSelectedLayers.begin(), setSelectedLayers.end());
    setLayerToDraw.insert(setLayersConnectedBySelectedEdges.begin(), setLayersConnectedBySelectedEdges.end());

    return setLayerToDraw;
}

std::unordered_set<std::shared_ptr<Edge>> calcEdgesToDraw(const std::unordered_set<std::shared_ptr<Layer>>& setLayer) {
    std::unordered_set<std::shared_ptr<Edge>> setEdge;
    for (const auto& layer : setLayer) {
        const auto layerEdges = layer->getSetEdge();
        setEdge.insert(layerEdges.begin(), layerEdges.end());
    }

    //Remove Edges to layers that are not drawn
    remove_if(setEdge, [&](const std::shared_ptr<Edge>& edge) {
        return setLayer.find(edge->getFromLayer()) == setLayer.end()
            || setLayer.find(edge->getToLayer()) == setLayer.end();
        });

    return setEdge;
}
