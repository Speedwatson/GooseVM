#include "stdafx.h"
#include "Arguments.hpp"
#include "Exceptions.hpp"

int main(int argc, char* argv[]) {
	std::vector<std::string> argvstr(argc - 1);
	for (int i = 1; i < argc; ++i) argvstr[i - 1] = argv[i];
	
	try {
		Arguments args(argvstr);

		// create Machine and run it...
	}
	catch (UnknownArgError& e) {
		std::cerr << e.what();
	}
		
	return 0;
}