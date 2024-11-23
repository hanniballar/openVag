#include "InsertInputPort.h"

#include <cassert>

#include "../OpenVag.h"

void InsertInputPort::doAct() {
    port = layer->insertNewInputPort();
    this->doFlag = false;
}

void InsertInputPort::undoAct() {
    layer->deletePort(port);
    layer.reset();
    this->doFlag = true;
}
