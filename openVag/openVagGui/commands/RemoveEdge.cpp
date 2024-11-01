#include "RemoveEdge.h"

void RemoveEdge::execute()
{
    if (doFlag) { doAct(); }
    else { undoAct(); }
}

void RemoveEdge::doAct() {
}

void RemoveEdge::undoAct() {

}
