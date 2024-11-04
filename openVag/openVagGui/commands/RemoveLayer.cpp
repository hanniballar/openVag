#include "RemoveLayer.h"

#include "RemoveEdge.h"

void RemoveLayer::execute()
{
    if (doFlag) { doAct(); }
    else { undoAct(); }
}

void RemoveLayer::doAct() {
    this->doFlag = false;
    //removeXMLElement.execute();
    std::vector<std::shared_ptr<ICommand>> vecEraseCommands;
    for (auto& layer : removeLayer->Parent()->vecLayerNodeGui) {
        if (layer == removeLayer) continue;
        for (auto& port : layer->vecOutputPort) {
            for (auto& edge : port->vecEdgeGui) {
                if (edge->inputPort->parent == removeLayer) {
                    vecEraseCommands.push_back(std::make_shared<RemoveEdge>(edge));
                }
            }
        }
    }
    for (auto& port : removeLayer->vecOutputPort) {
        for (auto& edge : port->vecEdgeGui) {
            vecEraseCommands.push_back(std::make_shared<RemoveEdge>(edge));
        }
    }
    std::reverse(vecEraseCommands.begin(), vecEraseCommands.end());
    for (auto command : vecEraseCommands) { commandCenter.execute(command); }
    commandCenter.execute(std::make_shared<RemoveXMLElement>(removeLayer->xmlLayer));
    positionAsChild = removeLayer->getMyPositionAsChild();
    removeLayer->Parent()->deleteChild(removeLayer);
}

void RemoveLayer::undoAct() {
    this->doFlag = true;
    removeXMLElement.execute();
    commandCenter.undoAll();
    removeLayer->Parent()->insert(positionAsChild, removeLayer);
    positionAsChild = {};
    commandCenter = {};
}
