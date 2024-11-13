#pragma once
#include "ICommand.h"

#include <string>
#include "tinyxml2.h"

#include "../XMLNodeWrapper.h"

class AddXMLEdge :public ICommand {
public:
	AddXMLEdge(std::string from_layer, std::string from_port, std::string to_layer, std::string to_port, std::shared_ptr<XMLNodeWrapper> xmlElementEdges) : 
		from_layer(from_layer),
		from_port(from_port),
		to_layer(to_layer),
		to_port(to_port),
		xmlElementEdges(xmlElementEdges) {}

	std::shared_ptr<XMLNodeWrapper> getXMLElement() { return xmlElement; }

private:
	void doAct() override;
	void undoAct() override;

	std::string from_layer;
	std::string from_port;
	std::string to_layer;
	std::string to_port;
	std::shared_ptr<XMLNodeWrapper> xmlElementEdges;

	std::shared_ptr<XMLNodeWrapper> xmlElement;

};

