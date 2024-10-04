#pragma once

#include <vector>
#include <memory>

#include "tinyxml2.h"

class XMLNodeWrapper {
protected:
    XMLNodeWrapper(tinyxml2::XMLNode* el) :el(el) {}
public:
    static std::shared_ptr<XMLNodeWrapper> make_shared(tinyxml2::XMLNode* el);
        
    virtual ~XMLNodeWrapper();
    void set(tinyxml2::XMLNode* _el);
    tinyxml2::XMLNode* el;
};
