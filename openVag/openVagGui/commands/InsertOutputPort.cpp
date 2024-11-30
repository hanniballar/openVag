#include "InsertOutputPort.h"

#include <cassert>

#include "../OpenVag.h"

void InsertOutputPort::doAct() {
    port = layer->insertNewOutputPort();
    this->doFlag = false;
}

void InsertOutputPort::undoAct() {
    layer->deletePort(port);
    layer.reset();
    this->doFlag = true;
}
