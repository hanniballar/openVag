#include "contextMenu.h"

#include <algorithm>
#include <vector>
#include <unordered_set>

#include "imgui.h"
#include "imgui_node_editor.h"

#include "copySelectedItems.h"
#include "pasteClipboardText.h"

#include "../commands/InsertLayer.h"
#include "../commands/DeleteLayer.h"
#include "../commands/DeleteInputPort.h"
#include "../commands/DeleteOutputPort.h"
#include "../commands/DeleteEdge.h"
#include "../commands/InsertInputPort.h"
#include "../commands/InsertOutputPort.h"
#include "../commands/ComposedCommand.h"

namespace Canvas {
    void contextMenu(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, RelayoutType& relayoutType) {
        auto openPopupPosition = ImGui::GetMousePos();
        static ax::NodeEditor::NodeId contextNodeId = 0;
        static ax::NodeEditor::PinId contextPinId = 0;
        static ax::NodeEditor::LinkId contextLinkId = 0;

        ax::NodeEditor::Suspend(); {
            if (ax::NodeEditor::ShowLinkContextMenu(&contextLinkId)) {
                contextPinId = 0;
                contextNodeId = 0;
                ImGui::OpenPopup("Link Context Menu");
            }
            else if (ax::NodeEditor::ShowPinContextMenu(&contextPinId)) {
                contextLinkId = 0;
                contextNodeId = 0;
                ImGui::OpenPopup("Pin Context Menu");
            }
            else if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId)) {
                contextPinId = 0;
                contextLinkId = 0;
                ImGui::OpenPopup("Node Context Menu");
            }
            else if (ax::NodeEditor::ShowBackgroundContextMenu())
            {
                contextNodeId = 0;
                contextPinId = 0;
                contextLinkId = 0;
                ImGui::OpenPopup("Background Context Menu");
            }
        } ax::NodeEditor::Resume();
        ax::NodeEditor::Suspend(); {
            if (ImGui::BeginPopup("Link Context Menu")) {
                if (ImGui::MenuItem("Copy")) {
                    copySelectedItems(irModel, contextNodeId, contextLinkId);
                }
                ImGui::Separator();
                auto selectedObjectCount = ax::NodeEditor::GetSelectedObjectCount();
                std::vector<ax::NodeEditor::LinkId> vecSelectedLinkId;
                vecSelectedLinkId.resize(selectedObjectCount);
                int linkCount = ax::NodeEditor::GetSelectedLinks(vecSelectedLinkId.data(), static_cast<int>(vecSelectedLinkId.size()));
                vecSelectedLinkId.resize(linkCount);
                if (vecSelectedLinkId.size() == 0) vecSelectedLinkId.push_back(contextLinkId);
                if (vecSelectedLinkId.size() > 1) {
                    if (ImGui::MenuItem("Delete Edges")) {
                        CommandCenter commandCC;
                        for (const auto selectedLinkId : vecSelectedLinkId) {
                            auto edge = irModel->getNetwork()->getEdges()->getEdge(selectedLinkId);
                            commandCC.add(std::make_shared<DeleteEdge>(edge));
                        }
                        auto composedCommand = std::make_shared<ComposedCommand>(commandCC);
                        commandCenter.execute(composedCommand);
                    }
                }
                else {
                    if (ImGui::MenuItem("Delete edge")) {
                        auto edge = irModel->getNetwork()->getEdges()->getEdge(*vecSelectedLinkId.begin());
                        assert(edge);
                        auto deleteEdgeC = std::make_shared<DeleteEdge>(edge);
                        commandCenter.execute(deleteEdgeC);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Reverse selection")) {
                    std::vector<ax::NodeEditor::LinkId> vecSelectedLinkId;
                    vecSelectedLinkId.resize(selectedObjectCount);
                    int linkCount = ax::NodeEditor::GetSelectedLinks(vecSelectedLinkId.data(), static_cast<int>(vecSelectedLinkId.size()));
                    vecSelectedLinkId.resize(linkCount);
                    for (const auto& edge : *irModel->getNetwork()->getEdges()) {
                        if (std::find(vecSelectedLinkId.begin(), vecSelectedLinkId.end(), edge->getId()) == vecSelectedLinkId.end()) {
                            ax::NodeEditor::SelectLink(edge->getId(), true);
                        }
                        else {
                            ax::NodeEditor::DeselectLink(edge->getId());
                        }
                    }
                }
                ImGui::EndPopup();
            }
            if (ImGui::BeginPopup("Pin Context Menu")) {
                if (ImGui::MenuItem("Delete port")) {
                    auto inputPort = irModel->getNetwork()->getLayers()->getInputPort(contextPinId);
                    if (inputPort) {
                        auto deletePortC = std::make_shared<DeleteInputPort>(inputPort);
                        commandCenter.execute(deletePortC);
                    }
                    else {
                        auto outputPort = irModel->getNetwork()->getLayers()->getOutputPort(contextPinId);
                        assert(outputPort);
                        auto deletePortC = std::make_shared<DeleteOutputPort>(outputPort);
                        commandCenter.execute(deletePortC);
                    }

                }
                ImGui::EndPopup();
            }
            if (ImGui::BeginPopup("Node Context Menu")) {
                if (ImGui::MenuItem("Copy")) {
                    copySelectedItems(irModel, contextNodeId, contextLinkId);
                }
                ImGui::Separator();
                auto selectedObjectCount = ax::NodeEditor::GetSelectedObjectCount();
                if (selectedObjectCount > 1) {
                    if (ImGui::MenuItem("Delete Layers")) {
                        std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
                        vecSelectedNodeId.resize(selectedObjectCount);
                        int linkCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
                        vecSelectedNodeId.resize(linkCount);
                        CommandCenter commandCC;
                        for (const auto selectedNodeId : vecSelectedNodeId) {
                            auto layer = irModel->getNetwork()->getLayers()->getLayer(selectedNodeId);
                            commandCC.add(std::make_shared<DeleteLayer>(layer));
                        }
                        auto composedCommand = std::make_shared<ComposedCommand>(commandCC);
                        commandCenter.execute(composedCommand);
                    }
                }
                else {
                    if (ImGui::MenuItem("New input port")) {
                        auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                        auto insertInputPort = std::make_shared<InsertInputPort>(layer);
                        commandCenter.execute(insertInputPort);
                    }
                    if (ImGui::MenuItem("New output port")) {
                        auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                        auto insertOutputPort = std::make_shared<InsertOutputPort>(layer);
                        commandCenter.execute(insertOutputPort);
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Delete Layer")) {
                        auto layer = irModel->getNetwork()->getLayers()->getLayer(contextNodeId);
                        auto deleteLayerC = std::make_shared<DeleteLayer>(layer);
                        commandCenter.execute(deleteLayerC);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Reverse selection")) {
                    std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
                    vecSelectedNodeId.resize(selectedObjectCount);
                    int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
                    vecSelectedNodeId.resize(nodeCount);
                    for (const auto& layer : *irModel->getNetwork()->getLayers()) {
                        if (std::find(vecSelectedNodeId.begin(), vecSelectedNodeId.end(), layer->getId()) == vecSelectedNodeId.end()) {
                            ax::NodeEditor::SelectNode(layer->getId(), true);
                        }
                        else {
                            ax::NodeEditor::DeselectNode(layer->getId());
                        }
                    }
                }
                if (ImGui::MenuItem("Select connecting layers")) {
                    std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
                    vecSelectedNodeId.resize(selectedObjectCount);
                    int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
                    vecSelectedNodeId.resize(nodeCount);
                    if (nodeCount == 0) {
                        vecSelectedNodeId.push_back(contextNodeId);
                        ax::NodeEditor::SelectNode(contextNodeId, true);
                    }
                    std::unordered_set<std::shared_ptr<Layer>> setToProcessLayers;
                    for (const auto& selectedNodeId : vecSelectedNodeId) { setToProcessLayers.insert(irModel->getLayers()->getLayer(selectedNodeId)); }
                    std::unordered_set<std::shared_ptr<Layer>> setProcessedLayers;
                    while (setToProcessLayers.size()) {
                        const auto& toProcessLayer = *setToProcessLayers.begin();
                        setProcessedLayers.insert(toProcessLayer);
                        
                        const auto newLayers = [&](){
                            std::unordered_set<std::shared_ptr<Layer>> newLayers;
                            const auto inputLayers = toProcessLayer->getInputLayers();
                            newLayers.insert(inputLayers.begin(), inputLayers.end());
                            const auto outputLayers = toProcessLayer->getOutputLayers();
                            newLayers.insert(outputLayers.begin(), outputLayers.end());
                            return newLayers;
                            }();

                        for (const auto& layer : newLayers) {
                            if (setProcessedLayers.find(layer) != setProcessedLayers.end()) continue;
                            ax::NodeEditor::SelectNode(layer->getId(), true);
                            setToProcessLayers.insert(layer);
                        }

                        setToProcessLayers.erase(toProcessLayer);
                    }
                }
                if (selectedObjectCount > 1) {
                    std::vector<ax::NodeEditor::NodeId> vecSelectedNodeId;
                    vecSelectedNodeId.resize(selectedObjectCount);
                    int nodeCount = ax::NodeEditor::GetSelectedNodes(vecSelectedNodeId.data(), static_cast<int>(vecSelectedNodeId.size()));
                    vecSelectedNodeId.resize(nodeCount);
                    if (nodeCount > 1) {
                        if (ImGui::MenuItem("Select connecting edges")) {
                            std::unordered_set<std::shared_ptr<Layer>> setSelectedLayer;
                            for (const auto& selectedNodeId : vecSelectedNodeId) { setSelectedLayer.insert(irModel->getLayers()->getLayer(selectedNodeId)); }
                            for (const auto& edge : *irModel->getEdges()) {
                                if (setSelectedLayer.find(edge->getFromLayer()) != setSelectedLayer.end() &&
                                    setSelectedLayer.find(edge->getToLayer()) != setSelectedLayer.end()) {
                                    ax::NodeEditor::SelectLink(edge->getId(), true);
                                }
                            }
                        }
                    }
                }
                ImGui::EndPopup();
            }
            bool pasteSuccsesfull = true;
            if (ImGui::BeginPopup("Background Context Menu")) {
                if (ImGui::MenuItem("Copy", "CTRL+C", false, ax::NodeEditor::GetSelectedObjectCount() > 0)) {
                    copySelectedItems(irModel);
                }
                const auto clipboardText = ImGui::GetClipboardText();
                if (ImGui::MenuItem("Paste", "CTRL+V", false, isValidPasteCliboard())) {
                    pasteSuccsesfull = pasteCliboardText(irModel, commandCenter/*, openPopupPosition*/);
                    if (pasteSuccsesfull) {
                        relayoutType = RelayoutType::Selection;
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("New Layer")) {
                    auto addLayerC = std::make_shared<InsertLayer>(irModel);
                    commandCenter.execute(addLayerC);
                    auto newLayer = addLayerC->getLayer();
                    ax::NodeEditor::BeginNode(newLayer->getId());
                    ax::NodeEditor::EndNode();
                    ax::NodeEditor::SetNodePosition(newLayer->getId(), openPopupPosition);
                }
                ImGui::EndPopup();
            }
            if (pasteSuccsesfull == false) {
                ImGui::OpenPopup("Paste error");
                pasteSuccsesfull = true;
            }
            displayPasteError();

        } ax::NodeEditor::Resume();

    }
}