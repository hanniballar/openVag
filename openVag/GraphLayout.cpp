#include "GraphLayout.h"

#include <list>
#include <map>
#include <utility>
#include <tuple>
#include <cassert>
#include <set>
#include <deque>
#include <algorithm>

#include "imgui.h"

class Row {
public:
    std::list<std::shared_ptr<Layer>> vecLayer;
    ImVec2 getSize(float horizontalSpacing) const;
    void layoutLayers(float horizontalSpacing, float leftmargin, float horizontalOffset, float verticalOffset) const;
};

class Graph {
public:
    Graph(const std::shared_ptr<Layer>& startLayer);
    std::set<std::shared_ptr<Layer>> getAllLayers();
    void layoutLayers(float horizontalSpacing, float verticalSpacing, ImVec2 startPos);
    ImVec2 getSize(float horizontalSpacing, float verticalSpacing) const;
private:
    std::list<Row>::iterator getItRow(std::shared_ptr<Layer>);
    std::list<Row>::iterator makePrevRowAvailable(std::list<Row>::iterator it);
    std::list<Row>::iterator makeNextRowAvailable(std::list<Row>::iterator it);
    std::list<Row> listRow;
};

Graph::Graph(const std::shared_ptr<Layer>& startLayer) {
    std::deque<std::shared_ptr<Layer>> dequeProcLayer = { startLayer };
    std::set<std::shared_ptr<Layer>> setSeenLayers;
    listRow.push_back(Row());
    listRow.front().vecLayer.push_back(startLayer);
    while (dequeProcLayer.size()) {
        auto procLayer = dequeProcLayer.front(); dequeProcLayer.pop_front();
        if (setSeenLayers.find(procLayer) != setSeenLayers.end()) continue;

        setSeenLayers.insert(procLayer);
        auto itRow = getItRow(procLayer);
        {
            auto setOutputLayer = procLayer->getOutputLayers();
            for (auto& layer : setSeenLayers) setOutputLayer.erase(layer);
            for (auto& layer : dequeProcLayer) setOutputLayer.erase(layer);
            if (setOutputLayer.size()) {
                auto itNextRow = makeNextRowAvailable(itRow);
                itNextRow->vecLayer.insert(itNextRow->vecLayer.end(), setOutputLayer.begin(), setOutputLayer.end());
                dequeProcLayer.insert(dequeProcLayer.end(), setOutputLayer.begin(), setOutputLayer.end());
            }
        }
        {
            auto setInputLayer = procLayer->getInputLayers();
            for (auto& layer : setSeenLayers) setInputLayer.erase(layer);
            for (auto& layer : dequeProcLayer) setInputLayer.erase(layer);
            if (setInputLayer.size()) {
                auto itPrevRow = makePrevRowAvailable(itRow);
                itPrevRow->vecLayer.insert(itPrevRow->vecLayer.end(), setInputLayer.begin(), setInputLayer.end());
                dequeProcLayer.insert(dequeProcLayer.end(), setInputLayer.begin(), setInputLayer.end());
            }
        }
    }
}

std::set<std::shared_ptr<Layer>> Graph::getAllLayers()
{
    std::set<std::shared_ptr<Layer>> setLayers;
    for (const auto& row : listRow) { setLayers.insert(row.vecLayer.begin(), row.vecLayer.end()); }
    return setLayers;
}

void Graph::layoutLayers(float horizontalSpacing, float verticalSpacing, ImVec2 startPos)
{
    auto graphSize = getSize(horizontalSpacing, verticalSpacing);
    float verticalOffset = 0;
    for (const auto& row : listRow) {
        auto rowSize = row.getSize(horizontalSpacing);
        auto leftMargin = (graphSize.x - rowSize.x) / 2;
        row.layoutLayers(horizontalSpacing, leftMargin, startPos.x, startPos.y + verticalOffset);
        verticalOffset += verticalSpacing + rowSize.y;
    }
}

ImVec2 Graph::getSize(float horizontalSpacing, float vericalSpacing) const
{
    ImVec2 graphSize;
    for (auto& row : listRow) {
        auto rowSize = row.getSize(horizontalSpacing);
        graphSize.x = std::max(graphSize.x, rowSize.x);
        graphSize.y += rowSize.y + vericalSpacing;
    }
    graphSize.y -= vericalSpacing; //Last row does not needd verticalSpacing
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
    if (it == listRow.begin()) {
        return listRow.insert(listRow.begin(), Row());
    }
    return --it;
}

std::list<Row>::iterator Graph::makeNextRowAvailable(std::list<Row>::iterator it)
{
    auto nextIt = ++it;

    if (nextIt != listRow.end()) { return nextIt; }
    return listRow.insert(listRow.end(), Row());
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
        startPos.x += (graph.getSize(horizontalSpacing, verticalSpacing)).x + horizontalSpacing;
    }
}

ImVec2 Row::getSize(float horizontalSpacing) const {
    ImVec2 rowSize;
    for (const auto& layer : vecLayer) {
        auto nodeSize = ax::NodeEditor::GetNodeSize(layer->getId());
        rowSize.x += nodeSize.x;
        rowSize.y = std::max(rowSize.y, nodeSize.y);
    }
    if (vecLayer.size()) rowSize.x += (vecLayer.size() - 1) * horizontalSpacing;
    return rowSize;
}

void Row::layoutLayers(float horizontalSpacing, float leftmargin, float horizontalOffset, float verticalOffset) const
{
    auto xOffset = leftmargin + horizontalOffset;
    for (const auto& layer : vecLayer) {
        ImVec2 pos(xOffset, verticalOffset);
        ax::NodeEditor::SetNodePosition(layer->getId(), pos);
        xOffset += ax::NodeEditor::GetNodeSize(layer->getId()).x + horizontalSpacing;
    }
}
