#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "LayerNode.h"

class LayerNodeGui {
public:
	ax::NodeEditor::NodeId id_gui;
	ImRect rect;
	LayerNode layerNode;
};
