#pragma once
#include "ICommand.h"

#include "CommandCenter.h"

class ComposedCommand :public ICommand {
public:
	ComposedCommand(const CommandCenter& commandCenter) : commandCenter(commandCenter) {}
private:
	void doAct() override;
	void undoAct() override;
	CommandCenter commandCenter;
};

