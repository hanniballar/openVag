#include "GraphLayout.h"

#include <list>
#include <map>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <cassert>
#include <set>
#include <deque>
#include <algorithm>

#include "imgui.h"

class BiMapConstLayerToOwnerLayer
{
public:
    void insertConstToOwner(std::shared_ptr<Layer> constLayer, std::shared_ptr<Layer> ownerLayer) {
        mapConstLayerToOwnerLayer[constLayer] = ownerLayer;
        mapOwnerLayerToVecConstLayer[ownerLayer].push_back(constLayer);
    }
    std::vector<std::shared_ptr<Layer>> getVecConstLayerForOwner(std::shared_ptr<Layer> ownerLayer) const {
        auto it = mapOwnerLayerToVecConstLayer.find(ownerLayer);
        if (it == mapOwnerLayerToVecConstLayer.end()) return {};
        return it->second;
    }
    std::shared_ptr<Layer> getOwnerLayerForConstLayer(std::shared_ptr<Layer> constLayer) const {
        auto it = mapConstLayerToOwnerLayer.find(constLayer);
        if (it == mapConstLayerToOwnerLayer.end()) return nullptr;
        return it->second;
    }
    std::vector<std::shared_ptr<Layer>> getAllConstLayers() {
        std::vector<std::shared_ptr<Layer>> res;
        for (const auto& mapVal : mapConstLayerToOwnerLayer) {
            res.push_back(mapVal.first);
        }
        return res;
    }

private:
    std::map<std::shared_ptr<Layer>, std::shared_ptr<Layer>> mapConstLayerToOwnerLayer;
    std::map<std::shared_ptr<Layer>, std::vector<std::shared_ptr<Layer>>> mapOwnerLayerToVecConstLayer;
};

class Row {
public:
    std::list<std::shared_ptr<Layer>> vecLayer;
    ImVec2 getSize(float horizontalSpacing, float verticalSpacing, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) const;
    void layoutLayers(float horizontalSpacing, float verticalSpacing, float leftmargin, float horizontalOffset, float verticalOffset, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) const;
};

class Graph {
public:
    Graph(const std::shared_ptr<Layer>& startLayer);
    std::set<std::shared_ptr<Layer>> getAllLayers();
    void layoutLayers(float horizontalSpacing, float verticalSpacing, ImVec2 startPos);
    ImVec2 getSize(float horizontalSpacing, float verticalSpacing, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) const;
    BiMapConstLayerToOwnerLayer biMapConstLayerToOwnerLayer;
private:
    std::list<Row> listRow;
    std::list<Row>::iterator getItRow(std::shared_ptr<Layer>);
    std::list<Row>::iterator makePrevRowAvailable(std::list<Row>::iterator it);
    std::list<Row>::iterator makeNextRowAvailable(std::list<Row>::iterator it);
    std::list<Row>::iterator makeParameterLayerRowAvailable();
    std::list<Row>::iterator makeResultLayerRowAvailable();
    std::list<Row>::iterator getDedicatedRow(std::shared_ptr<Layer> layer);
    std::list<Row>::iterator itInputLayerRow = listRow.end();
    std::list<Row>::iterator itResultLayerRow = listRow.end();

};

