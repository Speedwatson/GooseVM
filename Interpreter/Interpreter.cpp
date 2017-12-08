#include "stdafx.h"
#include "globals.hpp"
#include "Arguments.hpp"
#include "Exceptions.hpp"
#include "ExecutableFile.hpp"

int main(int argc, char* argv[]) {
	std::vector<std::string> argvec(argc - 1);
	for (int i = 1; i < argc; ++i) argvec[i - 1] = argv[i];

	try {
		Arguments args(argvec);

		if (args.get(args.argHelp)) {
			std::cout << HELP_STRING << '\n';
			return EXIT_SUCCESS;
		}

		const ExecutableFile execfile(argvec[0]);
		
		std::cout << "Sections count: " << execfile.tablesHeader.sectionsCount << "\nBlobs count: " << execfile.tablesHeader.blobsCount << "\n";

		return EXIT_SUCCESS;
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