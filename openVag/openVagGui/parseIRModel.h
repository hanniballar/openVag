#pragma once
#include <string>
#include <memory>

#include "IRModelGui.h"

std::shared_ptr<IRModelGui> parseIRModel(const std::string& fileName);
std::shared_ptr<IRModelGui> parseIRModel(const char* xmlContent, size_t nBytes);
tinyxml2::XMLError saveFile(const char* filename);
