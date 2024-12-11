#pragma once
#include "imgui_node_editor.h"
#include "../IRModel.h"

void showValidation(std::shared_ptr<IRModel> irModel, ax::NodeEditor::EditorContext* m_Context, bool* p_open = nullptr);