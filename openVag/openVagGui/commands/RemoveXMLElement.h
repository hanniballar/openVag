#pragma once
#include "ICommand.h"

#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class RemoveXMLElement :public ICommand {
public:
	RemoveXMLElement(std::shared_ptr<XMLNodeWrapper> removeEdge) : removeEdge(removeEdge) {}
	RemoveXMLElement(tinyxml2::XMLElement* edge) : RemoveXMLElement(XMLNodeWrapper::make_shared(edge)) {}
	void execute() override;

private:
	bool doFlag = true;
	void doAct();
	void undoAct();
	std::shared_ptr<XMLNodeWrapper> removeEdge;
	std::shared_ptr<XMLNodeWrapper> prevEdge;
	std::shared_ptr<XMLNodeWrapper> parentNode;
};

