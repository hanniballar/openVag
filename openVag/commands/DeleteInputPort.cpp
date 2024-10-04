#include "DeleteInputPort.h"

#include <cassert>

#include "../OpenVag.h"
#include "DeleteEdge.h"

void DeleteInputPort::doAct()
{
    position = port->getXmlPosition();
    parent = port->getParent();
    for (const auto edge : port->getSetEdges()) { commandCenter.execute(std::make_shared<DeleteEdge>(edge)); }
    auto xmlElClone = port->getXmlElement()->el->DeepClone(port->getXmlElement()->el->GetDocument());
    parent->deletePort(port);
    port->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteInputPort::undoAct()
{
    parent->insertPort(port, position);
    commandCenter.undoAll();
    commandCenter.reset();
    port.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
