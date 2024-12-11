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
	void reset() { vecUndo.clear(); vecRedo.clear(); };
	void execute(std::shared_ptr<ICommand> command);
	void add(std::shared_ptr<ICommand> command) { vecRedo.insert(vecRedo.begin(), command); }
	size_t getUndoSize() { return vecUndo.size(); }
	size_t getRedoSize() { return vecRedo.size(); }
private:
	std::vector<std::shared_ptr<ICommand>> vecUndo;
	std::vector<std::shared_ptr<ICommand>> vecRedo;
};