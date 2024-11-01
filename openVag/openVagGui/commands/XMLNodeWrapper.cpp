#include "XMLNodeWrapper.h"
#include <algorithm>
#include <cassert>
#include <map>

namespace {
    std::map <tinyxml2::XMLNode*, std::weak_ptr<XMLNodeWrapper>> mapXMLNodeToSharedPtr;
}

struct MakeSharedEnabler : public XMLNodeWrapper {
    MakeSharedEnabler(tinyxml2::XMLNode* el) : XMLNodeWrapper(el) {}
};

std::shared_ptr<XMLNodeWrapper>XMLNodeWrapper::make_shared(tinyxml2::XMLNode* el) {
    if (el == nullptr) return {};
    auto mapIterXMLNodeToSharedPtr = mapXMLNodeToSharedPtr.find(el);
    if (mapIterXMLNodeToSharedPtr != mapXMLNodeToSharedPtr.end()) {
        return mapIterXMLNodeToSharedPtr->second.lock();
    }
    else {
        auto xmlNodeWrapper = std::make_shared<MakeSharedEnabler>(el);

        mapXMLNodeToSharedPtr[el] = xmlNodeWrapper;
        return xmlNodeWrapper;
    }
}

XMLNodeWrapper::~XMLNodeWrapper()
{
    auto isErased = mapXMLNodeToSharedPtr.erase(this->el);
    assert(isErased);
}

void XMLNodeWrapper::set(tinyxml2::XMLNode* el) {
    auto mapIterXMLNodeToSharedPtr = mapXMLNodeToSharedPtr.find(this->el);
    assert(mapIterXMLNodeToSharedPtr != mapXMLNodeToSharedPtr.end());
    this->el = el;
    mapXMLNodeToSharedPtr[el] = mapIterXMLNodeToSharedPtr->second;
    mapXMLNodeToSharedPtr.erase(mapIterXMLNodeToSharedPtr);
}