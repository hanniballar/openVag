#include "../IRModel.h"

#include <unordered_set>
#include <map>
#include <string>
#include <imgui.h>

namespace Canvas {
    void drawLayerNodes(const std::unordered_set<std::shared_ptr<Layer>>& setLayer, const std::map<std::string, ImColor>& mapLayerTypeToColor);
}
