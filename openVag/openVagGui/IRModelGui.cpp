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

std::shared_ptr<EdgeGui> EdgeGui::PreviousSibling()
{
    std::shared_ptr<EdgeGui> previousSibling;
    for (auto& edgeGui : Parent()->vecEdgeGui) {
        if (edgeGui.get() == this) return previousSibling;
        else previousSibling = edgeGui;
    }
    return previousSibling;
}

std::shared_ptr<LayerOutputPortGui> EdgeGui::Parent()
{
    return outputPort;
}
