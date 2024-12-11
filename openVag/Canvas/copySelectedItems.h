#pragma once

#include <memory>

#include "../IRModel.h"

void copySelectedItems(const std::shared_ptr<IRModel>& irModel, ax::NodeEditor::NodeId contextNodeId = 0, ax::NodeEditor::LinkId contextLinkId = 0);