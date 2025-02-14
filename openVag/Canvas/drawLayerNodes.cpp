#include "drawLayerNodes.h"

#include <string>
#include <unordered_set>

#include "../OpenVag.h"

namespace Canvas {
    void drawLayerNode(const std::shared_ptr<Layer>& layer, const std::map<std::string, ImColor>& mapLayerTypeToColor) {
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 0.6f));
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -0.6f));
        ImColor nodeBgColor = mapLayerTypeToColor.find(layer->getType()) != mapLayerTypeToColor.end() ? mapLayerTypeToColor.at(layer->getType())
            : ImColor(0.12549f, 0.12549f, 0.12549f, 0.784314f);
        ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, nodeBgColor);
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
        ax::NodeEditor::PopStyleColor();
        ax::NodeEditor::PopStyleVar(2);
    }

    void drawLayerNodes(const std::unordered_set<std::shared_ptr<Layer>>& setLayer, const std::map<std::string, ImColor>& mapLayerTypeToColor) {
        for (auto& layer : setLayer) {
            drawLayerNode(layer, mapLayerTypeToColor);
        }
    }
}