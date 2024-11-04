#pragma once
#include "ICommand.h"

#include "../IRModelGui.h"
#include "RemoveXMLElement.h"
#include "CommandCenter.h"

class RemoveLayer :public ICommand {
public:
	RemoveLayer(std::shared_ptr<LayerNodeGui> layerNodeGui) : removeLayer(layerNodeGui), removeXMLElement(layerNodeGui->xmlLayer) {}
	void execute() override;
private:
	bool doFlag = true;
	void doAct();
	void undoAct();
	std::shared_ptr<LayerNodeGui> removeLayer;
	std::ptrdiff_t positionAsChild = {};
	RemoveXMLElement removeXMLElement;
	CommandCenter commandCenter;
};

