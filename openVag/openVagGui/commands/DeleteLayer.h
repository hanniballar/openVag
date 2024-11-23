#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "CommandCenter.h"


class DeleteLayer :public ICommand {
public:
	DeleteLayer(std::shared_ptr<Layer> layer) : layer(layer) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Layers> parent;
	std::shared_ptr<Layer> layer;
	size_t position = 0;
	CommandCenter commandCenter;
};

