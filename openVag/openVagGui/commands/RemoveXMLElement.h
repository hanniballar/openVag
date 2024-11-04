#pragma once
#include "ICommand.h"

#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class RemoveXMLElement :public ICommand {
public:
	RemoveXMLElement(std::shared_ptr<XMLNodeWrapper> xmlElement) : xmlElement(xmlElement) {}
	RemoveXMLElement(tinyxml2::XMLElement* xmlElement) : RemoveXMLElement(XMLNodeWrapper::make_shared(xmlElement)) {}
	void execute() override;

private:
	bool doFlag = true;
	void doAct();
	void undoAct();
	std::shared_ptr<XMLNodeWrapper> xmlElement;
	std::shared_ptr<XMLNodeWrapper> prevElement;
	std::shared_ptr<XMLNodeWrapper> parentNode;
};

