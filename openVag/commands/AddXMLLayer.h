#pragma once
#include "ICommand.h"

#include <string>
#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class AddXMLLayer :public ICommand {
public:
	AddXMLLayer(std::string id, std::string name, std::string type, std::shared_ptr<XMLNodeWrapper> xmlElementLayers) :
		id(id),
		name(name),
		type(type),
		xmlElementLayers(xmlElementLayers) {}

private:
	void doAct() override;
	void undoAct() override;

	std::string id;
	std::string name;
	std::string type;
	std::shared_ptr<XMLNodeWrapper> xmlElementLayers;

	//For undo
	std::shared_ptr<XMLNodeWrapper> xmlElement;

};

