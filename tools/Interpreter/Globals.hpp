#pragma once

#define HELP_STRING \
R"(GOOSE architecture binary file interpreter by Ilya Burakov

Possible usages: 
	interpreter.exe <filename>
		In this case, the given binary file is executed and no additional information is printed (so, the default argument set is just -r).
	
	interpreter.exe <filename> [arguments]
		Use arguments if you want to do something other that executing the file.
	
	interpreter.exe
	interpreter.exe /?
	interpreter.exe /h
	interpreter.exe -h
	interpreter.exe --help
		Just prints this help.

Possible arguments:
	/r
	-r
	--run
		Runs given binary executable.
		If any arguments except filename are presented, you should explicitly state a need to run the file (or it won't be executed).

	/i
	-i
	--information
		Prints information about the executable.

	/t
	-t
	--time
		Print execution time (ignored if -r is not presented).
)"