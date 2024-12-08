#pragma once
#include "ICommand.h"

#include "../IRModel.h"
#include "CommandCenter.h"


class InsertEdge :public ICommand {
public:
	InsertEdge(std::shared_ptr<IRModel> irModelGui, std::string from_layer, std::string from_port, std::string to_layer, std::string to_port) : 
		irModelGui(irModelGui), from_layer(from_layer), from_port(from_port), to_layer(to_layer), to_port(to_port) {}
	InsertEdge(std::shared_ptr<IRModel> irModelGui, std::shared_ptr<OutputPort> outputPort, std::shared_ptr<InputPort> inputPort) :
		irModelGui(irModelGui), from_layer(outputPort->getParent()->getXmlId()), from_port(outputPort->getXmlId()), to_layer(inputPort->getParent()->getXmlId()), to_port(inputPort->getXmlId()) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<IRModel> irModelGui;
	std::string from_layer;
	std::string from_port;
	std::string to_layer;
	std::string to_port;
	std::shared_ptr<Edge> undoEdge;
};

