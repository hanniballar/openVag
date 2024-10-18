#include "createModelEdgesGui.h"

#include <map>
#include <iostream>
#include <memory>

#include "IRXmlRep.h"
#include "OpenVag.h"

void drawModelEdges(std::vector<LayerNodeGui>& vecLayerNodeGui) {

}

bool createModelEdgesGui(std::vector<LayerNodeGui>& vecLayerNodeGui, const std::vector<Edge> vecEdge) {
    struct Port {
        Port(std::string layerID, std::string portID) : layerID(layerID), portID(portID) {}

        std::string layerID;
        std::string portID;

        bool operator<(const Port& other) const {
            if (this->layerID != other.layerID) return this->layerID < other.layerID;
            if (this->portID != other.portID) return this->portID < other.portID;

            return false;
        }

        std::string toString() {
            return this->layerID + ", " + this->portID;
        }

    };
    std::map<std::string, LayerNodeGui*> mapXMLLayerIDtoGuiLayer;
    std::map<Port, std::shared_ptr<LayerPortGui>> mapInputPorttoLayerPort;
    std::map<Port, std::shared_ptr<LayerPortGui>> mapOutputPorttoLayerPort;

    for (auto& layerNodeGui : vecLayerNodeGui) {
        if (mapXMLLayerIDtoGuiLayer.count(layerNodeGui.layerNode.layerID)) {
            std::cerr << "Duplicate layer id " << layerNodeGui.layerNode.layerID << std::endl;
        }
        else {
            mapXMLLayerIDtoGuiLayer[layerNodeGui.layerNode.layerID] = &layerNodeGui;
            for (auto& layerPort : layerNodeGui.vecInputPort) {
                Port port(layerNodeGui.layerNode.layerID, layerPort->layerPort.lock()->portID);
                if (mapInputPorttoLayerPort.count(port)) {
                    std::cerr << "Duplicate port: " << layerNodeGui.layerNode.layerID << std::endl;
                }
                else {
                    mapInputPorttoLayerPort[port] = layerPort;
                }
            }
            for (auto& layerPort : layerNodeGui.vecOutputPort) {
                Port port(layerNodeGui.layerNode.layerID, layerPort->layerPort.lock()->portID);
                if (mapOutputPorttoLayerPort.count(port)) {
                    std::cerr << "Duplicate port: " << layerNodeGui.layerNode.layerID << std::endl;
                }
                else {
                    mapOutputPorttoLayerPort[port] = layerPort;
                }
            }
        }
    }

    //std::map<Port, Port> mapOutputPortToInputPort;
    for (auto& edge : vecEdge) {
        Port outputPort(edge.from_layer, edge.from_port);
        Port inputPort(edge.to_layer, edge.to_port);
        //if (mapOutputPortToInputPort.count(outputPort)) {
        //    std::cerr << "Error: Edge from: " << outputPort.toString() << " to " << inputPort.toString() << " already exists" << std::endl;
        //    continue;
        //}
        //else {
        //    mapOutputPortToInputPort[outputPort] = inputPort;
        //}
        auto inputLayerPort = mapInputPorttoLayerPort[inputPort];
        auto outputLayerPort = mapOutputPorttoLayerPort[outputPort];
        ax::NodeEditor::LinkId linkID = GetNextId();
        std::shared_ptr<EdgeGui> edgeGui = std::make_shared<EdgeGui>(linkID, outputLayerPort, inputLayerPort);


        //inputLayerPort->vecEdge.push_back(edgeGui);
        //outputLayerPort->vecEdge.push_back(edgeGui);
    }

    return true;
}
