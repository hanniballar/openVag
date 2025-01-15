#include "fillLayerProperties.h"

#include <sstream>
#include <map>
#include <set>
#include <utility>
#include <algorithm>

#include "../commands/SetLayerAttributes.h"
#include "../commands/SetPortAttributes.h"
#include "../commands/SetPortDimensions.h"

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

std::vector<std::pair<std::string, std::string>> composePortAttributes(std::shared_ptr<Port> port) {
    auto xmlElementRaw = port->getXmlElement()->el->ToElement();
    std::vector<std::pair<std::string, std::string>> vecPortAttribute;
    for (const auto* attr = xmlElementRaw->FirstAttribute();
        attr != nullptr;
        attr = attr->Next()) {
        vecPortAttribute.emplace_back(attr->Name(), attr->Value());
    }
    return vecPortAttribute;
}

static std::string collapsingHeaderLayerName(const std::shared_ptr<Layer>& layer) {
    std::stringstream ss;
    ss << "Layer: " << layer->getName() << " id: " << layer->getId().Get();
    return ss.str();
}

std::map<std::shared_ptr<Layer>, std::vector<std::pair<std::string, std::string>>> mapLayerToVecLayerAttribute;

std::map<std::shared_ptr<Port>, std::vector<std::pair<std::string, std::string>>> mapPortToVecPortAttribute;
std::map<std::shared_ptr<Port>, std::vector<std::string>> mapPortToVecDim;

