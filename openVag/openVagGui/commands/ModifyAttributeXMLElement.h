#pragma once
#include "ICommand.h"

#include <map>
#include <string>

#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class ModifyAttributeXMLElement :public ICommand {
public:
	ModifyAttributeXMLElement(std::shared_ptr<XMLNodeWrapper> xmlElement, std::map<std::string, std::string> mapChangeAttr) : xmlElement(xmlElement), mapChangeAttr(mapChangeAttr) {}

private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
	std::map<std::string, std::string> mapChangeAttr;
};

