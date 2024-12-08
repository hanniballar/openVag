#include "drawEdges.h"

namespace Canvas {
    void drawModelEdges(std::shared_ptr<Edges> edges) {
        for (const auto& edge : *edges) {
            ax::NodeEditor::Link(edge->getId(), edge->getOutputPort()->getId(), edge->getInputPort()->getId());
        }
    }
}