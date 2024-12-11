#pragma once
#include "tinyxml2.h"

#include "../IRModel.h"

#include "ICommand.h"
#include "CommandCenter.h"

class InsertLayer :public ICommand {
public:
	InsertLayer(std::shared_ptr<IRModel> irModelGui);
	InsertLayer(std::shared_ptr<IRModel> irModelGui, tinyxml2::XMLElement* xmlElement);
	std::shared_ptr<Layer> getLayer() const { return layer; }
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<IRModel> irModelGui;
	std::shared_ptr<Layer> layer;
	CommandCenter commandCenter;
};

