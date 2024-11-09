#pragma once
#include "ICommand.h"

#include "../IRModelGui.h"
#include "RemoveXMLElement.h"

class RemoveEdge :public ICommand {
public:
	RemoveEdge(std::shared_ptr<EdgeGui> edgeGui) : removeEdge(edgeGui), removeXMLElement(edgeGui->xmlElement) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<EdgeGui> removeEdge;
	std::ptrdiff_t positionAsChild = {};
	RemoveXMLElement removeXMLElement;
};

