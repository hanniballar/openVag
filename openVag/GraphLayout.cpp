#include "GraphLayout.h"

#include <list>
#include <utility>
#include <tuple>
#include <cassert>
#include <set>
#include <unordered_set>
#include <deque>
#include <algorithm>

#include "imgui.h"

class ConstOwnerLayer;

class ConstOwnerLayer
{
public:
    ConstOwnerLayer(std::shared_ptr<Layer> layer) : layer(layer) {}

    ConstOwnerLayer* addConstLayer(const std::shared_ptr<Layer>& constLayer) {
        listConstLayer.emplace_back(constLayer);
        return &listConstLayer.back();
    }

    std::unordered_set<std::shared_ptr<Layer>> getAllLayers() const {
        std::unordered_set<std::shared_ptr<Layer>> setAllLayers;
        setAllLayers.insert(layer);
        for (const auto& constLayer : listConstLayer) {
            auto setLayers = constLayer.getAllLayers();
            setAllLayers.insert(setLayers.begin(), setLayers.end());
        }

        return setAllLayers;
    }

    ImVec2 getSize(ImVec2 spacing) const {
        if (sizeLayer.x == 0 && sizeLayer.y == 0) {
            //Calculate actual size
            sizeLayer = ax::NodeEditor::GetNodeSize(layer->getId());
            for (const auto constLayer : listConstLayer) {
                auto sizeConstLayer = constLayer.getSize(spacing);
                sizeListConstLayer.x += sizeConstLayer.x;
                sizeListConstLayer.y = std::max(sizeListConstLayer.y, sizeConstLayer.y);
            }
        }
        return { std::max(sizeLayer.x, getSizeListConstLayer(spacing).x),
            sizeLayer.y + (sizeListConstLayer.y == 0 ? 0 : sizeListConstLayer.y + spacing.y) };
    }

    void setPosition(ImVec2 pos, ImVec2 spacing) const {
        ImVec2 size = getSize(spacing);
        ImVec2 posLayer;
        posLayer.x = (size.x - sizeLayer.x) / 2 + pos.x;
        posLayer.y = (size.y - sizeLayer.y) + pos.y;
        ax::NodeEditor::SetNodePosition(layer->getId(), posLayer);

        for (auto& constLayer : listConstLayer) {
            ImVec2 posConstLayer = { (size.x - getSizeListConstLayer(spacing).x) / 2 + pos.x, pos.y};
            constLayer.setPosition(posConstLayer, spacing);
            pos.x += constLayer.getSize(spacing).x + spacing.x;
        }
    }

private:
    ImVec2 getSizeListConstLayer(ImVec2 spacing) const {
        return { sizeListConstLayer.x + (listConstLayer.size() > 1 ? listConstLayer.size() - 1 : 0) * spacing.x, 
            sizeListConstLayer.y };
    }
    std::shared_ptr<Layer> layer;
    mutable ImVec2 sizeLayer;
    mutable ImVec2 sizeListConstLayer;
    std::list<ConstOwnerLayer> listConstLayer;
};

class Row {
public:
    std::list<ConstOwnerLayer> listConstOwnerLayer;
    ImVec2 getSize(ImVec2 spacing) const;
    void setPosition(ImVec2 pos, ImVec2 spacing) const;
};

class Graph {
public:
    Graph(std::shared_ptr<Layer> startLayer);
    std::unordered_set <std::shared_ptr<Layer>> getAllLayers();
    void setPosition(ImVec2 pos, ImVec2 spacing);
    ImVec2 getSize(ImVec2 spacing) const;
private:
    std::list<Row> listRow;
    std::list<Row>::iterator makePrevRowAvailable(std::shared_ptr<Layer> layer, std::list<Row>::iterator it);
    std::list<Row>::iterator makeNextRowAvailable(std::shared_ptr<Layer> layer, std::list<Row>::iterator it);
    std::list<Row>::iterator makeParameterLayerRowAvailable();
    std::list<Row>::iterator makeResultLayerRowAvailable();
    std::list<Row>::iterator getDedicatedRow(std::shared_ptr<Layer> layer);
    std::list<Row>::iterator itInputLayerRow = listRow.end();
    std::list<Row>::iterator itResultLayerRow = listRow.end();
};

