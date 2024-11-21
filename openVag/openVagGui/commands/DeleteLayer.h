#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "CommandCenter.h"


class DeleteLayer :public ICommand {
public:
	DeleteLayer(std::shared_ptr<IRModel> irModelGui, std::shared_ptr<Layer> layer) : irModelGui(irModelGui), undoLayer(layer) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<IRModel> irModelGui;
	std::shared_ptr<Layer> undoLayer;
	size_t position = 0;
};

