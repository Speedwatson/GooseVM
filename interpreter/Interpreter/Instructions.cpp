#include "stdafx.h"
#include "Machine.hpp"
#include "Exceptions.hpp"

void Machine::nop(opcode_t opcode) {
	return;
}

void Machine::brk(opcode_t opcode) {
	__debugbreak();
}

void Machine::stop(opcode_t opcode) {
	if (mask(opcode, 7, 1)) {
		exit = stack.top();
		stack.pop();
	} else exit = read<imm8_t>(ef->getCodePtr(ip));
}

void Machine::push(opcode_t opcode) {
	bool plain = mask(opcode, 4, 1);
	bool deref = !mask(opcode, 5, 1);
	valtype_t type = (valtype_t)mask(opcode, 6, 2);
	switch (type) {
		case Machine::vtB: mem_to_stack<imm8_t> (plain, deref); break;
		case Machine::vtW: mem_to_stack<imm16_t>(plain, deref); break;
		case Machine::vtD: mem_to_stack<imm32_t>(plain, deref); break;
		case Machine::vtQ: mem_to_stack<imm64_t>(plain, deref); break;
	}
}

void Machine::top(opcode_t opcode) {
	bool plain = mask(opcode, 4, 1);
	bool deref = !mask(opcode, 5, 1);
	valtype_t type = (valtype_t)mask(opcode, 6, 2);
	switch (type) {
		case Machine::vtB: stack_to_mem<imm8_t> (plain, deref); break;
		case Machine::vtW: stack_to_mem<imm16_t>(plain, deref); break;
		case Machine::vtD: stack_to_mem<imm32_t>(plain, deref); break;
		case Machine::vtQ: stack_to_mem<imm64_t>(plain, deref); break;
	}
}

void Machine::pop_discard(opcode_t opcode) {
	stack.pop();
}

void Machine::swp(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top);
	stack.push(pretop);
}

void Machine::dpl(opcode_t opcode) {
	imm8_t num = read<imm8_t>(ef->getCodePtr(ip));

	assert(num > 0);
	std::vector<val_t> vals(num);
	for (auto it = vals.rbegin(); it != vals.rend(); ++it) {
		*it = stack.top();
		stack.pop();
	}

	for (auto& val : vals) stack.push(val);
	for (auto& val : vals) stack.push(val);
}


void Machine::jmp(opcode_t opcode) {
	bool plain = mask(opcode, 6, 1);
	bool deref = mask(opcode, 7, 1);

	ofs16_t targetOfs;
	if (plain) targetOfs = read<ofs16_t>(ef->getCodePtr(ip));
	else if (deref) targetOfs = read<ofs16_t>(ef->getDataPtr(read<ofs16_t>(ef->getCodePtr(ip))));
	else {
		targetOfs = stack.top();
		stack.pop();
	}

	ip = targetOfs;
}

void Machine::jmp_if_equal(opcode_t opcode) {
	bool plain = mask(opcode, 5, 1);
	bool deref = mask(opcode, 6, 1);
	bool zero = mask(opcode, 7, 1);

	if (zero) jump_if_unary([](val_t top) -> bool { return top == 0; }, getJumpTargetOfs(plain, deref));
	else jump_if([](val_t top, val_t pretop) -> bool { return top == pretop; }, getJumpTargetOfs(plain, deref));
}

void Machine::jmp_if(opcode_t opcode) {
	bool plain = mask(opcode, 3, 1);
	bool deref = mask(opcode, 4, 1);
	switch (mask(opcode, 5, 3)) // zero and comparator
	{
		case 0b000: jump_if([](val_t top, val_t pretop) -> bool { return top > pretop; }, getJumpTargetOfs(plain, deref)); break;
		case 0b001: jump_if([](val_t top, val_t pretop) -> bool { return top >= pretop; }, getJumpTargetOfs(plain, deref)); break;
		case 0b010: jump_if([](val_t top, val_t pretop) -> bool { return top < pretop; }, getJumpTargetOfs(plain, deref)); break;
		case 0b011: jump_if([](val_t top, val_t pretop) -> bool { return top <= pretop; }, getJumpTargetOfs(plain, deref)); break;
		case 0b100: jump_if_unary([](val_t top) -> bool { return top > 0; }, getJumpTargetOfs(plain, deref)); break;
		case 0b101: jump_if_unary([](val_t top) -> bool { return top >= 0; }, getJumpTargetOfs(plain, deref)); break;
		case 0b110: jump_if_unary([](val_t top) -> bool { return top < 0; }, getJumpTargetOfs(plain, deref)); break;
		case 0b111: jump_if_unary([](val_t top) -> bool { return top <= 0; }, getJumpTargetOfs(plain, deref)); break;
	}
}

