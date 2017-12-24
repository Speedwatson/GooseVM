#include "stdafx.h"
#include "Machine.hpp"
#include "Exceptions.hpp"

void Machine::nop() {
	return;
}

void Machine::stack_to_mem(bool pop = false) {
	ofs16_t destOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	void* destPtr = ef->getDataPtr(destOfs);

	Entry top = stack.top();
	if (pop) stack.pop();

	switch (top.type)
	{
	case Entry::etB: *(imm8_t*)destPtr = top.val;
	case Entry::etW: *(imm16_t*)destPtr = top.val;
	case Entry::etD: *(imm32_t*)destPtr = top.val;
	case Entry::etQ: *(imm64_t*)destPtr = top.val;
	case Entry::etOfs: *(ofs16_t*)destPtr = top.val;
	default: assert(false);
	}
}

void Machine::pop() {
	stack_to_mem(true);
}

void Machine::push_ofs() {
	stack.push(Entry(read<ofs16_t>(ef->getCodePtr(++ip)), Entry::etOfs));
}

void Machine::push_b() {
	stack.push(Entry(read<imm8_t>(ef->getCodePtr(++ip)), Entry::etB));
}

void Machine::push_w() {
	stack.push(Entry(read<imm16_t>(ef->getCodePtr(++ip)), Entry::etW));
}

void Machine::push_d() {
	stack.push(Entry(read<imm32_t>(ef->getCodePtr(++ip)), Entry::etD));
}

void Machine::push_q() {
	stack.push(Entry(read<imm64_t>(ef->getCodePtr(++ip)), Entry::etQ));
}

void Machine::push_by_ofs_b() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(Entry(read<imm8_t>(ef->getDataPtr(srcofs), false), Entry::etB));
}

void Machine::push_by_ofs_w() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(Entry(read<imm16_t>(ef->getDataPtr(srcofs), false), Entry::etW));
}

void Machine::push_by_ofs_d() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(Entry(read<imm32_t>(ef->getDataPtr(srcofs), false), Entry::etD));
}

void Machine::push_by_ofs_q() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(Entry(read<imm64_t>(ef->getDataPtr(srcofs), false), Entry::etQ));
}

void Machine::swp() {
	Entry top = stack.top();
	stack.pop();
	Entry pretop = stack.top();
	stack.pop();
	stack.push(top);
	stack.push(pretop);
	++ip;
}

void Machine::sav() {
	stack_to_mem();
}

void Machine::jmp() {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	ip = targetOfs;
}

void Machine::jmp_by_offset() {
	ofs16_t srcOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	ofs16_t targetOfs = read<ofs16_t>(ef->getDataPtr(srcOfs), false);
	ip = targetOfs;
}

void Machine::jump_if(bool (comparator)(uint64_t, uint64_t), bool by_ofs = false) {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	if (by_ofs) targetOfs = read<ofs16_t>(ef->getDataPtr(targetOfs), false);

	Entry top = stack.top();
	stack.pop();
	Entry pretop = stack.top();
	stack.pop();

	if (top.type != pretop.type) {
		assert(false);
		throw Error("Tried to compare two different types (on stack) in jump instruction.");
	}

	if (comparator(top.val, pretop.val)) ip = targetOfs;
}

void Machine::jump_if_unary(bool (function)(uint64_t), bool by_ofs = false) {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	if (by_ofs) targetOfs = read<ofs16_t>(ef->getDataPtr(targetOfs), false);

	Entry top = stack.top();
	stack.pop();

	if (function(top.val)) ip = targetOfs;
}

void Machine::je() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top == pretop; });
}

void Machine::je_by_offset() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top == pretop; }, true);
}

void Machine::jl() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top < pretop; });
}

void Machine::jl_by_offset() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top < pretop; }, true);
}

void Machine::jg() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top > pretop; });
}

void Machine::jg_by_offset() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top > pretop; }, true);
}

void Machine::jle() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top <= pretop; });
}

void Machine::jle_by_offset() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top <= pretop; }, true);
}

void Machine::jge() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top >= pretop; });
}

void Machine::jge_by_offset() {
	jump_if([](uint64_t top, uint64_t pretop) -> bool { return top >= pretop; }, true);
}

void Machine::jez() {
	jump_if_unary([](uint64_t top) -> bool { return top == 0; });
}

