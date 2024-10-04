#include "InsertLayer.h"

#include <cassert>

#include "../OpenVag.h"

void insertLayer::doAct()
{
    layer = irModelGui->getNetwork()->getLayers()->insertNewLayer();
    this->doFlag = false;
}

void insertLayer::undoAct()
{
    irModelGui->getNetwork()->getLayers()->deleteLayer(layer);
    layer.reset();
    this->doFlag = true;
}
