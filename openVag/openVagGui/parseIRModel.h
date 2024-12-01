#pragma once
#include <string>
#include <memory>

#include "IRModel.h"

std::shared_ptr<IRModel> parseIRModel();
std::shared_ptr<IRModel> parseIRModel(const std::string& fileName);
std::shared_ptr<IRModel> parseIRModel(const char* xmlContent, size_t nBytes);