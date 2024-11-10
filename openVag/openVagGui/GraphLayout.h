#pragma once
#include <cstdint>
#include <memory>

#include "IRModelGui.h"


class GraphLayout {
public:
	GraphLayout(int64_t rowSpacing, int64_t columnSpacing) : rowSpacing(rowSpacing), columnSpacing(columnSpacing) {};
	void layoutNodes(std::shared_ptr<IRModelGui> irModelGui);
private:
	int64_t rowSpacing;
	int64_t columnSpacing;
};