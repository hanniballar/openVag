#include "DeleteLayer.h"

#include <cassert>

#include "../OpenVag.h"

void DeleteLayer::doAct()
{
    position = undoLayer->getXmlPosition();
    auto xmlElClone = undoLayer->getXmlElement()->el->DeepClone(undoLayer->getXmlElement()->el->GetDocument());
    irModelGui->getNetwork()->getLayers()->deleteLayer(undoLayer);
    undoLayer->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteLayer::undoAct()
{
    irModelGui->getNetwork()->getLayers()->addLayer(undoLayer, position);
    undoLayer.reset();
    position = 0;
    this->doFlag = true;
}
