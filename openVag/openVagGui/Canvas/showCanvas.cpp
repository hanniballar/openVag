#include "showCanvas.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "../drawEdges.h"
#include "../drawLayerNodes.h"
#include "../beginCreate.h"
#include "../contextMenu.h"

void ShowCanvas(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, GraphLayout& graphLayout, ax::NodeEditor::EditorContext* m_Context, bool* p_open)
{
    ImGui::Begin("Canvas");
    ax::NodeEditor::SetCurrentEditor(m_Context);
    ax::NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));

    drawLayerNodes(irModel->getNetwork()->getLayers());
    drawModelEdges(irModel->getNetwork()->getEdges());
    graphLayout.layoutNodes(irModel);

    beginCreate(irModel, commandCenter);
    contextMenu(irModel, commandCenter);

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
    ImGui::End();
}
