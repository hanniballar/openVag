#pragma once
#include "IRModelGui.h"
#include "IRXmlRep.h"

void drawModelEdges(std::vector<LayerNodeGui>& vecLayerNodeGui);
bool createModelEdgesGui(std::vector<LayerNodeGui>& vecLayerNodeGui, const std::vector<Edge> vecEdge);
