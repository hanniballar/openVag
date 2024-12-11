#include "showProperties.h"
#include "fillEdgeProperties.h"
#include "fillLayerProperties.h"

void showProperties(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
{
    ax::NodeEditor::SetCurrentEditor(m_Context);
    auto selectedObjectCount = ax::NodeEditor::GetSelectedObjectCount();
    std::vector<ax::NodeEditor::LinkId> vecSelectedLinkId;
    std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
    vecSelectedLinkId.resize(selectedObjectCount);
    vecSelectedNodeId.resize(selectedObjectCount);
    int linkCount = ax::NodeEditor::GetSelectedLinks(vecSelectedLinkId.data(), static_cast<int>(vecSelectedLinkId.size()));
    int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
    vecSelectedLinkId.resize(linkCount);
    vecSelectedNodeId.resize(nodeCount);
    ImGui::Begin("Properties");
    if (nodeCount) {
        fillLayerProperties(vecSelectedNodeId, irModel, commandCenter);
    }
    if (linkCount) {
        fillEdgeProperties(vecSelectedLinkId, irModel, commandCenter);
    }
    ImGui::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}
