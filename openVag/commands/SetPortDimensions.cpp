#include "SetPortDimensions.h"

#include <algorithm>
#include <utility>

SetPortDimensions::SetPortDimensions(std::shared_ptr<Port> port, const std::vector<std::string>& vecDim) : port(port), vecDim(vecDim)
{
    auto xmlPort = port->getXmlElement();
    size_t pos = 0;
    for (auto child = xmlPort->FirstChild(); child != nullptr; child = child->NextSibling()) {
        if (child->Value() == std::string("dim")) vecInitialDimPos.push_back(pos);
        pos++;
    }
}

void SetPortDimensions::doAct() {
    std::vector<std::string> vecOldDim;
    vecOldDim.reserve(vecOldDim.size());
    auto xmlElementRaw = port->getXmlElement();
    vecOldDim = port->getVecDim();
    port->setVecDim(vecDim, vecInitialDimPos);
    vecDim = vecOldDim;
    this->doFlag = false;
}

void SetPortDimensions::undoAct() {
    doAct();
    this->doFlag = true;
}
