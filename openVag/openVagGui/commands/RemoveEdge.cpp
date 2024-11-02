#include "RemoveEdge.h"

void RemoveEdge::execute()
{
    if (doFlag) { doAct(); }
    else { undoAct(); }
}

void RemoveEdge::doAct() {
    this->doFlag = false;
    nextEdge = removeEdge->NextSibling();
    removeEdge->Parent()->DeleteChild(removeEdge);
}

void RemoveEdge::undoAct() {
    this->doFlag = true;
    removeEdge->Parent()->InsertBeforeChild(nextEdge, removeEdge);
    nextEdge.reset();
}
