#include "ComposedCommand.h"

void ComposedCommand::doAct() {
    commandCenter.redoAll();
    this->doFlag = false;
}

void ComposedCommand::undoAct() {
    commandCenter.undoAll();
    this->doFlag = true;
}
