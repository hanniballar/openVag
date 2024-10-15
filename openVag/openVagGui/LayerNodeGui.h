#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "IRXmlRep.h"

class LayerNodeGui {
public:
	LayerNodeGui() : layerNode("1") {}
	LayerNodeGui(ax::NodeEditor::NodeId id_gui, LayerNode layerNode, ImVec2 size, ImVec2 pos) : id_gui(id_gui), layerNode(layerNode), size(size), pos(pos) {}
	ax::NodeEditor::NodeId id_gui;
	ImVec2 size;
	ImVec2 pos;
	LayerNode layerNode;
};
