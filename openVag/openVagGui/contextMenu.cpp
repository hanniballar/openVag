#include "contextMenu.h"

#include "imgui.h"
#include "imgui_node_editor.h"

#include "commands/InsertLayer.h"
#include "commands/DeleteLayer.h"
#include "commands/DeleteInputPort.h"
#include "commands/DeleteOutputPort.h"
#include "commands/DeleteEdge.h"
#include "commands/InsertInputPort.h"

void contextMenu(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter) {
    auto openPopupPosition = ImGui::GetMousePos();
    static ax::NodeEditor::NodeId contextNodeId = 0;
    static ax::NodeEditor::PinId contextpinId = 0;
    static ax::NodeEditor::LinkId contextLinkId = 0;

    ax::NodeEditor::Suspend(); {
        if (ax::NodeEditor::ShowLinkContextMenu(&contextLinkId)) {
            ImGui::OpenPopup("Link Context Menu");
        }
        else if (ax::NodeEditor::ShowPinContextMenu(&contextpinId)) {
            ImGui::OpenPopup("Pin Context Menu");
        }
        else if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId)) {
            ImGui::OpenPopup("Node Context Menu");
        } 
        else if (ax::NodeEditor::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("Create New Node");
        }
    } ax::NodeEditor::Resume();
    ax::NodeEditor::Suspend(); {
        if (ImGui::BeginPopup("Link Context Menu")) {
            if (ImGui::MenuItem("Delete edge")) {
                auto edge = irModel->getNetwork()->getEdges()->getEdge(contextLinkId);
                assert(edge);
                auto deleteEdgeC = std::make_shared<DeleteEdge>(edge);
                commandCenter.execute(deleteEdgeC);
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("Pin Context Menu")) {

            if (ImGui::MenuItem("Delete port")) {
                auto inputPort = irModel->getNetwork()->getLayers()->getInputPort(contextpinId);
                if (inputPort) {
                    auto deletePortC = std::make_shared<DeleteInputPort>(inputPort);
                    commandCenter.execute(deletePortC);
                }
                else {
                    auto outputPort = irModel->getNetwork()->getLayers()->getOutputPort(contextpinId);
                    assert(outputPort);
                    auto deletePortC = std::make_shared<DeleteOutputPort>(outputPort);
                    commandCenter.execute(deletePortC);
                }
                
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("Node Context Menu")) {
            if (ImGui::MenuItem("New inputPort")) {
                auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                auto insertInputPort = std::make_shared<InsertInputPort>(layer);
                commandCenter.execute(insertInputPort);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete Layer")) {
                auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                auto deleteLayerC = std::make_shared<DeleteLayer>(layer);
                commandCenter.execute(deleteLayerC);
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("Create New Node")) {
            if (ImGui::MenuItem("New Layer")) {
                auto addLayerC = std::make_shared<insertLayer>(irModel);
                commandCenter.execute(addLayerC);
                auto newLayer = addLayerC->getLayer();
                ax::NodeEditor::SetNodePosition(newLayer->getId(), openPopupPosition);
            }
            ImGui::EndPopup();
        }
    } ax::NodeEditor::Resume();
}
