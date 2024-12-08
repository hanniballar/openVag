#pragma once

#include "../commands/CommandCenter.h"
#include "../IRModel.h"

namespace Canvas {
    void beginCreate(std::shared_ptr<IRModel> irModelGui, CommandCenter& commandCenter);
}