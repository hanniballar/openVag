#include "ModifyEdge.h"

#include "ModifyAttributeXMLElement.h"

#include <algorithm>


void sortVecEdge(std::vector<std::shared_ptr<EdgeGui>>& vecEdgeGui) {
    std::sort(vecEdgeGui.begin(), vecEdgeGui.end(),
        [](const std::shared_ptr<EdgeGui>& lhs, const std::shared_ptr<EdgeGui>& rhs) { return lhs->linkId.Get() < rhs->linkId.Get(); });
}

static void modifyFromPort(std::shared_ptr<EdgeGui> modifyEdge, std::shared_ptr<LayerNodeGui> layer, std::string from_port)
{
    auto holdingPort = modifyEdge->Parent();
    modifyEdge->outputPort = {};
    for (auto& port : layer->vecOutputPort) {
        if (from_port == port->getXmlId()) {
            port->vecEdgeGui.push_back(modifyEdge);
            sortVecEdge(port->vecEdgeGui);
            modifyEdge->outputPort = port;
            break;
        }
    }

    holdingPort->deleteChild(modifyEdge);
}

static void modifyFromLayerFromPort(std::shared_ptr<EdgeGui> modifyEdge, std::string from_layer, std::string from_port)
{
    auto network = modifyEdge->Parent()->Parent()->Parent();

    if (from_layer != modifyEdge->xmlElement->el->ToElement()->Attribute("from-layer")) {
        for (auto& layer : network->vecLayerNodeGui) {
            if (from_layer == layer->getXmlId()) {
                modifyFromPort(modifyEdge, layer, from_port);
                break;
            }
        }
    } else if (from_port != modifyEdge->xmlElement->el->ToElement()->Attribute("from-port")) {
        modifyFromPort(modifyEdge, modifyEdge->Parent()->Parent(), from_port);
    }
}

static void modifyToPort(std::shared_ptr<EdgeGui> modifyEdge, std::shared_ptr<LayerNodeGui> layer, std::string to_port)
{
    modifyEdge->inputPort = {};
    for (auto& port : layer->vecInputPort) {
        if (to_port == port->getXmlId()) {
            modifyEdge->inputPort = port;
            break;
        }
    }
}

static void modifyToLayerToPort(std::shared_ptr<EdgeGui> modifyEdge, std::string to_layer, std::string to_port)
{
    auto network = modifyEdge->Parent()->Parent()->Parent();

    if (to_layer != modifyEdge->xmlElement->el->ToElement()->Attribute("to-layer")) {
        for (auto& layer : network->vecLayerNodeGui) {
            if (to_layer == layer->getXmlId()) {
                modifyToPort(modifyEdge, layer, to_port);
                break;
            }
        }
    }
    else if (to_port != modifyEdge->xmlElement->el->ToElement()->Attribute("to-port")) {
        modifyToPort(modifyEdge, modifyEdge->inputPort->Parent(), to_port);
    }
}

void ModifyEdge::doAct() {
    prevInputPort = modifyEdge->inputPort;
    prevOutputPort = modifyEdge->outputPort;

    std::string from_layer = mapChangeAttr.find("from-layer") == mapChangeAttr.end() ?
        modifyEdge->xmlElement->el->ToElement()->Attribute("from-layer") :
        mapChangeAttr["from-layer"];
    std::string from_port = mapChangeAttr.find("from-port") == mapChangeAttr.end() ?
        modifyEdge->xmlElement->el->ToElement()->Attribute("from-port") :
        mapChangeAttr["from-port"];
    std::string to_layer = mapChangeAttr.find("to-layer") == mapChangeAttr.end() ?
        modifyEdge->xmlElement->el->ToElement()->Attribute("to-layer") :
        mapChangeAttr["to-layer"];
    std::string to_port = mapChangeAttr.find("to-port") == mapChangeAttr.end() ?
        modifyEdge->xmlElement->el->ToElement()->Attribute("to-port") :
        mapChangeAttr["to-port"];

    modifyToLayerToPort(modifyEdge, to_layer, to_port);
    if (modifyEdge->inputPort) {
        modifyFromLayerFromPort(modifyEdge, from_layer, from_port);
        if (!modifyEdge->outputPort) { modifyEdge->inputPort.reset(); }
    } else {
        modifyEdge->Parent()->deleteChild(modifyEdge);
        modifyEdge->outputPort.reset();
    }

    commandCenter.execute(std::make_shared<ModifyAttributeXMLElement>(modifyEdge->xmlElement, mapChangeAttr));
    this->doFlag = false;
}

void ModifyEdge::undoAct() {
    if (modifyEdge->Parent()) { modifyEdge->Parent()->deleteChild(modifyEdge); }
    prevOutputPort->vecEdgeGui.push_back(modifyEdge);
    sortVecEdge(prevOutputPort->vecEdgeGui);

    modifyEdge->outputPort = prevOutputPort;
    modifyEdge->inputPort = prevInputPort;

    commandCenter.undoAll();

    prevOutputPort.reset();
    prevInputPort.reset();

    this->doFlag = true;
}