Graph::Graph(const std::shared_ptr<Layer>& startLayer) {

    std::deque<std::shared_ptr<Layer>> dequeProcLayer = { startLayer };
    std::set<std::shared_ptr<Layer>> setSeenLayers;
    {
        auto startLayerTemp = startLayer;
        if (std::string(startLayerTemp->getType()) == "Const") {
            auto setOutputLayer = startLayerTemp->getOutputLayers();
            if (setOutputLayer.size()) {
                dequeProcLayer.clear();
                dequeProcLayer.push_back(*setOutputLayer.begin());
                startLayerTemp = *setOutputLayer.begin();
            }
        }
        auto itDedicatedRow = getDedicatedRow(startLayerTemp);
        auto itInsertRow = itDedicatedRow != listRow.end() ? itDedicatedRow : listRow.insert(listRow.begin(), Row());
        itInsertRow->vecLayer.push_back(startLayerTemp);
    }
    while (dequeProcLayer.size()) {
        auto procLayer = dequeProcLayer.front(); dequeProcLayer.pop_front();
        if (setSeenLayers.find(procLayer) != setSeenLayers.end()) continue;

        setSeenLayers.insert(procLayer);
        auto skipLayer = [&](const std::shared_ptr<Layer>& layer) {
                if (setSeenLayers.find(layer) != setSeenLayers.end()) return true;
                if (std::find(dequeProcLayer.begin(), dequeProcLayer.end(), layer) != dequeProcLayer.end()) return true;
                return false;
            };

        auto itRow = [&]() {
            auto activeLayer = procLayer;
            if (std::string(activeLayer->getType()) == "Const") {
                activeLayer = biMapConstLayerToOwnerLayer.getOwnerLayerForConstLayer(activeLayer);
                assert(activeLayer != nullptr);
            }
            return getItRow(activeLayer);
            }();
        assert(itRow != listRow.end());
        {
            auto setOutputLayer = procLayer->getOutputLayers();
            for (const auto& layer : setOutputLayer) {
                if (skipLayer(layer)) continue;
                dequeProcLayer.push_back(layer);
                if (std::string(layer->getType()) == "Const") {
                    biMapConstLayerToOwnerLayer.insertConstToOwner(layer, procLayer);
                    continue;
                }
                auto itDedicatedRow = getDedicatedRow(layer);
                auto itInsertRow = itDedicatedRow != listRow.end() ? itDedicatedRow : makeNextRowAvailable(itRow);
                itInsertRow->vecLayer.push_back(layer);
                
            }
        }
        {
            auto setInputLayer = procLayer->getInputLayers();
            for (const auto& layer : setInputLayer) {
                if (skipLayer(layer)) continue;
                dequeProcLayer.push_back(layer);
                if (std::string(layer->getType()) == "Const") {
                    if (biMapConstLayerToOwnerLayer.getOwnerLayerForConstLayer(layer) == nullptr) {
                        biMapConstLayerToOwnerLayer.insertConstToOwner(layer, procLayer);
                    }
                    continue;
                }
                auto itDedicatedRow = getDedicatedRow(layer);
                auto itInsertRow = itDedicatedRow != listRow.end() ? itDedicatedRow : makePrevRowAvailable(itRow);
                itInsertRow->vecLayer.push_back(layer);
            }
        }
    }
}

std::set<std::shared_ptr<Layer>> Graph::getAllLayers()
{
    std::set<std::shared_ptr<Layer>> setLayers;
    for (const auto& row : listRow) { setLayers.insert(row.vecLayer.begin(), row.vecLayer.end()); }
    auto vecConstLayers = biMapConstLayerToOwnerLayer.getAllConstLayers();
    setLayers.insert(vecConstLayers.begin(), vecConstLayers.end());
    return setLayers;
}

