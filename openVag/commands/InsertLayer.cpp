#include "InsertLayer.h"

#include <cassert>
#include <string>

#include "../OpenVag.h"
#include "../parseIRModel.h"
#include "DeleteLayer.h"

void static prepareLayerForInsertion(std::shared_ptr<Layer>& layer, std::shared_ptr<IRModel> irModelGui) {
    assert(layer != nullptr);
    assert(irModelGui != nullptr);
    if (irModelGui->getLayers()->getLayer(layer->getXmlId()) != nullptr) {
        layer->changeXmlId(std::to_string(irModelGui->getLayers()->getMaxLayerXmlId() + 1));
    }
}

InsertLayer::InsertLayer(std::shared_ptr<IRModel> irModelGui) : irModelGui(irModelGui) {
    layer = parseLayer(R"(<layer id="1" name="openVagLayer" type="Const"/>)", irModelGui->getXMLDocument().get());
    prepareLayerForInsertion(layer, irModelGui);
}

InsertLayer::InsertLayer(std::shared_ptr<IRModel> irModelGui, tinyxml2::XMLElement* xmlElement) : irModelGui(irModelGui) {
    layer = parseLayer(xmlElement);
    prepareLayerForInsertion(layer, irModelGui);
}

void InsertLayer::doAct()
{
    if (commandCenter.getUndoSize() == 0) {
        irModelGui->getNetwork()->getLayers()->insertLayer(layer);
    }
    else {
        commandCenter.undoAll();
        commandCenter.reset();
    }
    this->doFlag = false;
}

void InsertLayer::undoAct()
{
    commandCenter.execute(std::make_shared<DeleteLayer>(layer));
    this->doFlag = true;
}
