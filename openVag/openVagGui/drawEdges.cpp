#include "drawEdges.h"

void drawModelEdges(const std::vector<std::shared_ptr<LayerNodeGui>> vecLayerNodeGui) {
    for (const auto& layerNodeGui : vecLayerNodeGui) {
        for (const auto& portGui : layerNodeGui->vecOutputPort) {
            for (const auto& edge : portGui->vecEdgeGui)
                ax::NodeEditor::Link(edge->linkId, edge->outputPort->pinId_gui, edge->inputPort->pinId_gui);
        }
    }
}