void Machine::jez_by_offset() {
	jump_if_unary([](uint64_t top) -> bool { return top == 0; }, true);
}

void Machine::jgz() {
	jump_if_unary([](uint64_t top) -> bool { return top > 0; });
}

void Machine::jgz_by_offset() {
	jump_if_unary([](uint64_t top) -> bool { return top > 0; }, true);
}

void Machine::jlz() {
	jump_if_unary([](uint64_t top) -> bool { return top < 0; });
}

void Machine::jlz_by_offset() {
	jump_if_unary([](uint64_t top) -> bool { return top < 0; }, true);
}

void Machine::jgez() {
	jump_if_unary([](uint64_t top) -> bool { return top >= 0; });
}

void Machine::jgez_by_offset() {
	jump_if_unary([](uint64_t top) -> bool { return top >= 0; }, true);
}

void Machine::jlez() {
	jump_if_unary([](uint64_t top) -> bool { return top <= 0; });
}

void Machine::jlez_by_offset() {
	jump_if_unary([](uint64_t top) -> bool { return top <= 0; }, true);
}

void Machine::call() {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	imm8_t argsNumber = read<imm8_t>(ef->getCodePtr(ip));

	std::vector<Entry> args(argsNumber);
	for (int i = argsNumber - 1; i >=0; ++i) {
		args[i] = stack.top();
		stack.pop();
	}

	for (auto entry : args) stack.push(entry); // preserving caller's arguments on its stack by default

	stack.push(Entry(ip, Entry::etOfs));
	stack.push(Entry(csz, Entry::etW));
	csz = stack.size();

	for (auto entry : args) stack.push(entry);

	ip = targetOfs;
}

void Machine::ret() {
	imm8_t argsNumber = read<imm8_t>(ef->getCodePtr(++ip));

	if (stack.size() < csz + argsNumber) {
		assert(false);
		throw Error("Subroutine attempted to return " + std::to_string(argsNumber) + " values, but it has only " + 
			std::to_string(stack.size() - csz) + " left on the stack");
	}

	while (stack.size() > csz + argsNumber) stack.pop();

	std::vector<Entry> values(argsNumber);
	for (int i = argsNumber - 1; i >= 0; ++i) {
		values[i] = stack.top();
		stack.pop();
	}

	assert(stack.top().type == Entry::etW); // else stack corruption
	csz = stack.top().val;
	stack.pop();

	assert(stack.top().type == Entry::etOfs); // else stack corruption
	ip = stack.top().val;
	stack.pop();

	for (auto entry : values) stack.push(entry);
}

void Machine::add() {
	Entry top = stack.top();
	stack.pop();
	Entry pretop = stack.top();
	stack.pop();
	

}

void Machine::sub() {
	// INSTR TODO
}

void Machine::inc() {
	// INSTR TODO
}

void Machine::dec() {
	// INSTR TODO
}

void Machine::mlt() {
	// INSTR TODO
}

void Machine::div() {
	// INSTR TODO
}

void Machine::mod() {
	// INSTR TODO
}

void Machine::bnot() {
	// INSTR TODO
}

void Machine::band() {
	// INSTR TODO
}

void Machine::bor() {
	// INSTR TODO
}

void Machine::bxor() {
	// INSTR TODO
}

void Machine::inp_b() {
	// INSTR TODO
}

void Machine::inp_b_stack() {
	// INSTR TODO
}

void Machine::inp_w() {
	// INSTR TODO
}

void Machine::inp_w_stack() {
	// INSTR TODO
}

void Machine::inp_d() {
	// INSTR TODO
}

void Machine::inp_d_stack() {
	// INSTR TODO
}

void Machine::inp_q() {
	// INSTR TODO
}

void Machine::inp_q_stack() {
	// INSTR TODO
}

void Machine::inp_char() {
	// INSTR TODO
}

void Machine::inp_char_stack() {
	// INSTR TODO
}

void Machine::out_b() {
	// INSTR TODO
}

void Machine::out_w() {
	// INSTR TODO
}

void Machine::out_d() {
	// INSTR TODO
}

void Machine::out_q() {
	// INSTR TODO
}

void Machine::out_char() {
	// INSTR TODO
}

void Machine::out_stack() {
	// INSTR TODO
}

void Machine::stop() {
	// INSTR TODO
}

void Machine::stop_ecstack() {
	// INSTR TODO
}