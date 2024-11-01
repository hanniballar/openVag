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
private:
	std::vector<std::shared_ptr<ICommand>> vecUndo;
	std::vector<std::shared_ptr<ICommand>> vecRedo;
};