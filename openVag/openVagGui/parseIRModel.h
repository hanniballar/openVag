#pragma once
#include <string>

#include "IRModelGui.h"

IRModelGui parseIRModel(const std::string& fileName);
IRModelGui parseIRModel(const char* xmlContent, size_t nBytes);
tinyxml2::XMLError saveFile(const char* filename);
