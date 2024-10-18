#pragma once
#include <cstdint>

int64_t GetNextId();

class OpenVag{
public:
	OpenVag() {};
	bool Create();
	bool Run();
	bool Quit();
	~OpenVag();

	
private:

};