#include "showProperties.h"
#include "showEdgeProperties.h"

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
    if (linkCount) {
        showEdgeProperties(vecSelectedLinkId, irModel, commandCenter, m_Context);
    }
    ax::NodeEditor::SetCurrentEditor(nullptr);
}
