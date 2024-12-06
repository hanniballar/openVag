#include "fillLayerProperties.h"

#include <sstream>
#include <map>
#include <set>
#include <utility>

std::vector<std::pair<std::string, std::string>> composeLayerAttributes(std::shared_ptr<Layer> layer) {
    auto xmlElementRaw = layer->getXmlElement()->el->ToElement();
    std::vector<std::pair<std::string, std::string>> vecLayerAttribute;
    for (const auto* attr = xmlElementRaw->FirstAttribute();
        attr != nullptr;
        attr = attr->Next()) {
        vecLayerAttribute.emplace_back(attr->Name(), attr->Value());
    }
    return vecLayerAttribute;
}

static std::string collapsingHeaderLayerName(const std::shared_ptr<Layer>& layer) {
    std::stringstream ss;
    ss << "Layer: " << layer->getName() << " id: " << layer->getId().Get();
    return ss.str();
}

std::map<std::shared_ptr<Layer>, std::vector<std::pair<std::string, std::string>>> mapLayerTovecLayerAttribute;
void fillLayerProperties(const std::vector<ax::NodeEditor::NodeId>& vecSelectedNodeId, std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter) {
    for (const auto& nodeId : vecSelectedNodeId) {
        const auto& layer = irModel->getNetwork()->getLayers()->getLayer(nodeId);
        auto vecLayerAttribute = [&]() {
            auto it = mapLayerTovecLayerAttribute.find(layer);
            if (it == mapLayerTovecLayerAttribute.end()) {
                it = mapLayerTovecLayerAttribute.insert({ layer, composeLayerAttributes(layer) }).first;
            }
            return &(it->second);
            }();
        ImGui::PushID(layer->getId().AsPointer()); {
            if (ImGui::CollapsingHeader(collapsingHeaderLayerName(layer).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                for (auto& layerAttribute : *vecLayerAttribute) {
                    char inputTextbuf[100] = "";
                    strcpy_s(inputTextbuf, 100, layerAttribute.first.c_str());
                    ImGui::PushItemWidth(80); ImGui::PushID((void*)&(layerAttribute.first)); {
                        ImGui::InputText("##", inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                    } ImGui::PopID(); ImGui::PopItemWidth();
                    layerAttribute.first = inputTextbuf;
                    ImGui::SameLine();
                    strcpy_s(inputTextbuf, 100, layerAttribute.second.c_str());
                    ImGui::PushItemWidth(150); ImGui::PushID((void*)&(layerAttribute.second)); {
                        ImGui::InputText("##", inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                    } ImGui::PopID(); ImGui::PopItemWidth();
                    layerAttribute.second = inputTextbuf;
                    ImGui::SameLine();
                    ImGui::PushID((void*)&layerAttribute); {
                        ImGui::Button("-");
                    } ImGui::PopID();
                }
                if (ImGui::Button((std::string("+") + "##" + std::to_string(layer->getId().Get())).c_str())) {
                    vecLayerAttribute->emplace_back("", "");
                }
                const auto vecOriginalAttribute = composeLayerAttributes(layer);
                if (*vecLayerAttribute != vecOriginalAttribute) {
                    ImGui::SameLine();
                    bool isSaveAllowed = [&]() {
                        std::vector<std::string> vecMandatoryAttribute = { "id", "type", "name" };
                        for (const auto& mandatoryAttribute : vecMandatoryAttribute) {
                            auto it = std::find_if(vecLayerAttribute->begin(), vecLayerAttribute->end(), [&](const std::pair<std::string, std::string>& attributeData) {return attributeData.first == mandatoryAttribute; });
                            if (it == vecLayerAttribute->end()) return false;
                        }
                        //not allowed attributes
                        if (std::find_if(vecLayerAttribute->begin(), vecLayerAttribute->end(), [&](const std::pair<std::string, std::string>& attributeData) {return attributeData.first == ""; }) != vecLayerAttribute->end()) return false;
                        return true;
                        }();
                    if (isSaveAllowed) {
                        ImGui::Button("Save");
                        ImGui::SameLine();
                    }
                    if (ImGui::Button("Reset")) {
                        mapLayerTovecLayerAttribute[layer] = vecOriginalAttribute;
                    }
                }
            }
        } ImGui::PopID();
    }
}