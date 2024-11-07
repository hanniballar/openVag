#include "ModifyAttributeXMLElement.h"
#include <cassert>

void ModifyAttributeXMLElement::doAct() {
    this->doFlag = false;
    auto xmlElementRaw = xmlElement->el->ToElement();
    auto mapSaveChangeAttr = decltype(mapChangeAttr)();
    for (const auto& changeAttr : mapChangeAttr) {
        assert(xmlElementRaw->Attribute(changeAttr.first.c_str()) != nullptr);
        mapSaveChangeAttr[changeAttr.first] = xmlElementRaw->Attribute(changeAttr.first.c_str());
        xmlElementRaw->SetAttribute(changeAttr.first.c_str(), changeAttr.second.c_str());
    }
    mapChangeAttr = std::move(mapSaveChangeAttr);
}

void ModifyAttributeXMLElement::undoAct() {
    doAct();
    this->doFlag = true;
}
