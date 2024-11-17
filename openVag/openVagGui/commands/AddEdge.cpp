#include "AddEdge.h"

#include <cassert>

#include "../OpenVag.h"

void AddEdge::doAct()
{
    undoEdge = irModelGui->getNetwork()->getEdges()->insertNewEdge(GetNextId(), from_layer, from_port, to_layer, to_port);
    this->doFlag = false;
}

void AddEdge::undoAct()
{
    irModelGui->getNetwork()->getEdges()->deleteEdge(undoEdge);
    undoEdge.reset();
    this->doFlag = true;
}
