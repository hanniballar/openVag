#include "IRModelGui.h"

#include <algorithm>

#include <imgui_node_editor.h>

ImVec2 LayerNodeGui::getSize()
{
    return ax::NodeEditor::GetNodeSize(id_gui);
}

ImVec2 LayerNodeGui::getPos()
{
    return ax::NodeEditor::GetNodePosition(id_gui);
}

std::ptrdiff_t EdgeGui::getMyPositionAsChild()
{
    const auto& vecEdgeGui = EdgeGui::Parent()->vecEdgeGui;
    auto it = std::find_if(vecEdgeGui.begin(), vecEdgeGui.end(), [&, this](const std::shared_ptr<EdgeGui>& el) { return el.get() == this; });

    return std::distance(vecEdgeGui.begin(), it);
}

std::shared_ptr<LayerOutputPortGui> EdgeGui::Parent()
{
    return outputPort;
}

void LayerOutputPortGui::insert(std::ptrdiff_t pos, std::shared_ptr<EdgeGui> value)
{
    vecEdgeGui.insert(vecEdgeGui.begin() + pos, value);
}

void LayerOutputPortGui::deleteChild(std::shared_ptr<EdgeGui> child)
{
    auto it = std::find(vecEdgeGui.begin(), vecEdgeGui.end(), child);
    vecEdgeGui.erase(it);
}
