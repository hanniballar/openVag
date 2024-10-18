#include "createLayerNodeGui.h"
#include <memory>

#include "OpenVag.h"

void drawLayerNode(LayerNodeGui layerNodeGui) {
    ax::NodeEditor::BeginNode(layerNodeGui.id_gui);
    for (const auto& inputPort : layerNodeGui.vecInputPort) {
        ax::NodeEditor::BeginPin(inputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
            ImGui::Text(inputPort->layerPort.lock()->portID.c_str());
        ax::NodeEditor::EndPin();
        if (&inputPort != &(layerNodeGui.vecInputPort.back())) {
            ImGui::SameLine();
        }
    }

    ImGui::Text((std::string("Name: ") + layerNodeGui.layerNode.name).c_str());
    ImGui::Text((std::string("ID: ") + layerNodeGui.layerNode.layerID).c_str());
    ImGui::Text((std::string("Type: ") + layerNodeGui.layerNode.type).c_str());

    for (const auto& outputPort : layerNodeGui.vecOutputPort) {
        ax::NodeEditor::BeginPin(outputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
        ImGui::Text(outputPort->layerPort.lock()->portID.c_str());
        ax::NodeEditor::EndPin();
        if (&outputPort != &(layerNodeGui.vecOutputPort.back())) {
            ImGui::SameLine();
        }
    }
    ax::NodeEditor::EndNode();
}

LayerNodeGui createLayerNode(LayerNode layerNodeXml) {
    ax::NodeEditor::NodeId nodeId = GetNextId();

    std::vector<std::shared_ptr<LayerPortGui>> vecInputPort;
    for (auto layerPort : layerNodeXml.vecInputPort) {
        ax::NodeEditor::PinId pinId = GetNextId();
        vecInputPort.emplace_back(std::make_shared<LayerPortGui>(pinId, layerPort));
    }

    std::vector<std::shared_ptr<LayerPortGui>> vecOutputPort;
    for (auto& layerPort : layerNodeXml.vecOutputPort) {
        ax::NodeEditor::PinId pinId = GetNextId();
        vecOutputPort.emplace_back(std::make_shared<LayerPortGui>(pinId, layerPort));
    }

    LayerNodeGui layerNodeGui(nodeId, layerNodeXml, vecInputPort, vecOutputPort);
    drawLayerNode(layerNodeGui);

    auto nodeSize = ax::NodeEditor::GetNodeSize(nodeId);
    auto nodePosition = ax::NodeEditor::GetNodePosition(nodeId);

    layerNodeGui.size = nodeSize;
    layerNodeGui.pos = nodePosition;

    return layerNodeGui;
}