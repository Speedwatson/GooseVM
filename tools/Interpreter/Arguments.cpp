#include "stdafx.h"
#include "Arguments.hpp"
#include "Exceptions.hpp"

Arguments::Arguments(const std::vector<std::string>& argv) {
	mask = argNo;

	std::array<std::string, 4> help_ops{ "/?", "/h", "-h", "--help" };
	if (argv.size() == 0 || std::find(help_ops.begin(), help_ops.end(), argv[0]) != help_ops.end()) {
		mask |= argHelp;
	}
	else mask |= argPossibleFilename;

	if (argv.size() == 0) return;

	for (auto it = argv.begin() + 1; it != argv.end(); ++it) {
		if (it->size() > 2 && it->substr(0, 2) == "--") {
			Argument arg = str_to_arg(it->substr(2));
			if (arg) mask |= arg;
			else throw Error("Unknown argument: " + *it);
		}
		else if (it->size() > 1 && (it->at(0) == '-' || it->at(0) == '/')) {
			for (int i = 1; i < it->size(); ++i) {
				Argument arg = char_to_arg(it->at(i));
				if (arg) mask |= arg;
				else throw Error("Unknown argument: '" + std::string(1, it->at(i)) + "' in " + *it);
			}
		}
		else throw Error("Unknown argument: " + *it);
	}

	
}

Arguments::Argument Arguments::str_to_arg(const std::string & str) {
	if (str == "run") return argRun; 
	else if (str == "information") return argInformation; 
	else if (str == "time") return argTime;
	else return argNo;
}

Arguments::Argument Arguments::char_to_arg(char c) {
	switch (c) {
	case 'r': return argRun;
	case 'i': return argInformation;
	case 't': return argTime;
	default: return argNo;
	}
}
