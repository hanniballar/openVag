#pragma once
#include "ICommand.h"

#include "../IRModel.h"

class InsertOutputPort :public ICommand {
public:
	InsertOutputPort(std::shared_ptr<Layer> layer) : layer(layer) {}
	std::shared_ptr<OutputPort> getInputPort() const { return port; }
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Layer> layer;
	std::shared_ptr<OutputPort> port;
};

