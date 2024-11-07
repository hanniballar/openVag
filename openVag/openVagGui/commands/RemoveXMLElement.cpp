#include "RemoveXMLElement.h"

void RemoveXMLElement::doAct() {
    this->doFlag = false;
    auto prevEl = xmlElement->el->PreviousSibling();
    prevElement = XMLNodeWrapper::make_shared(prevEl);
    auto xmlRemoveEl = xmlElement->el;
    auto clone = xmlRemoveEl->DeepClone(xmlRemoveEl->GetDocument());
    parentNode = XMLNodeWrapper::make_shared(xmlRemoveEl->Parent());
    xmlElement->set(clone->ToElement());

    xmlRemoveEl->Parent()->DeleteChild(xmlRemoveEl);
}

void RemoveXMLElement::undoAct() {
    this->doFlag = true;
    if (prevElement == nullptr) {
        parentNode->el->InsertFirstChild(xmlElement->el);
    } else {
        parentNode->el->InsertAfterChild(prevElement->el, xmlElement->el);
    }
    parentNode.reset();
    prevElement.reset();
}
