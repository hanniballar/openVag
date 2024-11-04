#include "RemoveXMLElement.h"

void RemoveXMLElement::execute()
{
    if (doFlag) { doAct(); }
    else { undoAct(); }
}

void RemoveXMLElement::doAct() {
    this->doFlag = false;
    auto prevEl = removeEdge->el->PreviousSibling();
    prevEdge = XMLNodeWrapper::make_shared(prevEl);
    auto xmlRemoveEl = removeEdge->el;
    auto clone = xmlRemoveEl->DeepClone(xmlRemoveEl->GetDocument());
    parentNode = XMLNodeWrapper::make_shared(xmlRemoveEl->Parent());
    removeEdge->set(clone->ToElement());

    xmlRemoveEl->Parent()->DeleteChild(xmlRemoveEl);
}

void RemoveXMLElement::undoAct() {
    this->doFlag = true;
    if (prevEdge == nullptr) {
        parentNode->el->InsertFirstChild(removeEdge->el);
    } else {
        parentNode->el->InsertAfterChild(prevEdge->el, removeEdge->el);
    }
    parentNode.reset();
    prevEdge.reset();
}
