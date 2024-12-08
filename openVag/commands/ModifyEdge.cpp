#include "ModifyEdge.h"

#include <algorithm>

void ModifyEdge::doAct() {
    std::map<std::string, std::string> mapOldAttributes;
    auto xmlElementRaw = edge->getXmlElement()->el->ToElement();
    for (const auto& [attrName, attrValue] : mapAttribute) {
        mapOldAttributes[attrName] = xmlElementRaw->Attribute(attrName.c_str());
    }

    edge->modifyAttributes(mapAttribute);
    mapAttribute = mapOldAttributes;
    this->doFlag = false;
}

void ModifyEdge::undoAct() {
    doAct();
    this->doFlag = true;
}




