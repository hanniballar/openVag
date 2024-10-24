#include "drawLayerNode.h"
#include <memory>
#include <string>

#include "OpenVag.h"

void drawLayerNode(LayerNodeGui layerNodeGui) {
    ax::NodeEditor::BeginNode(layerNodeGui.id_gui);
    for (const auto& inputPort : layerNodeGui.vecInputPort) {
        ax::NodeEditor::BeginPin(inputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
            ImGui::Text(inputPort->getXmlId());
        ax::NodeEditor::EndPin();
        if (&inputPort != &(layerNodeGui.vecInputPort.back())) {
            ImGui::SameLine();
        }
    }

    ImGui::Text((std::string("Name: ") + layerNodeGui.getName()).c_str());
    ImGui::Text((std::string("ID: ") + layerNodeGui.getXmlId()).c_str());
    ImGui::Text((std::string("Type: ") + layerNodeGui.getType()).c_str());

    for (const auto& outputPort : layerNodeGui.vecOutputPort) {
        ax::NodeEditor::BeginPin(outputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
        ImGui::Text(outputPort->getXmlId());
        ax::NodeEditor::EndPin();
        if (&outputPort != &(layerNodeGui.vecOutputPort.back())) {
            ImGui::SameLine();
        }
    }
    ax::NodeEditor::EndNode();
}