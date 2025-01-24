#include "../IRModel.h"

#include "unordered_set"

std::unordered_set<std::shared_ptr<Layer>> calcLayersToDraw(const std::shared_ptr<Layers>& layers, bool forceDrawAllNodes = true);
std::unordered_set<std::shared_ptr<Edge>> calcEdgesToDraw(const std::unordered_set<std::shared_ptr<Layer>>& setLayer);