Graph::Graph(std::shared_ptr<Layer> startLayer) {
    startLayer = [&]() {
        if (std::string(startLayer->getType()) == "Const") {
            auto setOutputLayer = startLayer->getOutputLayers();
            if (setOutputLayer.size()) { startLayer = *setOutputLayer.begin(); }
        }
        return startLayer;
        }();
    auto itInsertRow = makeNextRowAvailable(startLayer, listRow.begin()); 
    itInsertRow->listConstOwnerLayer.emplace_back(startLayer);
    std::unordered_map<std::shared_ptr<Layer>, std::list<Row>::iterator> mapLayerToItRow = { {startLayer, itInsertRow } };
    std::unordered_map<std::shared_ptr<Layer>, ConstOwnerLayer*> mapLayerToConstOwnerLayer = { {startLayer, &itInsertRow->listConstOwnerLayer.back()} };
    std::deque<std::shared_ptr<Layer>> dequeProcLayer = { startLayer };

    while (dequeProcLayer.size()) {
        auto procLayer = dequeProcLayer.front(); dequeProcLayer.pop_front();

        auto skipLayer = [&](const std::shared_ptr<Layer>& layer) {
                return mapLayerToItRow.find(layer) != mapLayerToItRow.end();
            };

        assert(mapLayerToItRow.find(procLayer) != mapLayerToItRow.end());
        auto itRow = mapLayerToItRow[procLayer];
        {
            auto setOutputLayer = procLayer->getOutputLayers();
            for (const auto& layer : setOutputLayer) {
                if (skipLayer(layer)) continue;
                dequeProcLayer.push_back(layer);
                auto itInsertRow =  makeNextRowAvailable(layer, itRow);
                itInsertRow->listConstOwnerLayer.emplace_back(layer);
                mapLayerToConstOwnerLayer[layer] = &itInsertRow->listConstOwnerLayer.back();
                mapLayerToItRow[layer] = itInsertRow;
            }
        }
        {
            auto setInputLayer = procLayer->getInputLayers();
            for (const auto& layer : setInputLayer) {
                if (skipLayer(layer)) continue;
                dequeProcLayer.push_back(layer);
                if (std::string(layer->getType()) == "Const") {
                    assert(mapLayerToConstOwnerLayer.find(procLayer) != mapLayerToConstOwnerLayer.end());
                    auto procConstOwnerLayer = mapLayerToConstOwnerLayer[procLayer];
                    auto constOwnerLayer = procConstOwnerLayer->addConstLayer(layer);
                    mapLayerToConstOwnerLayer[layer] = constOwnerLayer;
                    mapLayerToItRow[layer] = itRow;
                    continue;
                }
                auto itInsertRow = makePrevRowAvailable(layer, itRow);
                itInsertRow->listConstOwnerLayer.push_back(layer);
                mapLayerToConstOwnerLayer[layer] = &itInsertRow->listConstOwnerLayer.back();
                mapLayerToItRow[layer] = itInsertRow;
            }
        }
    }
}

std::unordered_set <std::shared_ptr<Layer>> Graph::getAllLayers()
{
    std::unordered_set <std::shared_ptr<Layer>> setLayers;
    for (const auto& row : listRow) {
        for (const auto& constOwnerLayer : row.listConstOwnerLayer) {
            const auto layers = constOwnerLayer.getAllLayers();
            setLayers.insert(layers.begin(), layers.end());
        }
    }
    return setLayers;
}

void Graph::setPosition(ImVec2 pos, ImVec2 spacing)
{
    auto graphSize = getSize(spacing);
    for (const auto& row : listRow) {
        auto rowSize = row.getSize(spacing);
        ImVec2 rowPos = { (graphSize.x - rowSize.x) / 2 + pos.x, pos.y};
        row.setPosition(rowPos, spacing);
        pos.y += rowSize.y + spacing.y;
    }
}

ImVec2 Graph::getSize(ImVec2 spacing) const
{
    ImVec2 graphSize;
    for (auto& row : listRow) {
        auto rowSize = row.getSize(spacing);
        graphSize.x = std::max(graphSize.x, rowSize.x);
        graphSize.y += rowSize.y + spacing.y;
    }
    graphSize.y -= spacing.y; //Last row does not needd verticalSpacing
    return graphSize;
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

std::list<Row>::iterator Graph::makePrevRowAvailable(std::shared_ptr<Layer> layer, std::list<Row>::iterator it)
{
    auto itDedicated = getDedicatedRow(layer);
    if (itDedicated != listRow.end()) return itDedicated;
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

std::list<Row>::iterator Graph::makeNextRowAvailable(std::shared_ptr<Layer> layer, std::list<Row>::iterator it)
{
    auto itDedicated = getDedicatedRow(layer);
    if (itDedicated != listRow.end()) return itDedicated;

    if (it != listRow.end()) {
        auto nextIt = ++it;
        if (nextIt != itResultLayerRow) { return nextIt; }
    }
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

void GraphLayout::layoutNodes(std::set<std::shared_ptr<Layer>, LayerIDLess> layers, ImVec2 pos) {
    std::vector<Graph> vecGraph;

    while (layers.size()) {
        auto layer = *(layers.begin());
        Graph graph(layer);
        vecGraph.push_back(graph);
        for (auto& layer : graph.getAllLayers()) layers.erase(layer);
    }

    for (auto& graph : vecGraph) {
        graph.setPosition(pos, this->spacing);
        pos.x += graph.getSize(this->spacing).x + this->spacing.x;
    }
}

ImVec2 Row::getSize(ImVec2 spacing) const {
    ImVec2 rowSize;
    for (const auto& constOwnerLayer : listConstOwnerLayer) {
        auto nodeSize = constOwnerLayer.getSize(spacing);
        rowSize.x += nodeSize.x;
        rowSize.y = std::max(rowSize.y, nodeSize.y);
    }
    if (listConstOwnerLayer.size()) rowSize.x += (listConstOwnerLayer.size() - 1) * spacing.x;
    return rowSize;
}

void Row::setPosition(ImVec2 pos, ImVec2 spacing) const
{
    auto rowSize = this->getSize(spacing);
    for (const auto& node : listConstOwnerLayer) {
        auto nodeSize = node.getSize(spacing);
        ImVec2 nodePos = { pos.x, rowSize.y - nodeSize.y + pos.y };
        node.setPosition(nodePos, spacing);
        pos.x += nodeSize.x + spacing.x;
    }
}
