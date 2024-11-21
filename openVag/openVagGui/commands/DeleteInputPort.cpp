#include "DeleteInputPort.h"

#include <cassert>

#include "../OpenVag.h"

void DeleteInputPort::doAct()
{
    position = port->getXmlPosition();
    parent = port->getParent();
    auto xmlElClone = port->getXmlElement()->el->DeepClone(port->getXmlElement()->el->GetDocument());
    port->getParent()->deletePort(port);
    port->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteInputPort::undoAct()
{
    parent->addPort(port);
    port.reset();
    parent.reset();
    position = 0;
}
