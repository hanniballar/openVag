#include "createLayerNodeGui.h"
#include <memory>

#include "OpenVag.h"

void drawLayerNode(LayerNodeGui layerNodeGui) {
    //ax::NodeEditor::BeginNode(layerNodeGui.id_gui);
    //for (const auto& inputPort : layerNodeGui.vecInputPort) {
    //    ax::NodeEditor::BeginPin(inputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
    //        ImGui::Text(inputPort->layerPort->portID.c_str());
    //    ax::NodeEditor::EndPin();
    //    if (&inputPort != &(layerNodeGui.vecInputPort.back())) {
    //        ImGui::SameLine();
    //    }
    //}

    //ImGui::Text((std::string("Name: ") + layerNodeGui.layerNode.name).c_str());
    //ImGui::Text((std::string("ID: ") + layerNodeGui.layerNode.layerID).c_str());
    //ImGui::Text((std::string("Type: ") + layerNodeGui.layerNode.type).c_str());

    //for (const auto& outputPort : layerNodeGui.vecOutputPort) {
    //    ax::NodeEditor::BeginPin(outputPort->pinId_gui, ax::NodeEditor::PinKind::Input);
    //    ImGui::Text(outputPort->layerPort->portID.c_str());
    //    ax::NodeEditor::EndPin();
    //    if (&outputPort != &(layerNodeGui.vecOutputPort.back())) {
    //        ImGui::SameLine();
    //    }
    //}
    //ax::NodeEditor::EndNode();
}

LayerNodeGui createLayerNode(LayerNodeGui layerNodeXml) {
    //ax::NodeEditor::NodeId nodeId = GetNextId();

    //std::vector<std::shared_ptr<LayerInputPortGui>> vecInputPort;
    //for (auto layerPort : layerNodeXml.vecInputPort) {
    //    ax::NodeEditor::PinId pinId = GetNextId();
    //    vecInputPort.emplace_back(std::make_shared<LayerInputPortGui>(pinId, layerPort));
    //}

    //std::vector<std::shared_ptr<LayerOutputPortGui>> vecOutputPort;
    //for (auto& layerPort : layerNodeXml.vecOutputPort) {
    //    ax::NodeEditor::PinId pinId = GetNextId();
    //    vecOutputPort.emplace_back(std::make_shared<LayerOutputPortGui>(pinId, layerPort));
    //}

    //LayerNodeGui layerNodeGui(nodeId, layerNodeXml, vecInputPort, vecOutputPort);
    //drawLayerNode(layerNodeGui);

    //return layerNodeGui;
    return LayerNodeGui(1, nullptr);
}