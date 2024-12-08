#include "SetLayerAttributes.h"

#include <algorithm>
#include <utility>

static void deleteAllAtributes(tinyxml2::XMLElement* xmlElement) {
    for (const auto* attr = xmlElement->FirstAttribute();
        attr != nullptr;
        xmlElement->DeleteAttribute(attr->Name()), attr = xmlElement->FirstAttribute()) {
    }
}

static std::vector<std::pair<std::string, std::string>> getAllAtributes(tinyxml2::XMLElement* xmlElement) {
    std::vector<std::pair<std::string, std::string>> vecAttributes;
    for (auto xmlAttr = xmlElement->FirstAttribute(); xmlAttr != nullptr; xmlAttr = xmlAttr->Next()) {
        vecAttributes.emplace_back(xmlAttr->Name(), xmlAttr->Value());
    }
    return vecAttributes;
}

void SetLayerAttributes::doAct() {
    std::vector<std::pair<std::string, std::string>> vecOldAttribute;
    vecOldAttribute.reserve(vecAttribute.size());
    auto xmlElementRaw = layer->getXmlElement()->el->ToElement();
    vecOldAttribute = getAllAtributes(xmlElementRaw);
    layer->modifyAttributes(vecAttribute);
    vecAttribute = vecOldAttribute;
    this->doFlag = false;
}

void SetLayerAttributes::undoAct() {
    doAct();
    this->doFlag = true;
}




