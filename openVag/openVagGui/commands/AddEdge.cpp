#include "AddEdge.h"

#include <cassert>

#include "AddXMLEdge.h"

void AddEdge::doAct()
{
    addEdge->Parent()->vecEdgeGui.push_back(addEdge);
    const char* from_layer = addEdge->outputPort->Parent()->getXmlId();
    const char* from_port = addEdge->outputPort->getXmlId();
    const char* to_layer = addEdge->inputPort->Parent()->getXmlId();
    const char* to_port = addEdge->inputPort->getXmlId();
    auto xmlElementEdges = XMLNodeWrapper::make_shared(addEdge->Parent()->Parent()->Parent()->xmlElement->el->FirstChildElement("edges"));
    std::shared_ptr<AddXMLEdge> addXMLEdge = std::make_shared<AddXMLEdge>(from_layer, from_port, to_layer, to_port, xmlElementEdges);
    assert(addEdge->xmlElement == nullptr);
    addEdge->xmlElement = addXMLEdge->getXMLElement();
    commandCenter.execute(addXMLEdge);
    this->doFlag = false;
}

void AddEdge::undoAct()
{
    addEdge->Parent()->vecEdgeGui.pop_back();
    addEdge->xmlElement.reset();
    commandCenter.undo();
    this->doFlag = true;
}
