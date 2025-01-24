#include "copySelectedItems.h"

#include <vector>

void copySelectedItems(const std::shared_ptr<IRModel>& irModel, ax::NodeEditor::NodeId contextNodeId, ax::NodeEditor::LinkId contextLinkId) {
    auto selectedObjectCount = ax::NodeEditor::GetSelectedObjectCount();
    std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
    std::vector<ax::NodeEditor::LinkId> vecSelectedLinkId;
    vecSelectedNodeId.resize(selectedObjectCount);
    vecSelectedLinkId.resize(selectedObjectCount);
    int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
    int linkCount = ax::NodeEditor::GetSelectedLinks(vecSelectedLinkId.data(), static_cast<int>(vecSelectedLinkId.size()));
    vecSelectedNodeId.resize(nodeCount);
    vecSelectedLinkId.resize(linkCount);

    if (vecSelectedNodeId.empty() && vecSelectedLinkId.empty()) {
        if (contextNodeId.Get() != 0) vecSelectedNodeId.push_back(contextNodeId);
        if (contextLinkId.Get() != 0) vecSelectedLinkId.push_back(contextLinkId);
    }

    tinyxml2::XMLPrinter printer;
    for (const auto& selectedNodeId : vecSelectedNodeId) {
        irModel->getLayers()->getLayer(selectedNodeId)->getXmlElement()->Accept(&printer);
    }
    for (const auto& selectedLinkId : vecSelectedLinkId) {
        irModel->getEdges()->getEdge(selectedLinkId)->getXmlElement()->Accept(&printer);
    }
    ImGui::SetClipboardText(printer.CStr());
}