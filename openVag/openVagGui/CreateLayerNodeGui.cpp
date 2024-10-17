#include "CreateLayerNodeGui.h"
#include <memory>

static int64_t uniqueId = 1;

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

    ImGui::Text(layerNodeGui.layerNode.name.c_str());
    ImGui::Text(layerNodeGui.layerNode.type.c_str());

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
    ax::NodeEditor::NodeId nodeId = uniqueId++;

    std::vector<std::shared_ptr<LayerPortGui>> vecInputPort;
    for (auto layerPort : layerNodeXml.vecInputPort) {
        ax::NodeEditor::PinId pinId = uniqueId++;
        vecInputPort.emplace_back(std::make_shared<LayerPortGui>(pinId, layerPort));
    }

    std::vector<std::shared_ptr<LayerPortGui>> vecOutputPort;
    for (auto& layerPort : layerNodeXml.vecOutputPort) {
        ax::NodeEditor::PinId pinId = uniqueId++;
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