#pragma once
#include "ICommand.h"

#include <map>
#include <vector>
#include <string>

#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class AddAttributeXMLElement :public ICommand {
public:
	AddAttributeXMLElement(std::shared_ptr<XMLNodeWrapper> xmlElement, std::map<std::string, std::string> mapAttr) : xmlElement(xmlElement), mapAttr(mapAttr) {}
private:
	void doAct() override;
	void undoAct() override;
	std::vector<std::string> vecDeleteAttr;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
	std::map<std::string, std::string> mapAttr;
};

