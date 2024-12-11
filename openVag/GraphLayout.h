#pragma once
#include <cstdint>
#include <memory>

#include "IRModel.h"

class GraphLayout {
public:
	GraphLayout(float horizontalSpacing, float verticalSpacing) : horizontalSpacing(horizontalSpacing), verticalSpacing(verticalSpacing) {};
	void layoutNodes(std::set<std::shared_ptr<Layer>, LayerIDLess> layers, ImVec2 startPos = {0, 0});
private:
	float horizontalSpacing;
	float verticalSpacing;
};