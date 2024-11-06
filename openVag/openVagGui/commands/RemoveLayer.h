#pragma once
#include "ICommand.h"

#include "../IRModelGui.h"
#include "RemoveXMLElement.h"
#include "CommandCenter.h"

class RemoveLayer :public ICommand {
public:
	RemoveLayer(std::shared_ptr<LayerNodeGui> layerNodeGui) : removeLayer(layerNodeGui) {}
	void execute() override;
private:
	bool doFlag = true;
	void doAct();
	void undoAct();
	std::shared_ptr<LayerNodeGui> removeLayer;
	std::ptrdiff_t positionAsChild = {};
	CommandCenter commandCenter;
};

