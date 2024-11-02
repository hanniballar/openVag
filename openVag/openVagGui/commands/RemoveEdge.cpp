#include "RemoveEdge.h"

void RemoveEdge::execute()
{
    if (doFlag) { doAct(); }
    else { undoAct(); }
}

void RemoveEdge::doAct() {
    this->doFlag = false;
    removeEdgeXML.execute();
    positionAsChild = removeEdge->getMyPositionAsChild();
    removeEdge->Parent()->deleteChild(removeEdge);
}

void RemoveEdge::undoAct() {
    this->doFlag = true;
    removeEdgeXML.execute();
    removeEdge->Parent()->insert(positionAsChild, removeEdge);
    positionAsChild = {};
}
