#pragma once
#include <string>
#include <memory>
#include <tinyxml2.h>

#include "IRModel.h"

std::shared_ptr<Layer> parseLayer(std::string xmlString, tinyxml2::XMLDocument* xmlDocument);
std::shared_ptr<Layer> parseLayer(tinyxml2::XMLElement* xmlLayer);
std::shared_ptr<IRModel> parseIRModel();
std::shared_ptr<IRModel> parseIRModel(const std::string& fileName);
std::shared_ptr<IRModel> parseIRModel(const char* xmlContent, size_t nBytes);