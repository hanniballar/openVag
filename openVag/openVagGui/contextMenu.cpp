#include "contextMenu.h"

#include "imgui.h"
#include "imgui_node_editor.h"

#include "commands/AddLayer.h"
#include "commands/DeleteLayer.h"
#include "commands/DeleteInputPort.h"
#include "commands/DeleteOutputPort.h"

void contextMenu(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter) {
    auto openPopupPosition = ImGui::GetMousePos();
    static ax::NodeEditor::PinId contextpinId = 0;
    static ax::NodeEditor::NodeId contextNodeId = 0;

    ax::NodeEditor::Suspend(); {
        if (ax::NodeEditor::ShowPinContextMenu(&contextpinId)) {
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
        if (ImGui::BeginPopup("Pin Context Menu")) {
            if (ImGui::MenuItem("Delete Pin")) {
                auto inputPort = irModel->getNetwork()->getLayers()->getInputPort(contextpinId);
                if (inputPort) {
                    auto deletePortC = std::make_shared<DeleteInputPort>(inputPort); //Delete edge need to be created first
                    commandCenter.execute(deletePortC);
                }
                else {
                    auto outputPort = irModel->getNetwork()->getLayers()->getOutputPort(contextpinId);
                    assert(outputPort);
                    auto deletePortC = std::make_shared<DeleteOutputPort>(outputPort); //Delete edge need to be created first
                    commandCenter.execute(deletePortC);
                }
                
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("Node Context Menu")) {
            if (ImGui::MenuItem("Delete Layer")) {
                auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                auto deleteLayerC = std::make_shared<DeleteLayer>(layer); //Delete port and edge need to be created first
                commandCenter.execute(deleteLayerC);
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("Create New Node")) {
            if (ImGui::MenuItem("New Layer")) {
                auto addLayerC = std::make_shared<AddLayer>(irModel);
                commandCenter.execute(addLayerC);
                auto newLayer = addLayerC->getLayer();
                ax::NodeEditor::SetNodePosition(newLayer->getId(), openPopupPosition);
            }
            ImGui::EndPopup();
        }
    } ax::NodeEditor::Resume();
}
