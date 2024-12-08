#include "RemoveEdge.h"

void RemoveEdge::doAct() {
    this->doFlag = false;
    removeXMLElement.execute();
    positionAsChild = removeEdge->getMyPositionAsChild();
    removeEdge->Parent()->deleteChild(removeEdge);
}

void RemoveEdge::undoAct() {
    this->doFlag = true;
    removeXMLElement.execute();
    removeEdge->Parent()->insert(positionAsChild, removeEdge);
    positionAsChild = {};
}
