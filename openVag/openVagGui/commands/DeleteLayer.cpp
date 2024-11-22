#include "DeleteLayer.h"

#include <cassert>

#include "../OpenVag.h"
#include "DeleteEdge.h"

void DeleteLayer::doAct()
{
    position = layer->getXmlPosition();
    parent = layer->getParent();
    auto xmlElClone = layer->getXmlElement()->el->DeepClone(layer->getXmlElement()->el->GetDocument());
    parent->deleteLayer(layer);
    layer->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteLayer::undoAct()
{
    parent->addLayer(layer, position);
    layer.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
