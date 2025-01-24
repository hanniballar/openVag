#include "pasteClipboardText.h"

#include <vector>
#include <memory>

#include "imgui.h"
#include "imgui_node_editor.h"

#include "../parseIRModel.h"
#include "../IRModel.h"
#include "../commands/ComposedCommand.h"
#include "../commands/InsertLayer.h"
#include "../commands/InsertEdge.h"

bool isValidPasteCliboard()
{
    const auto clipboardText = ImGui::GetClipboardText();
    return clipboardText != nullptr && strlen(clipboardText) > 0;
}

void prepareIrModelForInsertion(std::shared_ptr<IRModel> sourceIrModel, std::shared_ptr<IRModel> destIrModel) {
    auto maxdestLayerXmlId = destIrModel->getLayers()->getMaxLayerXmlId();
    auto destLayers = destIrModel->getLayers();
    for (auto layer : *(sourceIrModel->getLayers())) {
        const auto xmlId = layer->getXmlId();
        if (destLayers->getLayer(layer->getXmlId()) != nullptr || layer->getXmlId() == std::to_string(maxdestLayerXmlId)) {
            maxdestLayerXmlId++;
            layer->changeXmlId(std::to_string(maxdestLayerXmlId));
        }
    }
}

bool pasteCliboardText(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter) {
    if (isValidPasteCliboard() == false) return false;

    const auto clipboardText = ImGui::GetClipboardText();
    try {
        const auto newIrModel = clipboardText != nullptr ? parseIRModel(clipboardText, strlen(clipboardText)) : std::shared_ptr<IRModel>{};
        if (newIrModel == nullptr || newIrModel->getLayers()->size() == 0) {
            return false;
        }

        prepareIrModelForInsertion(newIrModel, irModel);
        CommandCenter commandCenterCommand;
        std::vector<std::shared_ptr<InsertLayer>> vecInsertLayer;
        for (const auto& layer : *newIrModel->getLayers()) {
            auto xmlLayer = layer->getXmlElement()->DeepClone(irModel->getXMLDocument().get());
            auto insertLayer = std::make_shared<InsertLayer>(irModel, xmlLayer->ToElement());
            commandCenterCommand.add(insertLayer);
            vecInsertLayer.push_back(insertLayer);
        }

        for (const auto& edge : *newIrModel->getEdges()) {
            commandCenterCommand.add(std::make_shared<InsertEdge>(irModel,
                edge->getFromLayer()->getXmlId(), edge->getOutputPort()->getXmlId(),
                edge->getToLayer()->getXmlId(), edge->getInputPort()->getXmlId()));
        }
        auto composedCommand = std::make_shared<ComposedCommand>(commandCenterCommand);
        commandCenter.execute(composedCommand);
        ax::NodeEditor::ClearSelection();
        for (const auto& insertLayer : vecInsertLayer) {
            const auto layer = insertLayer->getLayer();
            ax::NodeEditor::BeginNode(layer->getId());
            ax::NodeEditor::EndNode();
            ax::NodeEditor::SelectNode(layer->getId(), true);
        }
        ax::NodeEditor::NavigateToSelection();
    }
    catch (const std::exception&) {
        return false;
    }

    return true;
}

void displayPasteError() {
    if (ImGui::BeginPopupModal("Paste error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Parsing clipboard text was not successfull");
        ImGui::Dummy(ImVec2(0, 1));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Close").x) * 0.5f);
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}