#include "showFind.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

static bool wildCard(const std::string& txt, const std::string& pat) {
    auto n = txt.size();
    auto m = pat.size();

    // dp[i][j] will be true if txt[0..i-1] matches pat[0..j-1]
    std::vector<std::vector<bool>> dp(n + 1, std::vector<bool>(m + 1, false));

    // Empty pattern matches with empty string
    dp[0][0] = true;

    // Handle patterns with '*' at the beginning
    for (int j = 1; j <= m; j++)
        if (pat[j - 1] == '*')
            dp[0][j] = dp[0][j - 1];

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (pat[j - 1] == txt[i - 1] || pat[j - 1] == '?') {

                // Either the characters match or pattern has '?'
                // result will be same as previous state
                dp[i][j] = dp[i - 1][j - 1];
            }

            else if (pat[j - 1] == '*') {

                // if the current character of pattern is '*'
                // first case: It matches with zero character
                // second case: It matches with one or more 
                dp[i][j] = dp[i][j - 1] || dp[i - 1][j];
            }
        }
    }

    return dp[n][m];
}

static std::string edgeToString(const std::shared_ptr<Edge>& edge) {
    std::stringstream ss;
    ss << edge->getFromLayer()->getXmlId() << ":" << edge->getOutputPort()->getXmlId() << "->" << edge->getToLayer()->getXmlId() << ":" << edge->getInputPort()->getXmlId();
    return ss.str();
}

static std::set<std::string> getAllLayerAttributesNames(const std::shared_ptr<IRModel>& irModel) {
    std::set<std::string> setAttributes;
    for (const auto& layer : *irModel->getNetwork()->getLayers()) {
        tinyxml2::XMLElement* xmlElement = layer->getXmlElement();
        for (auto xmlAttr = xmlElement->FirstAttribute(); xmlAttr != nullptr; xmlAttr = xmlAttr->Next()) {
            setAttributes.insert(xmlAttr->Name());
        }
    }
    return setAttributes;
}

namespace Find {
    void Find::ShowFind(std::shared_ptr<IRModel> irModel, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
    {
        if (ImGui::Begin("Find")) {
            ax::NodeEditor::SetCurrentEditor(m_Context);

            auto& io = ImGui::GetIO();

            std::vector<ax::NodeEditor::NodeId> selectedNodes;
            std::vector<ax::NodeEditor::LinkId> selectedLinks;
            selectedNodes.resize(ax::NodeEditor::GetSelectedObjectCount());
            selectedLinks.resize(ax::NodeEditor::GetSelectedObjectCount());
            int nodeCount = ax::NodeEditor::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
            int linkCount = ax::NodeEditor::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));
            selectedNodes.resize(nodeCount);
            selectedLinks.resize(linkCount);

            static char findBuf[100] = "*";
            ImGui::InputText("##Find", findBuf, IM_ARRAYSIZE(findBuf), ImGuiInputTextFlags_None);
            std::vector<const char*> vecType = { "Layer", "Edge" };
            static size_t typeSelector = 0;
            if (ImGui::BeginCombo("##ShowFindType", vecType[typeSelector], ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall | ImGuiComboFlags_NoArrowButton))
            {
                for (size_t pos = 0; pos < vecType.size(); pos++) {
                    const bool is_selected = (typeSelector == pos);
                    if (ImGui::Selectable(vecType[pos], is_selected))
                        typeSelector = pos;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (typeSelector == 0) { //Type == Layer
                std::set<std::string> setLayerAttribute = getAllLayerAttributesNames(irModel);
                static std::string selectedLayerAttribute = "name";
                ImGui::SameLine();
                if (ImGui::BeginCombo("##ShowFindLayerAttribute", selectedLayerAttribute.c_str(), ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall | ImGuiComboFlags_NoArrowButton))
                {
                    for (const auto layerAttribute : setLayerAttribute) {
                        const bool is_selected = (selectedLayerAttribute == layerAttribute);
                        if (ImGui::Selectable(layerAttribute.c_str(), is_selected))
                            selectedLayerAttribute = layerAttribute;

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                for (const auto& layer : *(irModel->getNetwork()->getLayers()))
                {
                    const auto& attributeValue = layer->getAttributteValue(selectedLayerAttribute);
                    if (attributeValue == nullptr || wildCard(attributeValue, std::string(findBuf)) == false) continue;
                    ImGui::PushID(layer->getId().AsPointer());
                    bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), layer->getId()) != selectedNodes.end();
                    const auto layerSelectableName = [&]() {
                        if (selectedLayerAttribute == "name") return std::string(layer->getName());
                        return std::string(layer->getName()) + " " + selectedLayerAttribute + ": " + attributeValue;
                        }();
                    if (ImGui::Selectable((layerSelectableName + "##" + std::to_string(layer->getId().Get())).c_str(), &isSelected)) {
                        if (io.KeyCtrl)
                        {
                            if (isSelected) {
                                ax::NodeEditor::SelectNode(layer->getId(), true);
                            }
                            else {
                                ax::NodeEditor::DeselectNode(layer->getId());
                            }
                        }
                        else {
                            ax::NodeEditor::SelectNode(layer->getId(), false);
                        }

                        ax::NodeEditor::NavigateToSelection();
                    }
                    ImGui::PopID();
                }
            }
            else { //Type == Edge
                for (const auto& edge : *(irModel->getNetwork()->getEdges()))
                {
                    std::string edgeName = edgeToString(edge);
                    if (!wildCard(edgeName, std::string(findBuf))) continue;

                    ImGui::PushID(edge->getId().AsPointer());
                    bool isSelected = std::find(selectedLinks.begin(), selectedLinks.end(), edge->getId()) != selectedLinks.end();

                    if (ImGui::Selectable((edgeName + "##" + std::to_string(reinterpret_cast<uintptr_t>(edge->getId().AsPointer()))).c_str(), &isSelected)) {
                        if (io.KeyCtrl)
                        {
                            if (isSelected) {
                                ax::NodeEditor::SelectLink(edge->getId(), true);
                            }
                            else {
                                ax::NodeEditor::DeselectLink(edge->getId());
                            }
                        }
                        else {
                            ax::NodeEditor::SelectLink(edge->getId(), false);
                        }
                        ax::NodeEditor::NavigateToSelection();
                    }
                    ImGui::PopID();
                }
            }
            ax::NodeEditor::SetCurrentEditor(nullptr);
        } ImGui::End();
    }
}