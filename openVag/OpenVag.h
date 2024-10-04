#pragma once
#include <cstdint>
#include "GraphLayout.h"

int64_t GetNextId();

class OpenVag{
public:
	OpenVag() {};
	bool Create();
	bool Run();
	bool Quit();
	~OpenVag();
};