#include "fillLayerProperties.h"

#include <sstream>
#include <map>
#include <set>

struct AttributeData {
    AttributeData(std::string name, std::string value) : name(name), value(value) {};
    std::string name;
    std::string value;

    bool operator==(const AttributeData& other) const {
        return name == other.name
            && value == other.value;
    }

    bool operator!=(const AttributeData& other) const {
        return !(operator==(other));
    }
};

std::vector<AttributeData> composeLayerAttributes(std::shared_ptr<Layer> layer) {
    auto xmlElementRaw = layer->getXmlElement()->el->ToElement();
    std::vector<AttributeData> vecLayerAttribute;
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

std::map<std::shared_ptr<Layer>, std::vector<AttributeData>> mapLayerTovecLayerAttribute;
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
                    strcpy_s(inputTextbuf, 100, layerAttribute.name.c_str());
                    ImGui::PushItemWidth(80); ImGui::PushID((void*)&(layerAttribute.name)); {
                        ImGui::InputText("##", inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                    } ImGui::PopID(); ImGui::PopItemWidth();
                    layerAttribute.name = inputTextbuf;
                    ImGui::SameLine();
                    strcpy_s(inputTextbuf, 100, layerAttribute.value.c_str());
                    ImGui::PushItemWidth(150); ImGui::PushID((void*)&(layerAttribute.value)); {
                        ImGui::InputText("##", inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                    } ImGui::PopID(); ImGui::PopItemWidth();
                    layerAttribute.value = inputTextbuf;
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
                            auto it = std::find_if(vecLayerAttribute->begin(), vecLayerAttribute->end(), [&](const AttributeData& attributeData) {return attributeData.name == mandatoryAttribute; });
                            if (it == vecLayerAttribute->end()) return false;
                        }
                        //not allowed attributes
                        if (std::find_if(vecLayerAttribute->begin(), vecLayerAttribute->end(), [&](const AttributeData& attributeData) {return attributeData.name == ""; }) != vecLayerAttribute->end()) return false;
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