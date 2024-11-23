#include "DeleteLayer.h"

#include <cassert>

#include "../OpenVag.h"
#include "DeleteInputPort.h"
#include "DeleteOutputPort.h"

void DeleteLayer::doAct()
{
    position = layer->getXmlPosition();
    parent = layer->getParent();
    for (const auto& port : layer->getSetInputPort()) { commandCenter.execute(std::make_shared<DeleteInputPort>(port)); }
    for (const auto& port : layer->getSetOutputPort()) { commandCenter.execute(std::make_shared<DeleteOutputPort>(port)); }
    auto xmlElClone = layer->getXmlElement()->el->DeepClone(layer->getXmlElement()->el->GetDocument());
    parent->deleteLayer(layer);
    layer->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteLayer::undoAct()
{
    parent->insertLayer(layer, position);
    commandCenter.undoAll();
    commandCenter.reset();
    layer.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
