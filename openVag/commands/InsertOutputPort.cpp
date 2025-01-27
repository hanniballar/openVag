#include "InsertOutputPort.h"

#include "../OpenVag.h"

void InsertOutputPort::doAct() {
    port = layer->insertNewOutputPort();
    this->doFlag = false;
}

void InsertOutputPort::undoAct() {
    layer->deletePort(port);
    this->doFlag = true;
}
