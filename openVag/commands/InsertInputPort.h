#pragma once
#include "ICommand.h"

#include "../IRModel.h"

class InsertInputPort :public ICommand {
public:
	InsertInputPort(std::shared_ptr<Layer> layer) : layer(layer) {}
	std::shared_ptr<InputPort> getInputPort() const { return port; }
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Layer> layer;
	std::shared_ptr<InputPort> port;
};

