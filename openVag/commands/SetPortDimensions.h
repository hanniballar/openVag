#pragma once
#include "ICommand.h"

#include <vector>

#include "../IRModel.h"

class SetPortDimensions :public ICommand {
public:
	SetPortDimensions(std::shared_ptr<Port> port, const std::vector<std::string>& vecDim);
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Port> port;
	std::vector<std::string> vecDim;
	std::vector<size_t> vecInitialDimPos; // Stors the position of all dim XMLNodes. All values are sorted asc
};

