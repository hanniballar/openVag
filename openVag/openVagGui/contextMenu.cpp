#include "contextMenu.h"

#include "imgui.h"
#include "imgui_node_editor.h"

#include "commands/AddLayer.h"
#include "commands/DeleteLayer.h"

void contextMenu(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter) {
    auto openPopupPosition = ImGui::GetMousePos();
    static ax::NodeEditor::NodeId contextNodeId = 0;
    ax::NodeEditor::Suspend(); {
        if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId))
            ImGui::OpenPopup("Node Context Menu");
        else if (ax::NodeEditor::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("Create New Node");
        }
    } ax::NodeEditor::Resume();
    ax::NodeEditor::Suspend(); {
        if (ImGui::BeginPopup("Node Context Menu")) {
            if (ImGui::MenuItem("Delete Layer")) {
                auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                auto deleteLayerC = std::make_shared<DeleteLayer>(irModel, layer); //Delete port and edge nned to be created first
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
