#include "DeleteEdge.h"

#include <cassert>

#include "../OpenVag.h"

void DeleteEdge::doAct()
{
    position = edge->getXmlPosition();
    parent = edge->getParent();
    auto xmlElClone = edge->getXmlElement()->el->DeepClone(edge->getXmlElement()->el->GetDocument());
    parent->deleteEdge(edge);
    edge->getXmlElement()->set(xmlElClone);
    this->doFlag = false;
}

void DeleteEdge::undoAct()
{
    parent->insertEdge(edge, position);
    edge.reset();
    parent.reset();
    position = 0;
    this->doFlag = true;
}
