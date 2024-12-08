#pragma once
#include "imgui_node_editor.h"
#include "../commands/CommandCenter.h"
#include "../IRModel.h"

void showProperties(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, ax::NodeEditor::EditorContext* m_Context, bool* p_open = NULL);