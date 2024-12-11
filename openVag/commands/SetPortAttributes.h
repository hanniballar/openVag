#pragma once
#include "ICommand.h"

#include <vector>

#include "../IRModel.h"

class SetPortAttributes :public ICommand {
public:
	SetPortAttributes(std::shared_ptr<Port> port, const std::vector<std::pair<std::string, std::string>>& vecAttribute) : port(port), vecAttribute(vecAttribute) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Port> port;
	std::vector<std::pair<std::string, std::string>> vecAttribute;
};

