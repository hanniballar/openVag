#include "showValidation.h"
#include <vector>
#include <string>

void showValidation(std::shared_ptr<IRModel> irModel, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
{
    ax::NodeEditor::SetCurrentEditor(m_Context);
    ImGui::Begin("Validation messages");
    static bool irModelChanged = irModel->registerHandlerModifyIR([]() { 
        irModelChanged = true;
        });
    static std::vector<std::string> validationMsg;
    if (irModelChanged) {
        validationMsg = irModel->validate();
        irModelChanged = false;
    }

    size_t pos = 0;
    for (const auto& msg : validationMsg) {
        char inputTextbuf[250] = "";
        strcpy_s(inputTextbuf, IM_ARRAYSIZE(inputTextbuf), msg.c_str());
        ImGui::PushItemWidth(-1); {
            ImGui::InputText((std::string("##") + "Validation messages" + std::to_string(pos)).c_str(), inputTextbuf, IM_ARRAYSIZE(inputTextbuf), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll | ImGuiSelectableFlags_SpanAllColumns);
        } ImGui::PopItemWidth();
        ++pos;
    }
    ImGui::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}
