#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "CommandCenter.h"

class DeleteInputPort :public ICommand {
public:
	DeleteInputPort(std::shared_ptr<InputPort> inputPort) : port(inputPort) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<InputPort> port;
	std::shared_ptr<Layer> parent;
	size_t position = 0;
	CommandCenter commandCenter;
};

