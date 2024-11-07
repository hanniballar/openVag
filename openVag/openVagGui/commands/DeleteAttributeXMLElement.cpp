#include "DeleteAttributeXMLElement.h"
#include <cassert>

void DeleteAttributeXMLElement::doAct()
{
    auto xmlElementRaw = xmlElement->el->ToElement();
    for (const auto* attr = xmlElementRaw->FirstAttribute();
        attr != nullptr;
        attr = attr->Next()) {
        vecUndoDeleteAttrData.emplace_back(attr->Name(), attr->Value());
    }
    for (const auto& deleteAttr : vecDeleteAttr) {
        xmlElementRaw->DeleteAttribute(deleteAttr.c_str());
    }
    this->doFlag = false;
}

static void deleteAllAtributes(tinyxml2::XMLElement* xmlElement) {
    for (const auto* attr = xmlElement->FirstAttribute();
        attr != nullptr;
        xmlElement->DeleteAttribute(attr->Name()), attr = xmlElement->FirstAttribute()){}
}

void DeleteAttributeXMLElement::undoAct() {
    auto xmlElementRaw = xmlElement->el->ToElement();
    deleteAllAtributes(xmlElementRaw);
    for (const auto& attrData : vecUndoDeleteAttrData) {
        xmlElementRaw->SetAttribute(attrData.name.c_str(), attrData.value.c_str());
    }
    vecUndoDeleteAttrData.clear();
    this->doFlag = true;
}
