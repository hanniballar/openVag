#include "AddAttributeXMLElement.h"

void AddAttributeXMLElement::doAct()
{
    auto xmlElementRaw = xmlElement->el->ToElement();
    for (const auto& changeAttr : mapAttr) {
        vecDeleteAttr.push_back(changeAttr.first);
        xmlElementRaw->SetAttribute(changeAttr.first.c_str(), changeAttr.second.c_str());
    }
    mapAttr.clear();
    this->doFlag = false;
}

void AddAttributeXMLElement::undoAct() {
    auto xmlElementRaw = xmlElement->el->ToElement();
    for (const auto& changeAttr : vecDeleteAttr) {
        mapAttr[changeAttr] = xmlElementRaw->Attribute(changeAttr.c_str());
        xmlElementRaw->DeleteAttribute(changeAttr.c_str());
    }
    vecDeleteAttr.clear();
    this->doFlag = true;
}
