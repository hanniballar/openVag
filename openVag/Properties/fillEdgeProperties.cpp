#include "fillEdgeProperties.h"

#include <algorithm>
#include <sstream>
#include <cassert>
#include <map>

#include "imgui.h"
#include "imgui_node_editor.h"

#include "../commands/ModifyEdge.h"

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

struct EdgeComboSelectors {
    int fromLayer;
    int fromPort;
    int toLayer;
    int toPort;

    bool operator==(const EdgeComboSelectors& other) const {
        return fromLayer == other.fromLayer 
            && fromPort == other.fromPort
            && toLayer == other.toLayer
            && toPort == other.toPort;
    }

    bool operator!=(const EdgeComboSelectors& other) const {
        return !(operator==(other));
    }
};

static EdgeComboSelectors composeEdgeComboItemSelectors(const std::shared_ptr<Edge>& edge, std::vector<const char*> vecFromLayerId, std::vector<const char*> vecToLayerId) {
    EdgeComboSelectors selectors;
    {
        auto it = std::find(vecFromLayerId.begin(), vecFromLayerId.end(), edge->getFromLayer()->getXmlId());
        assert(it != vecFromLayerId.end());
        selectors.fromLayer = static_cast<int>(std::distance(vecFromLayerId.begin(), it));
    }
    {
        auto it = std::find(vecToLayerId.begin(), vecToLayerId.end(), edge->getToLayer()->getXmlId());
        assert(it != vecToLayerId.end());
        selectors.toLayer = static_cast<int>(std::distance(vecToLayerId.begin(), it));
    }
    {
        const auto setOutputPort = edge->getFromLayer()->getSetOutputPort();
        auto it = setOutputPort.find(edge->getOutputPort());
        assert(it != setOutputPort.end());
        selectors.fromPort = static_cast<int>(std::distance(setOutputPort.begin(), it));
    }
    {
        const auto setInputPort = edge->getToLayer()->getSetInputPort();
        auto it = setInputPort.find(edge->getInputPort());
        assert(it != setInputPort.end());
        selectors.toPort = static_cast<int>(std::distance(setInputPort.begin(), it));
    }

    return selectors;
}

std::map<std::shared_ptr<Edge>, EdgeComboSelectors> mapEdgeToSelectors;
void fillEdgeProperties(const std::vector<ax::NodeEditor::LinkId>& vecSelectedLinkId, std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter)
{
    auto vecPossibleFromLayers = getVecPossibleFromLayersId(irModel);
    auto vecPossibleToLayers = getVecPossibleToLayersId(irModel);

    for (const auto& linkid : vecSelectedLinkId) {
        const auto& edge = irModel->getNetwork()->getEdges()->getEdge(linkid);
        if (edge == nullptr) continue;

        ImGui::PushID(edge->getId().AsPointer()); {
            if (ImGui::CollapsingHeader(collapsingHeaderEdgeName(edge).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                auto edgeComboSelectors = [&]() {
                    auto it = mapEdgeToSelectors.find(edge);
                    if (it == mapEdgeToSelectors.end()) {
                        it = mapEdgeToSelectors.insert({ edge, composeEdgeComboItemSelectors(edge, vecPossibleFromLayers, vecPossibleToLayers) }).first;
                    }
                    return &(it->second);
                    }();

                int selectedFromLayer = edgeComboSelectors->fromLayer;
                ImGui::Combo((std::string(std::string("from-layer") + "##" + std::to_string(edge->getId().Get()))).c_str(), &selectedFromLayer, vecPossibleFromLayers.data(), static_cast<int>(vecPossibleFromLayers.size()), -1);
                std::shared_ptr<Layer> fromLayer = irModel->getNetwork()->getLayers()->getLayer(vecPossibleFromLayers[selectedFromLayer]);
                if (selectedFromLayer != edgeComboSelectors->fromLayer) {
                    edgeComboSelectors->fromPort = -1;
                    edgeComboSelectors->fromLayer = selectedFromLayer;
                }

                auto vecOutputPortId = getVecOutputPortId(fromLayer);
            ImGui::Combo("from-port", &edgeComboSelectors->fromPort, vecOutputPortId.data(), static_cast<int>(vecOutputPortId.size()), -1);

                int selectedToLayer = edgeComboSelectors->toLayer;
            ImGui::Combo("to-layer", &selectedToLayer, vecPossibleToLayers.data(), static_cast<int>(vecPossibleToLayers.size()), -1);
                std::shared_ptr<Layer> toLayer = irModel->getNetwork()->getLayers()->getLayer(vecPossibleToLayers[selectedToLayer]);
                if (selectedToLayer != edgeComboSelectors->toLayer) {
                    edgeComboSelectors->toPort = -1;
                    edgeComboSelectors->toLayer = selectedToLayer;
                }

                auto vecInputPortId = getVecInputPortId(toLayer);
            ImGui::Combo("to-port", &edgeComboSelectors->toPort, vecInputPortId.data(), static_cast<int>(vecInputPortId.size()), -1);

                auto origSelectors = composeEdgeComboItemSelectors(edge, vecPossibleFromLayers, vecPossibleToLayers);
                if (*edgeComboSelectors != origSelectors) {
                    ImGui::Dummy(ImVec2(0, 2));
                    if (edgeComboSelectors->fromPort > -1 && edgeComboSelectors->toPort > -1) {
                        if (ImGui::Button("Apply")) {
                            std::map<std::string, std::string> mapAttribute =
                            { {"from-layer", vecPossibleFromLayers[edgeComboSelectors->fromLayer]},
                                {"from-port", vecOutputPortId[edgeComboSelectors->fromPort]},
                                {"to-layer", vecPossibleToLayers[edgeComboSelectors->toLayer]},
                                {"to-port", vecInputPortId[edgeComboSelectors->toPort]} };
                            commandCenter.execute(std::make_shared<ModifyEdge>(edge, mapAttribute));
                        }
                        ImGui::SameLine();
                    }
                    if (ImGui::Button("Reset")) {
                        mapEdgeToSelectors[edge] = origSelectors;
                    }
                }
            }
        } ImGui::PopID();
    }
}
