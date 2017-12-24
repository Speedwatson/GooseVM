#include "stdafx.h"
#include "Exceptions.hpp"
#include "Machine.hpp"


int Machine::run(ExecutableFile* execfile) {
	ef = execfile;
	ip = ef->start;	
	csz = ROOT_CSZ;
	stack.push(ROOT_CSZ);
	stack.push(ROOT_CSZ);

	do {
		uint8_t opcode = *(uint8_t*)ef->getCodePtr(ip);
		(this->*handler[opcode])();
	} while (exit == CONTINUE_EXIT_CODE);
	
	return exit;
}