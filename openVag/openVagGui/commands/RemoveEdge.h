#pragma once
#include "ICommand.h"

#include "tinyxml2.h"

#include "XMLNodeWrapper.h"
#include "../IRModelGui.h"

class RemoveEdge :public ICommand {
public:
	RemoveEdge(std::shared_ptr<EdgeGui> edgeGui) : removeEdge(edgeGui) {}
	void execute() override;
private:
	bool doFlag = true;
	void doAct();
	void undoAct();
	std::shared_ptr<EdgeGui> removeEdge;
	std::shared_ptr<XMLNodeWrapper> prevEdge;
	std::shared_ptr<XMLNodeWrapper> parentNode;
};

