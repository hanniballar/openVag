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

std::ptrdiff_t LayerNodeGui::getMyPositionAsChild()
{
    const auto& vecChildren = parent->vecLayerNodeGui;
    auto it = std::find_if(vecChildren.begin(), vecChildren.end(), [&, this](const auto& el) { return el.get() == this; });

    return std::distance(vecChildren.begin(), it);
}

std::ptrdiff_t EdgeGui::getMyPositionAsChild()
{
    const auto& vecChildren = Parent()->vecEdgeGui;
    auto it = std::find_if(vecChildren.begin(), vecChildren.end(), [&, this](const auto& el) { return el.get() == this; });

    return std::distance(vecChildren.begin(), it);
}

std::shared_ptr<LayerOutputPortGui> EdgeGui::Parent()
{
    return outputPort;
}

void LayerOutputPortGui::insert(std::ptrdiff_t pos, std::shared_ptr<EdgeGui> value)
{
    vecEdgeGui.insert(vecEdgeGui.begin() + pos, value);
}

void IRModelGui::insert(std::ptrdiff_t pos, std::shared_ptr<LayerNodeGui> value)
{
    vecLayerNodeGui.insert(vecLayerNodeGui.begin() + pos, value);
}

void LayerOutputPortGui::deleteChild(std::shared_ptr<EdgeGui> child)
{
    auto it = std::find(vecEdgeGui.begin(), vecEdgeGui.end(), child);
    vecEdgeGui.erase(it);
}

void IRModelGui::deleteChild(std::shared_ptr<LayerNodeGui> child)
{
    auto it = std::find(vecLayerNodeGui.begin(), vecLayerNodeGui.end(), child);
    vecLayerNodeGui.erase(it);
}


