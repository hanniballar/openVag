#include "CommandCenter.h"

#include <memory>

void CommandCenter::undo()
{
    if (vecUndo.empty()) return;

    auto& command = vecUndo.back();
    command->execute();

    vecRedo.push_back(std::move(command));
    vecUndo.pop_back();
}

void CommandCenter::undoAll() {
    while (!vecUndo.empty()) {
        undo();
    }
}

void CommandCenter::redoAll() {
    while (!vecRedo.empty()) {
        redo();
    }
}

void CommandCenter::redo() {
    if (vecRedo.empty()) return;

    auto& command = vecRedo.back();
    command->execute();
    vecUndo.push_back(std::move(command));
    vecRedo.pop_back();
}

void CommandCenter::execute(std::shared_ptr<ICommand> command) {
    command->execute();
    vecUndo.push_back(command);
    vecRedo.clear();
}
