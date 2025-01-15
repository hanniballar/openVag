#include "drawLayerNodes.h"

#include <string>
#include <unordered_set>

#include "../OpenVag.h"

namespace Canvas {
    void drawLayerNode(const std::shared_ptr<Layer>& layer) {
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 0.6f));
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -0.6f));
        ax::NodeEditor::BeginNode(layer->getId());
        for (const auto& inputPort : layer->getSetInputPort()) {
            ax::NodeEditor::BeginPin(inputPort->getId(), ax::NodeEditor::PinKind::Input);
            auto isConnectedToConstLayer = [&]() {
                for (const auto edge : inputPort->getSetEdge()) {
                    if (std::string(edge->getOutputPort()->getParent()->getType()) == "Const") return true;
                }
                return false;
                };
            if (isConnectedToConstLayer()) { 
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), inputPort->getXmlId()); }
            else { 
                ImGui::Text(inputPort->getXmlId()); }
            ax::NodeEditor::EndPin();
            if (inputPort != *(layer->getSetInputPort().rbegin())) { ImGui::SameLine(); }
        }
        ImGui::Text((std::string("Name: ") + layer->getName()).c_str());
        ImGui::Text((std::string("ID: ") + layer->getXmlId()).c_str());
#ifndef NDEBUG
        ImGui::SameLine();
        ImGui::Text((std::string("Gui ID: ") + std::to_string(layer->getId().Get())).c_str());
#endif // !NDEBUG
        ImGui::Text((std::string("Type: ") + layer->getType()).c_str());

        for (const auto& outputPort : layer->getSetOutputPort()) {
            ax::NodeEditor::BeginPin(outputPort->getId(), ax::NodeEditor::PinKind::Output);
            ImGui::Text(outputPort->getXmlId());
            ax::NodeEditor::EndPin();
            if (outputPort != *(layer->getSetOutputPort().rbegin())) {
                ImGui::SameLine();
            }
        }
        ax::NodeEditor::EndNode();
        ax::NodeEditor::PopStyleVar(2);
    }

    static bool skipLayer(const std::shared_ptr<Layer>& layer, const std::unordered_set<std::shared_ptr<Layer>>& setSelectedLayers, std::unordered_set<std::shared_ptr<Layer>> setLayersConnectedBySelectedEdges) {
        if (std::string(layer->getType()) != "Const") return false;
        if (setLayersConnectedBySelectedEdges.find(layer) != setLayersConnectedBySelectedEdges.end()) return false;
        if (setSelectedLayers.find(layer) != setSelectedLayers.end()) return false;
        const auto inputLayers = layer->getInputLayers();
        for (const auto& layer : inputLayers) {
            if (setSelectedLayers.find(layer) != setSelectedLayers.end()) return false;
        }
        const auto outputLayers = layer->getOutputLayers();
        for (const auto& layer : outputLayers) {
            if (setSelectedLayers.find(layer) != setSelectedLayers.end()) return false;
        }
        if (inputLayers.empty() && outputLayers.empty()) return false;        
        return true;
    }

    std::unordered_set<std::shared_ptr<Layer>> getSetSelectedLayers(const std::shared_ptr<Layers>& layers) {
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

    std::unordered_set<std::shared_ptr<Layer>> getSetLayersConnectedBySelectedEdges(const std::shared_ptr<Edges>& edges) {
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

    void drawLayerNodes(const std::shared_ptr<Layers>& layers, bool forceDrawAllNodes) {

        auto setSelectedLayers = getSetSelectedLayers(layers);
        auto setLayersConnectedBySelectedEdges = getSetLayersConnectedBySelectedEdges(layers->getNetwork()->getEdges());
        for (auto& layer : (*layers)) {
            if (forceDrawAllNodes == false && skipLayer(layer, setSelectedLayers, setLayersConnectedBySelectedEdges) == true) continue;
            drawLayerNode(layer);
        }
    }
}