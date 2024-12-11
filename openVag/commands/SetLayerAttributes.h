#pragma once
#include "ICommand.h"

#include <vector>

#include "../IRModel.h"

class SetLayerAttributes :public ICommand {
public:
	SetLayerAttributes(std::shared_ptr<Layer> layer, const std::vector<std::pair<std::string, std::string>>& vecAttribute) : layer(layer), vecAttribute(vecAttribute) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Layer> layer;
	std::vector<std::pair<std::string, std::string>> vecAttribute;
};

