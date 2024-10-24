#include "drawLayerNodes.h"
#include <string>

#include "OpenVag.h"

void drawLayerNode(const LayerNodeGui& layerNodeGui) {
    ax::NodeEditor::BeginNode(layerNodeGui.id_gui);
    for (const auto& inputPort : layerNodeGui.vecInputPort) {
        ax::NodeEditor::BeginPin(inputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
            ImGui::Text(inputPort->getXmlId());
        ax::NodeEditor::EndPin();
        if (&inputPort != &(layerNodeGui.vecInputPort.back())) {
            ImGui::SameLine();
        }
    }
    ImGui::Text("dert");
    auto res1 = layerNodeGui.getName();
    auto res2 = std::string("Name: ") + layerNodeGui.getName();
    auto res = (std::string("Name: ") + layerNodeGui.getName()).c_str();
    //ImGui::Text((std::string("Name: ") + layerNodeGui.getName()).c_str());
    //ImGui::Text((std::string("ID: ") + layerNodeGui.getXmlId()).c_str());
    //ImGui::Text((std::string("Type: ") + layerNodeGui.getType()).c_str());

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

void drawLayerNodes(std::vector<std::shared_ptr<LayerNodeGui>> vecLayerNodeGui) {
    for (const auto layerNode : vecLayerNodeGui) {
        drawLayerNode(*(layerNode.get()));
    }
}