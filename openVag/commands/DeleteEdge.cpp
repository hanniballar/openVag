#include "DeleteEdge.h"

#include <cassert>

#include "../OpenVag.h"

void DeleteEdge::doAct()
{
    position = edge->getXmlPosition();
    parent = edge->getParent();
    auto xmlElClone = edge->getXmlElement()->DeepClone(edge->getXmlElement()->GetDocument());
    parent->deleteEdge(edge);
    edge->setXmlElement(xmlElClone->ToElement());
    this->doFlag = false;
}

void DeleteEdge::undoAct()
{
    parent->insertEdge(edge, position);
    parent.reset();
    position = 0;
    this->doFlag = true;
}