void fillDimensionProperties(std::shared_ptr<Port> port, CommandCenter& commandCenter) {
    if (ImGui::CollapsingHeader("Dimensions")) {
        auto vecDim = [&]() {
            auto it = mapPortToVecDim.find(port);
            if (it == mapPortToVecDim.end()) {
                it = mapPortToVecDim.insert({ port, port->getVecDim() }).first;
            }
            return &(it->second);
            }();
        std::vector<size_t> vecRemovePos;
        for (size_t vecDimPos = 0; vecDimPos < vecDim->size(); vecDimPos++) {
            auto& dim = (*vecDim)[vecDimPos];
            char inputTextbuf[100] = "";
            strcpy_s(inputTextbuf, 100, dim.c_str());
            ImGui::PushID(&dim); {
                ImGui::PushItemWidth(230); {
                    ImGui::InputText((std::string("##") + "Value").c_str(), inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                } ImGui::PopItemWidth();
                dim = inputTextbuf;
                ImGui::SameLine();
                if (ImGui::Button((std::string("-##") + "Delete").c_str())) {
                    vecRemovePos.emplace_back(vecDimPos);
                }
            } ImGui::PopID();
        }
        if (vecRemovePos.size()) {
            std::sort(vecRemovePos.begin(), vecRemovePos.end(), std::greater<size_t>());
            for (auto pos : vecRemovePos) { vecDim->erase(vecDim->begin() + pos); }
        }
        if (ImGui::Button((std::string("+") + "##" + "Dimenision " + std::to_string(port->getId().Get())).c_str())) {
            vecDim->emplace_back("0");
        }
        const auto vecOriginalDimension = port->getVecDim();
        if (*vecDim != vecOriginalDimension) {
            ImGui::SameLine();
            if (ImGui::Button("Apply")) {
                commandCenter.execute(std::make_shared<SetPortDimensions>(port, *vecDim));
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                mapPortToVecDim[port] = vecOriginalDimension;
            }
        }
    }
}
void fillPortProperties(std::shared_ptr<Port> port, CommandCenter& commandCenter) {
    if(port == nullptr) return; // Port was already deleted
    ImGui::PushID(port->getId().AsPointer()); {
        if (ImGui::CollapsingHeader((std::string("Port: ") + port->getXmlId()).c_str())) {
            auto vecPortAttribute = [&]() {
                auto it = mapPortToVecPortAttribute.find(port);
                if (it == mapPortToVecPortAttribute.end()) {
                    it = mapPortToVecPortAttribute.insert({ port, composePortAttributes(port) }).first;
                }
                return &(it->second);
                }();
            std::vector< std::pair<std::string, std::string>*> vecRemoveValues;
            for (auto& portAttribute : *vecPortAttribute) {
                char inputTextbuf[100] = "";
                strcpy_s(inputTextbuf, 100, portAttribute.first.c_str());
                ImGui::PushID(&portAttribute); {
                    ImGui::PushItemWidth(80); {
                        ImGui::InputText((std::string("##") + "Key").c_str(), inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                    } ImGui::PopItemWidth();
                    portAttribute.first = inputTextbuf;
                    ImGui::SameLine();
                    strcpy_s(inputTextbuf, 100, portAttribute.second.c_str());
                    ImGui::PushItemWidth(150); {
                        ImGui::InputText((std::string("##") + "Value").c_str(), inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                    } ImGui::PopItemWidth();
                    portAttribute.second = inputTextbuf;
                    ImGui::SameLine();
                    if (ImGui::Button((std::string("-##") + "Delete").c_str())) {
                        vecRemoveValues.emplace_back(&portAttribute);
                    }
                } ImGui::PopID();
            }
            if (vecRemoveValues.size()) {
                vecPortAttribute->erase(std::remove_if(vecPortAttribute->begin(), vecPortAttribute->end(), [&](const auto& val) {
                    return std::find(vecRemoveValues.begin(), vecRemoveValues.end(), &val) != vecRemoveValues.end();
                    }), vecPortAttribute->end());
            }
            if (ImGui::Button((std::string("+") + "##" + std::to_string(port->getId().Get())).c_str())) {
                vecPortAttribute->emplace_back("", "");
            }
            const auto vecOriginalAttribute = composePortAttributes(port);
            if (*vecPortAttribute != vecOriginalAttribute) {
                ImGui::SameLine();
                bool isSaveAllowed = [&]() {
                    std::vector<std::string> vecMandatoryAttribute = { "id" };
                    for (const auto& mandatoryAttribute : vecMandatoryAttribute) {
                        auto it = std::find_if(vecPortAttribute->begin(), vecPortAttribute->end(), [&](const std::pair<std::string, std::string>& attributeData) {return attributeData.first == mandatoryAttribute; });
                        if (it == vecPortAttribute->end()) return false;
                    }
                    //not allowed attributes
                    if (std::find_if(vecPortAttribute->begin(), vecPortAttribute->end(), [&](const std::pair<std::string, std::string>& attributeData) {return attributeData.first == ""; }) != vecPortAttribute->end()) return false;
                    return true;
                    }();
                if (isSaveAllowed) {
                    if (ImGui::Button("Apply")) {
                        commandCenter.execute(std::make_shared<SetPortAttributes>(port, *vecPortAttribute));
                    }
                    ImGui::SameLine();
                }
                if (ImGui::Button("Reset")) {
                    mapPortToVecPortAttribute[port] = vecOriginalAttribute;
                }
            }

            fillDimensionProperties(port, commandCenter);
        }
    } ImGui::PopID();
}

void fillLayerProperties(const std::vector<ax::NodeEditor::NodeId>& vecSelectedNodeId, std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter) {
    for (const auto& nodeId : vecSelectedNodeId) {
        const auto& layer = irModel->getNetwork()->getLayers()->getLayer(nodeId);
        if (layer == nullptr) continue; //Node was already deleted
        auto vecLayerAttribute = [&]() {
            auto it = mapLayerToVecLayerAttribute.find(layer);
            if (it == mapLayerToVecLayerAttribute.end()) {
                it = mapLayerToVecLayerAttribute.insert({ layer, composeLayerAttributes(layer) }).first;
            }
            return &(it->second);
            }();
        ImGui::PushID(layer->getId().AsPointer()); {
            auto layerPropFlags = vecSelectedNodeId.size() < 3 ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
            if (ImGui::CollapsingHeader(collapsingHeaderLayerName(layer).c_str(), layerPropFlags)) {
                std::vector< std::pair<std::string, std::string>*> vecRemoveValues;
                for (auto& layerAttribute : *vecLayerAttribute) {
                    char inputTextbuf[100] = "";
                    strcpy_s(inputTextbuf, IM_ARRAYSIZE(inputTextbuf), layerAttribute.first.c_str());
                    ImGui::PushID(&layerAttribute); {
                        ImGui::PushItemWidth(80); {
                            ImGui::InputText((std::string("##") + "Key").c_str(), inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                        } ImGui::PopItemWidth();
                        layerAttribute.first = inputTextbuf;
                        ImGui::SameLine();
                        strcpy_s(inputTextbuf, IM_ARRAYSIZE(inputTextbuf), layerAttribute.second.c_str());
                        ImGui::PushItemWidth(150); {
                            ImGui::InputText((std::string("##") + "Value").c_str(), inputTextbuf, IM_ARRAYSIZE(inputTextbuf));
                        } ImGui::PopItemWidth();
                        layerAttribute.second = inputTextbuf;
                        ImGui::SameLine();
                        if (ImGui::Button((std::string("-##") + "Delete").c_str())) {
                            vecRemoveValues.emplace_back(&layerAttribute);
                        }
                    } ImGui::PopID();
                }
                if (vecRemoveValues.size()) {
                    vecLayerAttribute->erase(std::remove_if(vecLayerAttribute->begin(), vecLayerAttribute->end(), [&](const auto& val) {
                        return std::find(vecRemoveValues.begin(), vecRemoveValues.end(), &val) != vecRemoveValues.end();
                        }), vecLayerAttribute->end());
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
                        if (ImGui::Button("Apply")) {
                            commandCenter.execute(std::make_shared<SetLayerAttributes>(layer, *vecLayerAttribute));
                        }
                        ImGui::SameLine();
                    }
                    if (ImGui::Button("Reset")) {
                        mapLayerToVecLayerAttribute[layer] = vecOriginalAttribute;
                    }
                }

                const auto& setInputPort = layer->getSetInputPort();
                if (setInputPort.size()) {
                    if (ImGui::CollapsingHeader("InputPorts")) {
                        for (const auto& port : setInputPort) {
                            fillPortProperties(port, commandCenter);
                        }
                    }
                }

                const auto& setOutputPort = layer->getSetOutputPort();
                if (setOutputPort.size()) {
                    if (ImGui::CollapsingHeader("OutputPorts")) {
                        for (const auto& port : setOutputPort) {
                            fillPortProperties(port, commandCenter);
                        }
                    }
                }
            }
        } ImGui::PopID();
    }
}