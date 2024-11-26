#include "drawLayerNodes.h"
#include <string>

#include "../OpenVag.h"

void drawLayerNode(std::shared_ptr<Layer> layer) {
    ax::NodeEditor::BeginNode(layer->getId());
    for (const auto& inputPort : layer->getSetInputPort()) {
        ax::NodeEditor::BeginPin(inputPort->getId(), ax::NodeEditor::PinKind::Input);
            ImGui::Text(inputPort->getXmlId());
        ax::NodeEditor::EndPin();
        if (inputPort != *(layer->getSetInputPort().rbegin())) {
            ImGui::SameLine();
        }
    }
    ImGui::Text((std::string("Name: ") + layer->getName()).c_str());
    ImGui::Text((std::string("ID: ") + layer->getXmlId()).c_str());
    ImGui::Text((std::string("Type: ") + layer->getType()).c_str());

    for (const auto& outputPort : layer->getSetOutputPort()) {
        ax::NodeEditor::BeginPin(outputPort->getId(), ax::NodeEditor::PinKind::Input);
        ImGui::Text(outputPort->getXmlId());
        ax::NodeEditor::EndPin();
        if (outputPort != *(layer->getSetOutputPort().rbegin())) {
            ImGui::SameLine();
        }
    }
    ax::NodeEditor::EndNode();
}

void drawLayerNodes(std::shared_ptr<Layers> layers) {
    for (auto& layer : (*layers)) {
        drawLayerNode(layer);
    }
}