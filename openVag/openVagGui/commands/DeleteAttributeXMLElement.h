#pragma once
#include "ICommand.h"

#include <map>
#include <vector>
#include <string>

#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"
#include "AddAttributeXMLElement.h"

class DeleteAttributeXMLElement : public ICommand {
public:
	DeleteAttributeXMLElement(std::shared_ptr<XMLNodeWrapper> xmlElement, std::vector<std::string> vecDeleteAttr) : xmlElement(xmlElement), vecDeleteAttr(vecDeleteAttr) {}

private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<XMLNodeWrapper> xmlElement;
	std::vector<std::string> vecDeleteAttr;
	struct AttrData {
		AttrData(std::string name, std::string value) : name(name), value(value) {};
		std::string name;
		std::string value;
	};
	std::vector<AttrData> vecUndoDeleteAttrData;
};

