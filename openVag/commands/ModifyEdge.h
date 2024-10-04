#pragma once
#include "ICommand.h"

#include <map>

#include "../IRModel.h"
#include "CommandCenter.h"


class ModifyEdge :public ICommand {
public:
	ModifyEdge(std::shared_ptr<Edge> edge, std::map<std::string, std::string> mapAttribute) : edge(edge), mapAttribute(mapAttribute) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Edge> edge;
	std::map<std::string, std::string> mapAttribute;
	CommandCenter commandCenter;
};

