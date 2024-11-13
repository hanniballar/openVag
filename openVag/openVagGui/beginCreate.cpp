#include "beginCreate.h"

#include <iostream>

#include "commands/AddEdge.h"
#include "OpenVag.h"

void beginCreate(std::shared_ptr<IRModelGui> irModelGui, CommandCenter& commandCenter) {
    if (ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 1.0f)) {
        ax::NodeEditor::PinId startPinId = 0, endPinId = 0;
        if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId)) {
            std::shared_ptr<LayerOutputPortGui> outputPort;
            std::shared_ptr<LayerInputPortGui> inputPort;
            outputPort = irModelGui->getOutputPort(startPinId);
            if (outputPort) { inputPort = irModelGui->getInputPort(endPinId); }
            else {
                outputPort = irModelGui->getOutputPort(endPinId);
                 if (outputPort) { inputPort = irModelGui->getInputPort(startPinId); }
            }

            if (outputPort && inputPort) {
                if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                    std::shared_ptr<EdgeGui> edgeGui = std::make_shared<EdgeGui>(GetNextId(), outputPort, inputPort, nullptr);
                    commandCenter.execute(std::make_shared<AddEdge>(edgeGui));
                }
            }
            else {
                ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }

            std::cout << startPinId.Get() << " | " << endPinId.Get() << std::endl;
        }
    } ax::NodeEditor::EndCreate();
}