#include "AddLayer.h"

#include <cassert>

#include "../OpenVag.h"

void AddLayer::doAct()
{
    undoLayer = irModelGui->getNetwork()->getLayers()->insertNewLayer();
    this->doFlag = false;
}

void AddLayer::undoAct()
{
    irModelGui->getNetwork()->getLayers()->deleteLayer(undoLayer);
    undoLayer.reset();
    this->doFlag = true;
}
