#include "DeleteOutputPort.h"

#include <cassert>

#include "../OpenVag.h"
#include "DeleteEdge.h"

void DeleteOutputPort::doAct()
{
    position = port->getXmlPosition();
    parent = port->getParent();
    for (const auto edge : port->getSetEdge()) { commandCenter.execute(std::make_shared<DeleteEdge>(edge)); }
    auto xmlElClone = port->getXmlElement()->el->DeepClone(port->getXmlElement()->el->GetDocument());
    parent->deletePort(port);
    port->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteOutputPort::undoAct()
{
    parent->insertPort(port, position);
    commandCenter.undoAll();
    commandCenter.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
