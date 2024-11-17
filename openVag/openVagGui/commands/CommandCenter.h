#pragma once
#include <vector>
#include <memory>

#include "ICommand.h"
#include "tinyxml2.h"

class CommandCenter {
public:
	CommandCenter() {}
	void undo();
	void undoAll();
	void redo();
	void redoAll();
	void execute(std::shared_ptr<ICommand> command);
	size_t getUndoSize() { return vecUndo.size(); }
	size_t getRedoSize() { return vecRedo.size(); }
private:
	std::vector<std::shared_ptr<ICommand>> vecUndo;
	std::vector<std::shared_ptr<ICommand>> vecRedo;
};