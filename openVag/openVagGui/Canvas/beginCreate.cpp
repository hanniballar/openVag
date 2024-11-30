#include "beginCreate.h"

#include <iostream>

#include "../commands/InsertEdge.h"
#include "../OpenVag.h"

namespace Canvas {
    void beginCreate(std::shared_ptr<IRModel> irModelGui, CommandCenter& commandCenter) {
        if (ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 1.0f)) {
            ax::NodeEditor::PinId startPinId = 0, endPinId = 0;
            if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId)) {
                std::shared_ptr<OutputPort> outputPort;
                std::shared_ptr<InputPort> inputPort;
                outputPort = irModelGui->getNetwork()->getLayers()->getOutputPort(startPinId);
                if (outputPort) { inputPort = irModelGui->getNetwork()->getLayers()->getInputPort(endPinId); }
                else {
                    outputPort = irModelGui->getNetwork()->getLayers()->getOutputPort(endPinId);
                    if (outputPort) { inputPort = irModelGui->getNetwork()->getLayers()->getInputPort(startPinId); }
                }

                if (outputPort && inputPort && (irModelGui->getNetwork()->getEdges()->getEdge(outputPort, inputPort) == nullptr)) {
                    if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                        commandCenter.execute(std::make_shared<InsertEdge>(irModelGui, outputPort, inputPort));
                    }
                }
                else {
                    ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
            }
        } ax::NodeEditor::EndCreate();
    }
}