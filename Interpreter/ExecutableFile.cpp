#include "stdafx.h"
#include "ExecutableFile.hpp"

class ExecutableFile {
	bool valid = false;

public:
	bool is_valid() { return valid; }


	ExecutableFile(std::string filename);
	
};