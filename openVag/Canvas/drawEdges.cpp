#include "drawEdges.h"

namespace Canvas {
    void drawModelEdges(const std::unordered_set<std::shared_ptr<Edge>>& setEdge) {
        for (const auto& edge : setEdge) {
            ax::NodeEditor::Link(edge->getId(), edge->getOutputPort()->getId(), edge->getInputPort()->getId());
        }
    }
}