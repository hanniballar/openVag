#include "AddXMLEdge.h"

#include <cassert>

void AddXMLEdge::doAct() {
    assert(std::string(xmlElementEdges->el->ToElement()->Name()) == "edges");
    xmlElement = XMLNodeWrapper::make_shared(xmlElementEdges->el->GetDocument()->NewElement("edge"));
    xmlElement->el->ToElement()->SetAttribute("from-layer", from_layer.c_str());
    xmlElement->el->ToElement()->SetAttribute("from-port", from_port.c_str());
    xmlElement->el->ToElement()->SetAttribute("to-layer", to_layer.c_str());
    xmlElement->el->ToElement()->SetAttribute("to-port", to_port.c_str());

    xmlElementEdges->el->InsertEndChild(xmlElement->el);
    this->doFlag = false;
}

void AddXMLEdge::undoAct() {
    xmlElementEdges->el->DeleteChild(xmlElement->el);
    xmlElement.reset();
    this->doFlag = true;
}
