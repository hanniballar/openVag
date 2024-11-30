#include "showFind.h"
#include <iostream>
#include <string>
#include <vector>

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

            for (const auto& layer : *(irModel->getNetwork()->getLayers()))
            {
                if (!wildCard(layer->getName(), std::string(findBuf))) continue;

                ImGui::PushID(layer->getId().AsPointer());
                bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), layer->getId()) != selectedNodes.end();

                if (ImGui::Selectable((std::string(layer->getName()) + "##" + std::to_string(reinterpret_cast<uintptr_t>(layer->getId().AsPointer()))).c_str(), &isSelected)) {
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

            ax::NodeEditor::SetCurrentEditor(nullptr);

        } ImGui::End();
    }
}