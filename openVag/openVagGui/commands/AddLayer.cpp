#include "AddLayer.h"

#include <cassert>

#include "../OpenVag.h"

void AddLayer::doAct()
{
    layer = irModelGui->getNetwork()->getLayers()->insertNewLayer();
    this->doFlag = false;
}

void AddLayer::undoAct()
{
    irModelGui->getNetwork()->getLayers()->deleteLayer(layer);
    layer.reset();
    this->doFlag = true;
}
