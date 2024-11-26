#pragma once
#include "imgui_node_editor.h"
#include "../commands/CommandCenter.h"
#include "../IRModel.h"
#include "../GraphLayout.h"

namespace Canvas {
    void ShowCanvas(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, bool reLayoutNodes, ax::NodeEditor::EditorContext* m_Context, bool* p_open = NULL);
}