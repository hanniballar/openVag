#pragma once
#include "ICommand.h"

#include <map>
#include <string>

#include "../IRModelGui.h"
#include "CommandCenter.h"


class ModifyEdge :public ICommand {
public:
	ModifyEdge(std::shared_ptr<EdgeGui> edgeGui, std::map<std::string, std::string> mapChangeAttr) : modifyEdge(edgeGui), mapChangeAttr(mapChangeAttr) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<LayerInputPortGui> prevInputPort;
	std::shared_ptr<LayerOutputPortGui> prevOutputPort;
	std::shared_ptr<EdgeGui> modifyEdge;
	std::map<std::string, std::string> mapChangeAttr;
	CommandCenter commandCenter;
};

