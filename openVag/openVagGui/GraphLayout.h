#pragma once
#include <cstdint>
#include <memory>

#include "IRModelGui.h"


class GraphLayout {
public:
	GraphLayout(float horizontalSpacing, float verticalSpacing) : horizontalSpacing(horizontalSpacing), verticalSpacing(verticalSpacing) {};
	void layoutNodes(std::shared_ptr<IRModelGui> irModelGui);
private:
	float horizontalSpacing;
	float verticalSpacing;
};