#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "CommandCenter.h"

class DeleteOutputPort :public ICommand {
public:
	DeleteOutputPort(std::shared_ptr<IRModel> irModelGui, std::shared_ptr<OutputPort> outputPort) : irModelGui(irModelGui), port(outputPort) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<IRModel> irModelGui;
	std::shared_ptr<OutputPort> port;
	std::shared_ptr<Layer> parent;
	size_t position = 0;
};

