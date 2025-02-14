#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "imgui.h"
#include "imgui_node_editor.h"
#include "../commands/CommandCenter.h"
#include "../IRModel.h"
#include "../GraphLayout.h"

namespace Canvas {
    enum class RelayoutType : uint8_t { None = 0, All, Selection };
    void ShowCanvas(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, RelayoutType reLayoutNodes, const std::map<std::string, ImColor>& mapLayerTypeToColor, ax::NodeEditor::EditorContext* m_Context, bool* p_open = NULL);
}