#include "InsertEdge.h"

void InsertEdge::doAct()
{
    undoEdge = irModelGui->getNetwork()->getEdges()->insertNewEdge(from_layer, from_port, to_layer, to_port);
    this->doFlag = false;
}

void InsertEdge::undoAct()
{
    irModelGui->getNetwork()->getEdges()->deleteEdge(undoEdge);
    undoEdge.reset();
    this->doFlag = true;
}
