#pragma once

#include "../IRModel.h"

#include "../commands/CommandCenter.h"
#include "showCanvas.h"

namespace Canvas {
    void contextMenu(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter, RelayoutType& relayoutType);
}