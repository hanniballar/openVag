#pragma once
#include "imgui_node_editor.h"
#include "../commands/CommandCenter.h"
#include "../IRModel.h"

namespace Find {
    void ShowFind(std::shared_ptr<IRModel> irModel, ax::NodeEditor::EditorContext* m_Context, bool* p_open = NULL);
}