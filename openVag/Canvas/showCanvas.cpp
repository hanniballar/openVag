#include "showCanvas.h"

#include <iostream>
#include <cassert>

#include "imgui.h"
#include "imgui_node_editor.h"
#include "drawEdges.h"
#include "drawLayerNodes.h"
#include "calcLayersToDraw.h"
#include "beginCreate.h"
#include "contextMenu.h"
#include "copySelectedItems.h"
#include "pasteClipboardText.h"

#include "../commands/DeleteLayer.h"
#include "../commands/DeleteEdge.h"
#include "../commands/ComposedCommand.h"

namespace Canvas {
    static GraphLayout graphLayout({ 30, 20 });

    void ShowCanvas(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, RelayoutType reLayoutNodes, const std::map<std::string, ImColor>& mapLayerTypeToColor, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
    {
        if (ImGui::Begin("Canvas")) {
            ax::NodeEditor::SetCurrentEditor(m_Context);
            ax::NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));
            {
                RelayoutType contextReLayoutNodes = RelayoutType::None;
                contextMenu(irModel, commandCenter, contextReLayoutNodes);
                if (contextReLayoutNodes != RelayoutType::None && reLayoutNodes == RelayoutType::None) {
                    reLayoutNodes = contextReLayoutNodes;
                }
            }
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V, ImGuiInputFlags_RouteFocused)) {
                auto pasteSuccsesfull = pasteCliboardText(irModel, commandCenter);
                if (pasteSuccsesfull == false) {
                    ImGui::OpenPopup("Paste error");
                }
                else {
                    if (reLayoutNodes == RelayoutType::None) { 
                        reLayoutNodes = RelayoutType::Selection; }
                }
            }
            const auto layersToDraw = calcLayersToDraw(irModel->getNetwork()->getLayers(), reLayoutNodes != RelayoutType::None);
            drawLayerNodes(layersToDraw, mapLayerTypeToColor);
            drawModelEdges(calcEdgesToDraw(layersToDraw));

            switch (reLayoutNodes) {
            case RelayoutType::All:
                graphLayout.layoutNodes({ irModel->getLayers()->begin(), irModel->getLayers()->end() });
                break;
            case RelayoutType::Selection:
            {
                std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
                vecSelectedNodeId.resize(ax::NodeEditor::GetSelectedObjectCount());
                int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
                vecSelectedNodeId.resize(nodeCount);
                std::vector<std::shared_ptr<Layer>> vecSelectedLayer;
                for (const auto& nodeId : vecSelectedNodeId) { vecSelectedLayer.push_back(irModel->getLayers()->getLayer(nodeId)); }
                graphLayout.layoutNodes({ vecSelectedLayer.begin(), vecSelectedLayer.end() }, ImGui::GetMousePos());
                ax::NodeEditor::NavigateToSelection();
            }
                break;
            case RelayoutType::None:
                break;
            default:
                assert(false);
                break;
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
                CommandCenter commandCenterCommand;
                
                for (const auto selectedLinkId : vecSelectedLinkId) {
                    auto edge = irModel->getNetwork()->getEdges()->getEdge(selectedLinkId);
                    commandCenterCommand.add(std::make_shared<DeleteEdge>(edge));
                }

                for (const auto selectedNodeId : vecSelectedNodeId) {
                    auto layer = irModel->getNetwork()->getLayers()->getLayer(selectedNodeId);
                    commandCenterCommand.add(std::make_shared<DeleteLayer>(layer));
                }

                auto composedCommand = std::make_shared<ComposedCommand>(commandCenterCommand);
                commandCenter.execute(composedCommand);
            }
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C, ImGuiInputFlags_RouteFocused)) {
                copySelectedItems(irModel);
            }

            beginCreate(irModel, commandCenter);

            ax::NodeEditor::End();
            ax::NodeEditor::SetCurrentEditor(nullptr);
        } ImGui::End();
    }
}