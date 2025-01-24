#pragma once
#include <cstdint>
#include <memory>

#include "IRModel.h"

class GraphLayout {
public:
	GraphLayout(ImVec2 spacing) : spacing(spacing) {};
	void layoutNodes(std::set<std::shared_ptr<Layer>, LayerIDLess> layers, ImVec2 pos = {0, 0});
private:
	ImVec2 spacing;
};