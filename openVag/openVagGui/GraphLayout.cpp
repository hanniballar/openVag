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
    std::list<std::shared_ptr<LayerNodeGui>> vecLayer;
    ImVec2 getSize(float horizontalSpacing) const;
    void layoutLayers(float horizontalSpacing, float leftmargin, float horizontalOffset, float verticalOffset) const;
};

class Graph {
public:
    Graph(
        std::shared_ptr<LayerNodeGui> startLayer,
        const std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>& mapLayerToInputEdge,
        const std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>& mapLayerToOutputEdge);
    std::set<std::shared_ptr<LayerNodeGui>> getAllLayers();
    void layoutLayers(float horizontalSpacing, float verticalSpacing, float horizontalOffset);
    ImVec2 getSize(float horizontalSpacing, float verticalSpacing) const;
private:
    std::list<Row>::iterator getItRow(std::shared_ptr<LayerNodeGui>);
    std::list<Row>::iterator makePrevRowAvailable(std::list<Row>::iterator it);
    std::list<Row>::iterator makeNextRowAvailable(std::list<Row>::iterator it);
    std::vector<std::shared_ptr<LayerNodeGui>> getInputLayers(std::shared_ptr<LayerNodeGui> layer);
    std::vector<std::shared_ptr<LayerNodeGui>> getOutputLayers(std::shared_ptr<LayerNodeGui> layer);
    std::list<Row> listRow;

    const std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>& mapLayerToInputEdge;
    const std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>& mapLayerToOutputEdge;
};

Graph::Graph(
    std::shared_ptr<LayerNodeGui> startLayer,
    const std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>& mapLayerToInputEdge,
    const std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>& mapLayerToOutputEdge)
        : mapLayerToInputEdge(mapLayerToInputEdge), mapLayerToOutputEdge(mapLayerToOutputEdge)
{
    std::deque<std::shared_ptr<LayerNodeGui>> dequeProcLayer = { startLayer };
    std::set<std::shared_ptr<LayerNodeGui>> setSeenLayers;
    listRow.push_back(Row());
    listRow.front().vecLayer.push_back(startLayer);
    while (dequeProcLayer.size()) {
        auto procLayer = dequeProcLayer.front(); dequeProcLayer.pop_front();
        if (setSeenLayers.find(procLayer) != setSeenLayers.end()) continue;

        setSeenLayers.insert(procLayer);
        auto itRow = getItRow(procLayer);
        {
            auto vecInputLayer = getInputLayers(procLayer);
            vecInputLayer.erase(std::remove_if(vecInputLayer.begin(), vecInputLayer.end(),
                [&setSeenLayers](const auto& layer) { return setSeenLayers.find(layer) != setSeenLayers.end(); }
            ), vecInputLayer.end());
            if (vecInputLayer.size()) {
                auto itPrevRow = makePrevRowAvailable(itRow);
                itPrevRow->vecLayer.insert(itPrevRow->vecLayer.end(), vecInputLayer.begin(), vecInputLayer.end());
                dequeProcLayer.insert(dequeProcLayer.end(), vecInputLayer.begin(), vecInputLayer.end());
            }
        }
        {
            auto vecOutputLayer = getOutputLayers(procLayer);
            vecOutputLayer.erase(std::remove_if(vecOutputLayer.begin(), vecOutputLayer.end(),
                [&setSeenLayers](const auto& layer) { 
                    return setSeenLayers.find(layer) != setSeenLayers.end();
                }
            ), vecOutputLayer.end());
            if (vecOutputLayer.size()) {
                auto itNextRow = makeNextRowAvailable(itRow);
                itNextRow->vecLayer.insert(itNextRow->vecLayer.end(), vecOutputLayer.begin(), vecOutputLayer.end());
                dequeProcLayer.insert(dequeProcLayer.end(), vecOutputLayer.begin(), vecOutputLayer.end());
            }
        }
    }
}

std::set<std::shared_ptr<LayerNodeGui>> Graph::getAllLayers()
{
    std::set<std::shared_ptr<LayerNodeGui>> setLayers;
    for (const auto& row : listRow) { setLayers.insert(row.vecLayer.begin(), row.vecLayer.end()); }
    return setLayers;
}