ofs16_t Machine::getJumpTargetOfs(bool plain, bool deref) {
	ofs16_t targetOfs;
	if (plain) targetOfs = read<ofs16_t>(ef->getCodePtr(ip));
	else if (deref) targetOfs = read<ofs16_t>(ef->getDataPtr(read<ofs16_t>(ef->getCodePtr(ip))));
	else {
		targetOfs = stack.top();
		stack.pop();
	}
	return targetOfs;
}

void Machine::jump_if(bool (comparator)(val_t, val_t), ofs16_t targetOfs) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();

	if (comparator(top, pretop)) ip = targetOfs;
}

void Machine::jump_if_unary(bool (function)(val_t), ofs16_t targetOfs) {
	val_t top = stack.top();
	stack.pop();

	if (function(top)) ip = targetOfs;
}

void Machine::call(opcode_t opcode) {
	imm8_t argsNumber = read<imm8_t>(ef->getCodePtr(ip));
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(ip));

	std::vector<val_t> args(argsNumber);
	for (int i = argsNumber - 1; i >=0; --i) {
		args[i] = stack.top();
		stack.pop();
	}
	
	// POPPING caller's arguments on its stack by default - use dpl to save them
	// for (auto entry : args) stack.push(entry); 
	
	stack.push(ip);
	stack.push(csz);
	csz = stack.size();

	for (auto entry : args) stack.push(entry);

	ip = targetOfs;
}

void Machine::ret(opcode_t opcode) {
	if (csz == ROOT_CSZ) {
		exit = EXIT_SUCCESS;
		return;
	}

	imm8_t argsNumber = read<imm8_t>(ef->getCodePtr(ip));

	if (stack.size() < csz + argsNumber) {
		assert(false);
		throw Error("Subroutine attempted to return " + std::to_string(argsNumber) + " values, but it has only " + 
			std::to_string(stack.size() - csz) + " left on the stack");
	}

	while (stack.size() > csz + argsNumber) stack.pop();

	std::vector<val_t> values(argsNumber);
	for (int i = argsNumber - 1; i >= 0; --i) {
		values[i] = stack.top();
		stack.pop();
	}

	csz = stack.top();
	stack.pop();

	ip = stack.top();
	stack.pop();

	for (auto entry : values) stack.push(entry);
}

void Machine::add(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();

	if (mask(opcode, 7, 1)) stack.push(top - pretop);
	else stack.push(top + pretop);
}

void Machine::inc(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();

	if (mask(opcode, 7, 1)) stack.push(top - 1);
	else stack.push(top + 1);
}

void Machine::mlt(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	if (mask(opcode, 7, 1)) stack.push(top / pretop);
	else stack.push(top * pretop);
}

void Machine::mod(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top % pretop);
}

void Machine::band(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top & pretop);
}

void Machine::bor(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top | pretop);
}

void Machine::bxor(opcode_t opcode) {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top ^ pretop);
}

void Machine::inp(opcode_t opcode) {
	bool to_stack = mask(opcode, 3, 1);
	bool as_char = mask(opcode, 4, 1);
	bool deref = mask(opcode, 5, 1);
	valtype_t type = (valtype_t)mask(opcode, 6, 2);

	if (as_char) input<char>(to_stack, deref);
	else {
		switch (type)
		{
		case Machine::vtB: input<imm8_t>(to_stack, deref, true); break;
		case Machine::vtW: input<imm16_t>(to_stack, deref); break;
		case Machine::vtD: input<imm32_t>(to_stack, deref); break;
		case Machine::vtQ: input<imm64_t>(to_stack, deref); break;
		}
	}
}

void Machine::out(opcode_t opcode) {
	bool from_stack = mask(opcode, 3, 1);
	bool as_char = mask(opcode, 4, 1);
	bool deref = mask(opcode, 5, 1);
	valtype_t type = (valtype_t)mask(opcode, 6, 2);

	if (as_char) output<char>(from_stack, deref);
	else {
		switch (type)
		{
		case Machine::vtB: output<imm8_t>(from_stack, deref, true); break;
		case Machine::vtW: output<imm16_t>(from_stack, deref); break;
		case Machine::vtD: output<imm32_t>(from_stack, deref); break;
		case Machine::vtQ: output<imm64_t>(from_stack, deref); break;
		}
	}
}
