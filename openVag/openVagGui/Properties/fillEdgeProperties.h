#pragma once
#include <vector>

#include "imgui_node_editor.h"
#include "../commands/CommandCenter.h"
#include "../IRModel.h"

void fillEdgeProperties(const std::vector<ax::NodeEditor::LinkId>& vecSelectedLinkId, std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter);