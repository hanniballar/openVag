#pragma once
#include "ICommand.h"

#include "../IRModelGui.h"
#include "RemoveEdgeXML.h"

class RemoveEdge :public ICommand {
public:
	RemoveEdge(std::shared_ptr<EdgeGui> edgeGui) : removeEdge(edgeGui), removeEdgeXML(edgeGui->edge) {}
	void execute() override;
private:
	bool doFlag = true;
	void doAct();
	void undoAct();
	std::shared_ptr<EdgeGui> removeEdge;
	std::ptrdiff_t positionAsChild;
	RemoveXMLElement removeEdgeXML;
};

