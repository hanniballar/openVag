#pragma once

#include <string>

#include "IRXmlRep.h"

IRXmlRep parseIRModel(const std::string& fileName);
IRXmlRep parseIRModel(const char* xmlContent, size_t nBytes);
