#include "DeleteOutputPort.h"

#include <cassert>

#include "../OpenVag.h"
#include "DeleteEdge.h"

void DeleteOutputPort::doAct()
{
    position = port->getXmlPosition();
    parent = port->getParent();
    auto edges = port->getSetEdges();
    for (const auto edge : edges) {
        commandCenter.execute(std::make_shared<DeleteEdge>(edge));
    }
    auto xmlElClone = port->getXmlElement()->el->DeepClone(port->getXmlElement()->el->GetDocument());
    parent->deletePort(port);
    port->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteOutputPort::undoAct()
{
    commandCenter.undoAll();
    commandCenter.reset();
    parent->addPort(port, position);
    port.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
