#include "drawEdges.h"

void drawModelEdges(std::vector<LayerNodeGui>& vecLayerNodeGui) {
    for (auto& layerNodeGui : vecLayerNodeGui) {
        for (auto& portGui : layerNodeGui.vecOutputPort) {
            for (auto& edge : portGui->vecEdgeGui)
                ax::NodeEditor::Link(edge->linkId, edge->outputPort->pinId_gui, edge->inputPort->pinId_gui);
        }
    }
}