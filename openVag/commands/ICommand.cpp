#include "ICommand.h"

void ICommand::execute()
{
    if (doFlag) { doAct(); }
    else { undoAct(); }
}
