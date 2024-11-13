#pragma once
#include "ICommand.h"

#include "../IRModelGui.h"
#include "CommandCenter.h"


class AddEdge :public ICommand {
public:
	AddEdge(std::shared_ptr<EdgeGui> edgeGui) : addEdge(edgeGui) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<EdgeGui> addEdge;
	CommandCenter commandCenter;
};

