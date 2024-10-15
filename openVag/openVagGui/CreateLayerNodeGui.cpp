#include "CreateLayerNodeGui.h"

static int64_t uniqueId = 0;
LayerNodeGui CreateLayerNode(LayerNode layerNdoeXml) {

    uniqueId = 1;
    ax::NodeEditor::NodeId nodeId = uniqueId++;

    ax::NodeEditor::BeginNode(nodeId);
    ImGui::Text("Node A");
    ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
    ImGui::Text("-> In");
    ax::NodeEditor::EndPin();
    ImGui::SameLine();
    ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
    ImGui::Text("Out ->");
    ax::NodeEditor::EndPin();
    ax::NodeEditor::EndNode();

    auto nodeSize = ax::NodeEditor::GetNodeSize(nodeId);
    auto nodePosition = ax::NodeEditor::GetNodePosition(nodeId);

    return LayerNodeGui();
}