void Graph::layoutLayers(float horizontalSpacing, float verticalSpacing, ImVec2 startPos)
{
    auto graphSize = getSize(horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
    float verticalOffset = 0;
    for (const auto& row : listRow) {
        auto rowSize = row.getSize(horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
        auto leftMargin = (graphSize.x - rowSize.x) / 2;
        row.layoutLayers(horizontalSpacing, verticalSpacing, leftMargin, startPos.x, startPos.y + verticalOffset, biMapConstLayerToOwnerLayer);
        verticalOffset += verticalSpacing + rowSize.y;
    }
}

ImVec2 Graph::getSize(float horizontalSpacing, float verticalSpacing, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) const
{
    ImVec2 graphSize;
    for (auto& row : listRow) {
        auto rowSize = row.getSize(horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
        graphSize.x = std::max(graphSize.x, rowSize.x);
        graphSize.y += rowSize.y + verticalSpacing;
    }
    graphSize.y -= verticalSpacing; //Last row does not needd verticalSpacing
    return graphSize;
}

std::list<Row>::iterator Graph::getItRow(std::shared_ptr<Layer> searchLayer)
{
    for (auto itRow = listRow.begin(); itRow != listRow.end(); ++itRow) {
        if (std::find(itRow->vecLayer.begin(), itRow->vecLayer.end(), searchLayer) != itRow->vecLayer.end()) {
            return itRow;
        }
    }

    return listRow.end();
}

std::list<Row>::iterator Graph::makePrevRowAvailable(std::list<Row>::iterator it)
{
    if (it == itInputLayerRow) {
        return listRow.insert(std::next(itInputLayerRow), Row());
    }
    if (it == listRow.begin()) {
        return listRow.insert(listRow.begin(), Row());
    }
    if (std::prev(it) == itInputLayerRow) {
        return listRow.insert(std::next(itInputLayerRow), Row());
    }
    return std::prev(it);
}

std::list<Row>::iterator Graph::makeNextRowAvailable(std::list<Row>::iterator it)
{
    auto nextIt = ++it;

    if (nextIt != itResultLayerRow) { return nextIt; }
    return listRow.insert(itResultLayerRow, Row());
}

std::list<Row>::iterator Graph::makeParameterLayerRowAvailable()
{
    if (itInputLayerRow != listRow.end()) return itInputLayerRow;
    itInputLayerRow = listRow.insert(listRow.begin(), Row());
    return itInputLayerRow;
}

std::list<Row>::iterator Graph::makeResultLayerRowAvailable()
{
    if (itResultLayerRow != listRow.end()) return itResultLayerRow;
    itResultLayerRow = listRow.insert(listRow.end(), Row());
    return itResultLayerRow;
}

std::list<Row>::iterator Graph::getDedicatedRow(std::shared_ptr<Layer> layer)
{
    const auto layerType = std::string(layer->getType());
    if (layerType == "Parameter")
        return makeParameterLayerRowAvailable();
    if (layerType == "Result")
        return makeResultLayerRowAvailable();
    return listRow.end();
}

void GraphLayout::layoutNodes(std::set<std::shared_ptr<Layer>, LayerIDLess> layers, ImVec2 startPos) {
    std::vector<Graph> vecGraph;

    while (layers.size()) {
        auto layer = *(layers.begin());
        Graph graph(layer);
        vecGraph.push_back(graph);
        for (auto& layer : graph.getAllLayers()) layers.erase(layer);
    }

    for (auto& graph : vecGraph) {
        graph.layoutLayers(horizontalSpacing, verticalSpacing, startPos);
        startPos.x += (graph.getSize(horizontalSpacing, verticalSpacing, graph.biMapConstLayerToOwnerLayer)).x + horizontalSpacing;
    }
}

static ImVec2 getNodeSize(const std::shared_ptr<Layer>& layer, float horizontalSpacing, float verticalSpacing, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) {
    auto ownerNodeSize = ax::NodeEditor::GetNodeSize(layer->getId());
    auto vecConst = biMapConstLayerToOwnerLayer.getVecConstLayerForOwner(layer);
    ImVec2 vecConstSize(0.0, 0.0);
    for (const auto& constLayer : vecConst) {
        auto constLayerSize = getNodeSize(constLayer, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
        vecConstSize.x += constLayerSize.x;
        vecConstSize.y = std::max(vecConstSize.y, constLayerSize.y);
    }
    if (vecConst.size()) { vecConstSize.x += (vecConst.size() - 1) * horizontalSpacing; }
    return ImVec2(std::max(vecConstSize.x, ownerNodeSize.x), vecConst.size() ? vecConstSize.y + verticalSpacing + ownerNodeSize.y : ownerNodeSize.y);
}

ImVec2 Row::getSize(float horizontalSpacing, float verticalSpacing, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) const {
    ImVec2 rowSize;
    for (const auto& layer : vecLayer) {
        auto nodeSize = getNodeSize(layer, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
        rowSize.x += nodeSize.x;
        rowSize.y = std::max(rowSize.y, nodeSize.y);
    }
    if (vecLayer.size()) rowSize.x += (vecLayer.size() - 1) * horizontalSpacing;
    return rowSize;
}

static void SetLayerPosition(std::shared_ptr<Layer> layer, ImVec2 pos, float horizontalSpacing, float verticalSpacing, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer)
{
    auto wholeNodeSize = getNodeSize(layer, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
    auto ownerNodeSize = ax::NodeEditor::GetNodeSize(layer->getId());
    ImVec2 actualOwnerNodePos = pos;
    actualOwnerNodePos.x = pos.x + (wholeNodeSize.x - ownerNodeSize.x) / 2;
    actualOwnerNodePos.y = pos.y + (wholeNodeSize.y - ownerNodeSize.y);
    ax::NodeEditor::SetNodePosition(layer->getId(), actualOwnerNodePos);
    auto vecConstLayer = biMapConstLayerToOwnerLayer.getVecConstLayerForOwner(layer);
    ImVec2 constLayerPos(pos.x, pos.y);
    for (const auto& constLayer : vecConstLayer) {
        SetLayerPosition(constLayer, constLayerPos, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
        constLayerPos.x += horizontalSpacing + getNodeSize(constLayer, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer).x;
    }
}

void Row::layoutLayers(float horizontalSpacing, float verticalSpacing, float leftmargin, float horizontalOffset, float verticalOffset, const BiMapConstLayerToOwnerLayer& biMapConstLayerToOwnerLayer) const
{
    auto xOffset = leftmargin + horizontalOffset;
    for (const auto& layer : vecLayer) {
        ImVec2 pos(xOffset, verticalOffset);
        SetLayerPosition(layer, pos, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer);
        xOffset += getNodeSize(layer, horizontalSpacing, verticalSpacing, biMapConstLayerToOwnerLayer).x + horizontalSpacing;
    }
}
