#pragma once
#include "ICommand.h"

#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class RemoveXMLElement :public ICommand {
public:
	RemoveXMLElement(std::shared_ptr<XMLNodeWrapper> xmlElement) : xmlElement(xmlElement) {}
	RemoveXMLElement(tinyxml2::XMLElement* xmlElement) : RemoveXMLElement(XMLNodeWrapper::make_shared(xmlElement)) {} //ToDo check if needed

private:	
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
	std::shared_ptr<XMLNodeWrapper> prevElement;
	std::shared_ptr<XMLNodeWrapper> parentNode; //Even if prevElement might hold this value prevElement might be null when xmlElement is first or only child
};

