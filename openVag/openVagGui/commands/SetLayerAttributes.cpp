#include "SetLayerAttributes.h"

#include <algorithm>
#include <utility>

void SetLayerAttributes::doAct() {
    std::vector<std::pair<std::string, std::string>> vecOldAttribute;
    vecOldAttribute.reserve(vecAttribute.size());
    auto xmlElementRaw = layer->getXmlElement()->el->ToElement();
    for (const auto& [attrName, attrValue] : vecAttribute) {
        const auto newAttrValue = xmlElementRaw->Attribute(attrName.c_str());
        vecOldAttribute.emplace_back(attrName, newAttrValue);
    }

    layer->modifyAttributes(vecAttribute);
    vecAttribute = vecOldAttribute;
    this->doFlag = false;
}

void SetLayerAttributes::undoAct() {
    doAct();
    this->doFlag = true;
}




