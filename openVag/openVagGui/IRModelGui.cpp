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

std::shared_ptr<EdgeGui> EdgeGui::NextSibling()
{
    const auto& vecEdgeGui = EdgeGui::Parent()->vecEdgeGui;
    auto it = std::find_if(vecEdgeGui.begin(), vecEdgeGui.end(), [&, this](const std::shared_ptr<EdgeGui>& el) { return el.get() == this; });
    auto nextIt = ++it;
    if (nextIt == vecEdgeGui.end()) return {};
    return *it++;
}

std::shared_ptr<LayerOutputPortGui> EdgeGui::Parent()
{
    return outputPort;
}

void LayerOutputPortGui::InsertBeforeChild(std::shared_ptr<EdgeGui> beforeThis, std::shared_ptr<EdgeGui> addThis)
{
    if (beforeThis) {
        auto it = std::find(vecEdgeGui.begin(), vecEdgeGui.end(), beforeThis);
        vecEdgeGui.insert(it, addThis);
    }
    else {
        vecEdgeGui.push_back(addThis);
    }
}

void LayerOutputPortGui::DeleteChild(std::shared_ptr<EdgeGui> child)
{
    auto it = std::find(vecEdgeGui.begin(), vecEdgeGui.end(), child);
    vecEdgeGui.erase(it);
}