void Graph::layoutLayers(float horizontalSpacing, float verticalSpacing, float horizontalOffset)
{
    auto graphSize = getSize(horizontalSpacing, verticalSpacing);
    float verticalOffset = 0;
    for (const auto& row : listRow) {
        auto rowSize = row.getSize(horizontalSpacing);
        auto leftMargin = (graphSize.x - rowSize.x) / 2;
        row.layoutLayers(horizontalSpacing, leftMargin, horizontalOffset, verticalOffset);
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

std::list<Row>::iterator Graph::getItRow(std::shared_ptr<LayerNodeGui> searchLayer)
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

std::vector<std::shared_ptr<LayerNodeGui>> Graph::getInputLayers(std::shared_ptr<LayerNodeGui> layer)
{
    std::vector<std::shared_ptr<LayerNodeGui>> vecInputLayers;
    if (mapLayerToInputEdge.find(layer) == mapLayerToInputEdge.end()) return {};
    for (const auto& edge : mapLayerToInputEdge.at(layer)) {
        vecInputLayers.push_back(edge->outputPort->Parent());
    }
    return vecInputLayers;
}

std::vector<std::shared_ptr<LayerNodeGui>> Graph::getOutputLayers(std::shared_ptr<LayerNodeGui> layer)
{
    std::vector<std::shared_ptr<LayerNodeGui>> vecOutputLayers;
    if (mapLayerToOutputEdge.find(layer) == mapLayerToOutputEdge.end()) return {};
    for (const auto& edge : mapLayerToOutputEdge.at(layer)) {
        vecOutputLayers.push_back(edge->inputPort->Parent());
    }
    return vecOutputLayers;
}

std::pair<std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>, std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>>>
    getMapLayerToOutputEdge(std::shared_ptr<IRModelGui> irModelGui) 
{
    std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>> mapLayerToInputEdge;
    std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>> mapLayerToOutputEdge;

    for (const auto& layer : irModelGui->vecLayerNodeGui) {
        for (const auto& port : layer->vecOutputPort) {
            for (const auto& edge : port->vecEdgeGui) {
                mapLayerToInputEdge[edge->inputPort->Parent()].push_back(edge);
                mapLayerToOutputEdge[edge->outputPort->Parent()].push_back(edge);
            }
        }
    }

    return { mapLayerToInputEdge, mapLayerToOutputEdge };
}

void GraphLayout::layoutNodes(std::shared_ptr<IRModelGui> irModelGui)
{
    std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>> mapLayerToInputEdge;
    std::map<std::shared_ptr<LayerNodeGui>, std::vector<std::shared_ptr<EdgeGui>>> mapLayerToOutputEdge;
    std::tie(mapLayerToInputEdge, mapLayerToOutputEdge) = getMapLayerToOutputEdge(irModelGui);

    std::vector<std::shared_ptr<LayerNodeGui>> vecLayerNotProc = irModelGui->vecLayerNodeGui;
    std::vector<Graph> vecGraph;

    while (vecLayerNotProc.size()) {
        auto layer = vecLayerNotProc[0];
        Graph graph(layer, mapLayerToInputEdge, mapLayerToOutputEdge);
        vecGraph.push_back(graph);
        const auto setGraphLayer = graph.getAllLayers();
        vecLayerNotProc.erase(std::remove_if(vecLayerNotProc.begin(), vecLayerNotProc.end(), [&setGraphLayer](const auto& layer) {
            return setGraphLayer.find(layer) != setGraphLayer.end();
            }), vecLayerNotProc.end());
    }

    float prevGraphOffSet = 0;
    for (auto& graph : vecGraph) {
        graph.layoutLayers(horizontalSpacing, verticalSpacing, prevGraphOffSet);
        prevGraphOffSet += (graph.getSize(horizontalSpacing, verticalSpacing)).x + horizontalSpacing;
    }
}

ImVec2 Row::getSize(float horizontalSpacing) const {
    ImVec2 rowSize;
    for (const auto& layer : vecLayer) {
        auto nodeSize = ax::NodeEditor::GetNodeSize(layer->id_gui);
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
        ax::NodeEditor::SetNodePosition(layer->id_gui, pos);
        xOffset += ax::NodeEditor::GetNodeSize(layer->id_gui).x + horizontalSpacing;
    }
}
