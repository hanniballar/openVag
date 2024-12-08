#pragma once
#include "ICommand.h"

#include "../IRModelGui.h"
#include "RemoveXMLElement.h"
#include "CommandCenter.h"

class RemoveLayer :public ICommand {
public:
	RemoveLayer(std::shared_ptr<LayerNodeGui> layerNodeGui) : removeLayer(layerNodeGui) {}	
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<LayerNodeGui> removeLayer;
	std::ptrdiff_t positionAsChild = {};
	CommandCenter commandCenter;
};

