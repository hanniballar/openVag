#include "AddXMLLayer.h"
#include <cassert>

void AddXMLLayer::doAct() {
    assert(std::string(xmlElementLayers->el->ToElement()->Name()) == "layers");
    xmlElement = XMLNodeWrapper::make_shared(xmlElementLayers->el->GetDocument()->NewElement("layer"));
    xmlElement->el->ToElement()->SetAttribute("id", id.c_str());
    xmlElement->el->ToElement()->SetAttribute("name", name.c_str());
    xmlElement->el->ToElement()->SetAttribute("type", type.c_str());

    xmlElementLayers->el->InsertEndChild(xmlElement->el);
    this->doFlag = false;
}

void AddXMLLayer::undoAct() {
    xmlElementLayers->el->DeleteChild(xmlElement->el);
    xmlElement.reset();
    this->doFlag = true;
}
