#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "RemoveXMLElement.h"

class RemoveEdge :public ICommand {
public:
	RemoveEdge(std::shared_ptr<Edge> edgeGui) : removeEdge(edgeGui), removeXMLElement(edgeGui->getXmlElement()) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Edge> removeEdge;
	std::ptrdiff_t positionAsChild = {};
	RemoveXMLElement removeXMLElement;
};

