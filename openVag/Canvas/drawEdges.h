#pragma once
#include "../IRModel.h"

#include "unordered_set"

namespace Canvas {
    void drawModelEdges(const std::unordered_set<std::shared_ptr<Edge>>& setEdge);
}
