#include "IRModelGui.h"

#include <imgui_node_editor.h>

ImVec2 LayerNodeGui::getSize()
{
    return ax::NodeEditor::GetNodeSize(id_gui);
}

ImVec2 LayerNodeGui::getPos()
{
    return ax::NodeEditor::GetNodePosition(id_gui);
}
