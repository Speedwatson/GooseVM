#include "stdafx.h"
#include "Exceptions.hpp"
#include "Machine.hpp"

// Debug-related lines and functions are commented out here

int Machine::run(ExecutableFile* execfile) {
	ef = execfile;
	ip = ef->start;	
	csz = ROOT_CSZ;
	stack.push(ROOT_CSZ);
	stack.push(ROOT_CSZ);

	// std::vector<size_t> coverage(ef->sourceTextRanges.back().line + 1, 0);
	
	//int line;
	do {
		// line = getLine();
		// ++coverage[line];
		opcode_t opcode = *(opcode_t*)ef->getCodePtr(ip++);
		(this->*handlers[opcode])(opcode);
	} while (exit == CONTINUE_EXIT_CODE && ip < ef->code->len);
	
	return std::max(0, exit);
}

//int Machine::getLine(int tip) {
//	if (tip == -1) tip = ip;
//
//	int found = -1;
//	for (auto& point : ef->sourceCodePoints) {
//		if (point.address == tip) {
//			found = point.sourceOperationRangeIndex;
//			break;
//		}
//	}
//
//	return (found != -1) ? ef->sourceTextRanges[found].line : -1;
//}

