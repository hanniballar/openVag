#include "ModifyEdge.h"

#include <algorithm>

void ModifyEdge::doAct() {
    std::map<std::string, std::string> mapOldAttributes;
    auto xmlElementRaw = edge->getXmlElement();
    for (const auto& [attrName, attrValue] : mapAttribute) {
        mapOldAttributes[attrName] = xmlElementRaw->Attribute(attrName.c_str());
    }

    edge->setAttributes(mapAttribute);
    mapAttribute = mapOldAttributes;
    this->doFlag = false;
}

void ModifyEdge::undoAct() {
    doAct();
    this->doFlag = true;
}




