#include "showEdgeProperties.h"

#include <algorithm>
#include <sstream>

#include "imgui.h"
#include "imgui_node_editor.h"

static std::vector<const char*> getVecPossibleFromLayersId(std::shared_ptr<IRModel> irModel) {
    std::vector<const char*> vecPossibleFromLayers;
    auto layers = irModel->getNetwork()->getLayers();
    for (const auto& layer : *layers) {
        if (!layer->getSetOutputPort().empty()) {
            vecPossibleFromLayers.push_back(layer->getXmlId());
        }
    }

    std::sort(vecPossibleFromLayers.begin(), vecPossibleFromLayers.end(), [](const char* lhs, const char* rhs) {
        return std::strcmp(lhs, rhs) < 0;
        });
    vecPossibleFromLayers.erase(std::unique(vecPossibleFromLayers.begin(), vecPossibleFromLayers.end(), [](const char* lhs, const char* rhs) {
        return std::strcmp(lhs, rhs) == 0;
        }), vecPossibleFromLayers.end());
    return vecPossibleFromLayers;
}

static std::vector<const char*> getVecPossibleToLayersId(std::shared_ptr<IRModel> irModel) {
    std::vector<const char*> vecPossibleToLayers;
    auto layers = irModel->getNetwork()->getLayers();
    for (const auto& layer : *layers) {
        if (!layer->getSetInputPort().empty()) {
            vecPossibleToLayers.push_back(layer->getXmlId());
        }
    }

    std::sort(vecPossibleToLayers.begin(), vecPossibleToLayers.end(), [](const char* lhs, const char* rhs) {
        return std::strcmp(lhs, rhs) < 0;
        });
    vecPossibleToLayers.erase(std::unique(vecPossibleToLayers.begin(), vecPossibleToLayers.end(), [](const char* lhs, const char* rhs) {
        return std::strcmp(lhs, rhs) == 0;
        }), vecPossibleToLayers.end());
    return vecPossibleToLayers;
}

static std::vector<const char*> getVecInputPortId(const std::shared_ptr<Layer>& layer) {
    std::vector<const char*> vecInputPortId;
    for (const auto inputPort : layer->getSetInputPort()) {
        vecInputPortId.push_back(inputPort->getXmlId());
    }
    return vecInputPortId;
}

static std::vector<const char*> getVecOutputPortId(const std::shared_ptr<Layer>& layer) {
    std::vector<const char*> vecOutputPortId;
    for (const auto inputPort : layer->getSetOutputPort()) {
        vecOutputPortId.push_back(inputPort->getXmlId());
    }
    return vecOutputPortId;
}

static std::string edgeToString(const std::shared_ptr<Edge>& edge) {
    std::stringstream ss;
    ss << edge->getFromLayer()->getXmlId() << ":" << edge->getOutputPort()->getXmlId() << "->" << edge->getToLayer()->getXmlId() << ":" << edge->getInputPort()->getXmlId();
    return ss.str();
}

static std::string collapsingHeaderEdgeName(const std::shared_ptr<Edge>& edge) {
    return std::string("Edge ") + edgeToString(edge);
}

void showEdgeProperties(const std::vector<ax::NodeEditor::LinkId>& vecSelectedLinkId, std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
{
    ImGui::Begin("Edge properties");
    ax::NodeEditor::SetCurrentEditor(m_Context);
    auto linkid = *vecSelectedLinkId.begin();
    auto edge = irModel->getNetwork()->getEdges()->getEdge(linkid);
    auto vecPossibleFromLayers = getVecPossibleFromLayersId(irModel);
    auto vecPossibleToLayers = getVecPossibleToLayersId(irModel);

    if (ImGui::CollapsingHeader(collapsingHeaderEdgeName(edge).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        static int selectedFromLayer = 0;
        ImGui::Combo("from-layer", &selectedFromLayer, vecPossibleFromLayers.data(), static_cast<int>(vecPossibleFromLayers.size()), -1);

        std::shared_ptr<Layer> fromLayer = irModel->getNetwork()->getLayers()->getLayer(vecPossibleFromLayers[selectedFromLayer]);
        auto vecOutputPortId = getVecOutputPortId(fromLayer);
        static int selectedOutputPort = 0;
        ImGui::Combo("from-port", &selectedOutputPort, vecOutputPortId.data(), static_cast<int>(vecOutputPortId.size()), -1);

        static int selectedToLayer = 0;
        ImGui::Combo("to-layer", &selectedToLayer, vecPossibleToLayers.data(), static_cast<int>(vecPossibleToLayers.size()), -1);

        std::shared_ptr<Layer> toLayer = irModel->getNetwork()->getLayers()->getLayer(vecPossibleToLayers[selectedToLayer]);
        auto vecInputPortId = getVecInputPortId(toLayer);
        static int selectedInputPort = 0;
        ImGui::Combo("to-port", &selectedInputPort, vecInputPortId.data(), static_cast<int>(vecInputPortId.size()), -1);

        ImGui::Dummy(ImVec2(0, 2));
        ImGui::Button("Save");
        ImGui::SameLine();
        ImGui::Button("Cancel");



    }
    
    ax::NodeEditor::SetCurrentEditor(nullptr);
    ImGui::End();
}
