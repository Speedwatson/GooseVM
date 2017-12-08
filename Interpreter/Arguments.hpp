#pragma once

class Arguments {
public:
	typedef uint8_t Mask;

	Arguments(const std::vector<std::string>& argv);

	enum Argument {
		argNo = 0,
		argPossibleFilename = 1,
		argHelp = 1 << 1,
		argRun = 1 << 2,
		argInformation = 1 << 3,
		argTime = 1 << 4,
	};

	Mask get() { return mask; }
	bool get(Argument attr) { return mask & attr;  }
	
private:
	Mask mask;	

	Argument str_to_arg(const std::string& str);
	Argument char_to_arg(char c);
};