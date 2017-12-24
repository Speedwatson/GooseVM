#include "stdafx.h"
#include "Globals.hpp"
#include "Arguments.hpp"
#include "Exceptions.hpp"
#include "ExecutableFile.hpp"
#include "Machine.hpp"

int main(int argc, char* argv[]) {
	std::vector<std::string> argvec(argc - 1);
	for (int i = 1; i < argc; ++i) argvec[i - 1] = argv[i];

	try {
		Arguments args(argvec);

		if (args.get(args.argHelp)) {
			std::cout << HELP_STRING << '\n';
			return EXIT_SUCCESS;
		}

		Machine machine;
		ExecutableFile execfile(argvec[0]);
		
		int excode = machine.run(&execfile);
		return excode;

		/*
			TODO:
				4) Implementing Machine, machine.run(), running fib.asm
				5) Writing merge sort
		*/
	}
	catch (Error& e) {
		std::cerr << e.what();
		return EXIT_FAILURE;
	}
	catch (std::bad_alloc& e) {
		std::cerr << "Could't allocate memory: " << e.what() << ".\n";
		return EXIT_FAILURE;
	}

}