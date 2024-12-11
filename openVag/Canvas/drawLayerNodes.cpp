#include "drawLayerNodes.h"
#include <string>

#include "../OpenVag.h"

namespace Canvas {
    void drawLayerNode(std::shared_ptr<Layer> layer) {
        ax::NodeEditor::BeginNode(layer->getId());
        for (const auto& inputPort : layer->getSetInputPort()) {
            ax::NodeEditor::BeginPin(inputPort->getId(), ax::NodeEditor::PinKind::Input);
            ImGui::Text(inputPort->getXmlId());
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
    }

    void drawLayerNodes(std::shared_ptr<Layers> layers) {
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 0.6f));
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -0.6f));
        for (auto& layer : (*layers)) {
            drawLayerNode(layer);
        }
        ax::NodeEditor::PopStyleVar(2);
    }
}