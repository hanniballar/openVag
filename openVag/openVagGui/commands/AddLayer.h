#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "CommandCenter.h"


class AddLayer :public ICommand {
public:
	AddLayer(std::shared_ptr<IRModel> irModelGui) : irModelGui(irModelGui) {}
	std::shared_ptr<Layer> getLayer() const { return layer; }
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<IRModel> irModelGui;
	std::shared_ptr<Layer> layer;
};

