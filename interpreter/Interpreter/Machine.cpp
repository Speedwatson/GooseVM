#include "stdafx.h"
#include "Exceptions.hpp"
#include "Machine.hpp"


int Machine::run(ExecutableFile* execfile) {
	ef = execfile;
	ip = ef->start;	
	csz = ROOT_CSZ;
	stack.push(Entry(ROOT_CSZ, Entry::etOfs));
	stack.push(Entry(ROOT_CSZ, Entry::etW));

	do {
		uint8_t opcode = *(uint8_t*)ef->getCodePtr(ip);
		(this->*handler[opcode])();
	} while (!exit);
	
	return exit;
}