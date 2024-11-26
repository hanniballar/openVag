#pragma once
#include <cstdint>
#include <memory>

#include "IRModel.h"

class GraphLayout {
public:
	GraphLayout(float horizontalSpacing, float verticalSpacing) : horizontalSpacing(horizontalSpacing), verticalSpacing(verticalSpacing) {};
	void layoutNodes(const std::shared_ptr<IRModel>& irModelGui);
private:
	float horizontalSpacing;
	float verticalSpacing;
};