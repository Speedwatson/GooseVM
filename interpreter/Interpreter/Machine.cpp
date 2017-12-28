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
		opcode_t opcode = *(opcode_t*)ef->getCodePtr(ip++);
		handle(opcode);
	} while (exit == CONTINUE_EXIT_CODE && ip < ef->code->len);
	
	return std::max(0, exit);
}

