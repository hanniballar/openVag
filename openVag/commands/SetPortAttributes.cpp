#include "SetPortAttributes.h"

#include <algorithm>
#include <utility>

static std::vector<std::pair<std::string, std::string>> getAllAtributes(tinyxml2::XMLElement* xmlElement) {
    std::vector<std::pair<std::string, std::string>> vecAttributes;
    for (auto xmlAttr = xmlElement->FirstAttribute(); xmlAttr != nullptr; xmlAttr = xmlAttr->Next()) {
        vecAttributes.emplace_back(xmlAttr->Name(), xmlAttr->Value());
    }
    return vecAttributes;
}

void SetPortAttributes::doAct() {
    std::vector<std::pair<std::string, std::string>> vecOldAttribute;
    vecOldAttribute.reserve(vecAttribute.size());
    auto xmlElementRaw = port->getXmlElement();
    vecOldAttribute = getAllAtributes(xmlElementRaw);
    port->setAttributes(vecAttribute);
    vecAttribute = vecOldAttribute;
    this->doFlag = false;
}

void SetPortAttributes::undoAct() {
    doAct();
    this->doFlag = true;
}