#include "stdafx.h"
#include "Machine.hpp"
#include "Exceptions.hpp"

void Machine::nop() {
	return;
}

void Machine::push_ofs() {
	stack.push(read<ofs16_t>(ef->getCodePtr(++ip)));
}

void Machine::push_b() {
	stack.push(read<imm8_t>(ef->getCodePtr(++ip)));
}

void Machine::push_w() {
	stack.push(read<imm16_t>(ef->getCodePtr(++ip)));
}

void Machine::push_d() {
	stack.push(read<imm32_t>(ef->getCodePtr(++ip)));
}

void Machine::push_q() {
	stack.push(read<imm64_t>(ef->getCodePtr(++ip)));
}

void Machine::push_by_ofs_b() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(read<imm8_t>(ef->getDataPtr(srcofs), false));
}

void Machine::push_by_ofs_w() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(read<imm16_t>(ef->getDataPtr(srcofs), false));
}

void Machine::push_by_ofs_d() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(read<imm32_t>(ef->getDataPtr(srcofs), false));
}

void Machine::push_by_ofs_q() {
	ofs16_t srcofs = read<ofs16_t>(ef->getCodePtr(++ip));
	stack.push(read<imm64_t>(ef->getDataPtr(srcofs), false));
}

void Machine::swp() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top);
	stack.push(pretop);
	++ip;
}

void Machine::pop_b() {
	stack_to_mem<imm8_t>(true);
}

void Machine::pop_w() {
	stack_to_mem<imm16_t>(true);
}

void Machine::pop_d() {
	stack_to_mem<imm32_t>(true);
}

void Machine::pop_q() {
	stack_to_mem<imm64_t>(true);
}

void Machine::sav_b() {
	stack_to_mem<imm8_t>();
}

void Machine::sav_w() {
	stack_to_mem<imm16_t>();
}

void Machine::sav_d() {
	stack_to_mem<imm32_t>();
}

void Machine::sav_q() {
	stack_to_mem<imm64_t>();
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

void Machine::jump_if(bool (comparator)(val_t, val_t), bool by_ofs) {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	if (by_ofs) targetOfs = read<ofs16_t>(ef->getDataPtr(targetOfs), false);

	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();

	//if (top.type != pretop.type) {
	//	assert(false);
	//	throw Error("Tried to compare two different types (on stack) in jump instruction.");
	//}

	if (comparator(top, pretop)) ip = targetOfs;
}

void Machine::jump_if_unary(bool (function)(val_t), bool by_ofs) {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	if (by_ofs) targetOfs = read<ofs16_t>(ef->getDataPtr(targetOfs), false);

	val_t top = stack.top();
	stack.pop();

	if (function(top)) ip = targetOfs;
}

void Machine::je() {
	jump_if([](val_t top, val_t pretop) -> bool { return top == pretop; });
}

void Machine::je_by_offset() {
	jump_if([](val_t top, val_t pretop) -> bool { return top == pretop; }, true);
}

void Machine::jl() {
	jump_if([](val_t top, val_t pretop) -> bool { return top < pretop; });
}

void Machine::jl_by_offset() {
	jump_if([](val_t top, val_t pretop) -> bool { return top < pretop; }, true);
}

void Machine::jg() {
	jump_if([](val_t top, val_t pretop) -> bool { return top > pretop; });
}

void Machine::jg_by_offset() {
	jump_if([](val_t top, val_t pretop) -> bool { return top > pretop; }, true);
}

void Machine::jle() {
	jump_if([](val_t top, val_t pretop) -> bool { return top <= pretop; });
}

void Machine::jle_by_offset() {
	jump_if([](val_t top, val_t pretop) -> bool { return top <= pretop; }, true);
}

void Machine::jge() {
	jump_if([](val_t top, val_t pretop) -> bool { return top >= pretop; });
}

void Machine::jge_by_offset() {
	jump_if([](val_t top, val_t pretop) -> bool { return top >= pretop; }, true);
}

void Machine::jez() {
	jump_if_unary([](val_t top) -> bool { return top == 0; });
}

void Machine::jez_by_offset() {
	jump_if_unary([](val_t top) -> bool { return top == 0; }, true);
}

void Machine::jgz() {
	jump_if_unary([](val_t top) -> bool { return top > 0; });
}

void Machine::jgz_by_offset() {
	jump_if_unary([](val_t top) -> bool { return top > 0; }, true);
}

void Machine::jlz() {
	jump_if_unary([](val_t top) -> bool { return top < 0; });
}

void Machine::jlz_by_offset() {
	jump_if_unary([](val_t top) -> bool { return top < 0; }, true);
}

void Machine::jgez() {
	jump_if_unary([](val_t top) -> bool { return top >= 0; });
}

void Machine::jgez_by_offset() {
	jump_if_unary([](val_t top) -> bool { return top >= 0; }, true);
}

void Machine::jlez() {
	jump_if_unary([](val_t top) -> bool { return top <= 0; });
}

void Machine::jlez_by_offset() {
	jump_if_unary([](val_t top) -> bool { return top <= 0; }, true);
}

void Machine::call() {
	ofs16_t targetOfs = read<ofs16_t>(ef->getCodePtr(++ip));
	imm8_t argsNumber = read<imm8_t>(ef->getCodePtr(ip));

	std::vector<val_t> args(argsNumber);
	for (int i = argsNumber - 1; i >=0; ++i) {
		args[i] = stack.top();
		stack.pop();
	}

	for (auto entry : args) stack.push(entry); // preserving caller's arguments on its stack by default

	stack.push(ip);
	stack.push(csz);
	csz = stack.size();

	for (auto entry : args) stack.push(entry);

	ip = targetOfs;
}

void Machine::ret() {
	if (csz == -1) {
		exit = EXIT_SUCCESS;
		return;
	}

	imm8_t argsNumber = read<imm8_t>(ef->getCodePtr(++ip));

	if (stack.size() < csz + argsNumber) {
		assert(false);
		throw Error("Subroutine attempted to return " + std::to_string(argsNumber) + " values, but it has only " + 
			std::to_string(stack.size() - csz) + " left on the stack");
	}

	while (stack.size() > csz + argsNumber) stack.pop();

	std::vector<val_t> values(argsNumber);
	for (int i = argsNumber - 1; i >= 0; ++i) {
		values[i] = stack.top();
		stack.pop();
	}

	//assert(stack.top().type == Entry::etW); // else stack corruption
	csz = stack.top();
	stack.pop();

	//assert(stack.top().type == Entry::etOfs); // else stack corruption
	ip = stack.top();
	stack.pop();

	for (auto entry : values) stack.push(entry);
}

void Machine::add() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top + pretop);
	++ip;
}

