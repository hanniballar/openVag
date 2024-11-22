#include "DeleteOutputPort.h"

#include <cassert>

#include "../OpenVag.h"

void DeleteOutputPort::doAct()
{
    position = port->getXmlPosition();
    parent = port->getParent();
    auto xmlElClone = port->getXmlElement()->el->DeepClone(port->getXmlElement()->el->GetDocument());
    parent->deletePort(port);
    port->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteOutputPort::undoAct()
{
    parent->addPort(port, position);
    port.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
