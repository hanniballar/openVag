#include "showCanvas.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "drawEdges.h"
#include "drawLayerNodes.h"
#include "beginCreate.h"
#include "contextMenu.h"
#include "../commands/DeleteLayer.h"
#include "../commands/DeleteEdge.h"
#include "../commands/ComposedCommand.h"

#include <iostream>

namespace Canvas {
    static GraphLayout graphLayout(30, 20);

    void ShowCanvas(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, bool reLayoutNodes, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
    {
        ImGui::Begin("Canvas");
        ax::NodeEditor::SetCurrentEditor(m_Context);
        ax::NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));

        drawLayerNodes(irModel->getNetwork()->getLayers());
        drawModelEdges(irModel->getNetwork()->getEdges());
        if (reLayoutNodes) {
            graphLayout.layoutNodes(irModel);
        }

        if (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_RouteFocused)) {
            std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
            std::vector<ax::NodeEditor::LinkId> vecSelectedLinkId;
            vecSelectedNodeId.resize(ax::NodeEditor::GetSelectedObjectCount());
            vecSelectedLinkId.resize(ax::NodeEditor::GetSelectedObjectCount());
            int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
            int linkCount = ax::NodeEditor::GetSelectedLinks(vecSelectedLinkId.data(), static_cast<int>(vecSelectedLinkId.size()));
            vecSelectedNodeId.resize(nodeCount);
            vecSelectedLinkId.resize(linkCount);
            CommandCenter commandCC;
            for (const auto selectedNodeId : vecSelectedNodeId) {
                auto layer = irModel->getNetwork()->getLayers()->getLayer(selectedNodeId);
                commandCC.add(std::make_shared<DeleteLayer>(layer));
            }
            for (const auto selectedLinkId : vecSelectedLinkId) {
                auto edge = irModel->getNetwork()->getEdges()->getEdge(selectedLinkId);
                commandCC.add(std::make_shared<DeleteEdge>(edge));
            }
            auto composedCommand = std::make_shared<ComposedCommand>(commandCC);
            commandCenter.execute(composedCommand);
        }

        beginCreate(irModel, commandCenter);
        contextMenu(irModel, commandCenter);

        ax::NodeEditor::End();
        ax::NodeEditor::SetCurrentEditor(nullptr);
        ImGui::End();
    }
}