void Machine::sub() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top - pretop);
	++ip;
}

void Machine::inc() {
	val_t top = stack.top();
	stack.pop();
	stack.push(top + 1);
	++ip;
}

void Machine::dec() {
	val_t top = stack.top();
	stack.pop();
	stack.push(top - 1);
	++ip;
}

void Machine::mlt() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top * pretop);
	++ip;
}

void Machine::div() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top / pretop);
	++ip;
}

void Machine::mod() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top % pretop);
	++ip;
}

void Machine::bnot() {
	val_t top = stack.top();
	stack.pop();
	stack.push(!top);
	++ip; ++ip;
}

void Machine::band() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top & pretop);
	++ip;
}

void Machine::bor() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top | pretop);
	++ip;
}

void Machine::bxor() {
	val_t top = stack.top();
	stack.pop();
	val_t pretop = stack.top();
	stack.pop();
	stack.push(top ^ pretop);
	++ip;
}

void Machine::inp_b() {
	input<imm8_t>(false, true);
}

void Machine::inp_b_stack() {
	input<imm8_t>(true, true);
}

void Machine::inp_w() {
	input<imm16_t>();
}

void Machine::inp_w_stack() {
	input<imm16_t>(true);
}

void Machine::inp_d() {
	input<imm32_t>();
}

void Machine::inp_d_stack() {
	input<imm32_t>(true);
}

void Machine::inp_q() {
	input<imm64_t>();
}

void Machine::inp_q_stack() {
	input<imm64_t>(true);
}

void Machine::inp_char() {
	input<char>();
}

void Machine::inp_char_stack() {
	input<char>(true);
}

void Machine::out_b() {
	output<imm8_t>(false, true);
}

void Machine::out_b_stack() {
	output<imm8_t>(true, true);
}

void Machine::out_w() {
	output<imm16_t>();
}

void Machine::out_w_stack() {
	output<imm16_t>(true);
}


void Machine::out_d() {
	output<imm32_t>();
}

void Machine::out_d_stack() {
	output<imm32_t>(true);
}

void Machine::out_q() {
	output<imm64_t>();
}

void Machine::out_q_stack() {
	output<imm64_t>(true);
}

void Machine::out_char() {
	output<char>();
}

void Machine::out_char_stack() {
	output<char>(true);
}

void Machine::stop() {
	exit = read<imm8_t>(ef->getCodePtr(++ip));
}

void Machine::stop_ecstack() {
	++ip;
	exit = stack.top();
	stack.pop();
}