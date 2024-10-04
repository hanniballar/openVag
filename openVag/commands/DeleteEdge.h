#pragma once
#include "ICommand.h"

#include "../IRModel.h"

class DeleteEdge :public ICommand {
public:
	DeleteEdge(std::shared_ptr<Edge> edge) : edge(edge) {}
private:
	void doAct() override;
	void undoAct() override;
	std::shared_ptr<Edge> edge;
	std::shared_ptr<Edges> parent;
	size_t position = 0;
};

