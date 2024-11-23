#include "InsertInputPort.h"

#include <cassert>

#include "../OpenVag.h"

void InsertInputPort::doAct() {
    layer = layer->insertNewLayer();
    this->doFlag = false;
}

void InsertInputPort::undoAct() {
    irModelGui->getNetwork()->getLayers()->deleteLayer(layer);
    layer.reset();
    this->doFlag = true;
}
