#pragma once
#include "stdafx.h"
#include "ExecutableFile.hpp"

#define ROOT_CSZ -1

struct Entry {
	int64_t val;
	enum Type { etB, etW, etD, etQ, etOfs } type;

	Entry(int64_t val, Type type) { this->type = type; this->val = val; }
};

class Machine {
public:
	int run(ExecutableFile* execfile);

private:
	uint16_t csz = ROOT_CSZ;
	ofs16_t ip = 0;

	std::stack<Entry> stack;
	ExecutableFile* ef;

	int exit = 0;

	template<typename T>
	T read(void* ptr, bool incIP = true) {
		if (incIP) ip += sizeof(T);
		return *(T*)ptr;
	}

	typedef void (Machine::*InstrPtr)();

	InstrPtr handler[256] = {
		&Machine::nop,				// 000 0 0000
		nullptr,				// 000 0 0001
		&Machine::push_ofs,			// 000 0 0010
		&Machine::push_b,				// 000 0 0011
		&Machine::push_w,				// 000 0 0100
		&Machine::push_d,				// 000 0 0101
		&Machine::push_q,				// 000 0 0110
		&Machine::push_by_ofs_b,		// 000 0 0111
		&Machine::push_by_ofs_w,		// 000 0 1000
		&Machine::push_by_ofs_d,		// 000 0 1001
		&Machine::push_by_ofs_q,		// 000 0 1010
		nullptr,				// 000 0 1011
		&Machine::sav,				// 000 0 1100
		nullptr,					// 000 0 1101
		&Machine::pop_b,					// 000 0 1110
		&Machine::pop_w,					// 000 0 1111
		&Machine::pop_d,					// 000 1 0000
		&Machine::pop_q,					// 000 1 0001
		&Machine::swp_b,					// 000 1 0010
		&Machine::swp_w,					// 000 1 0011
		&Machine::swp_d,					// 000 1 0100
		&Machine::swp_q,					// 000 1 0101
		nullptr,					// 000 1 0110
		nullptr,					// 000 1 0111
		nullptr,					// 000 1 1000
		nullptr,					// 000 1 1001
		nullptr,					// 000 1 1010
		nullptr,					// 000 1 1011
		nullptr,					// 000 1 1100
		nullptr,					// 000 1 1101
		nullptr,					// 000 1 1110
		nullptr,					// 000 1 1111
		&Machine::jmp,				// 001 0 0000
		&Machine::jmp_by_offset,		// 001 0 0001
		&Machine::je,					// 001 0 0010
		&Machine::je_by_offset,		// 001 0 0011
		&Machine::jl,					// 001 0 0100
		&Machine::jl_by_offset,		// 001 0 0101
		&Machine::jg,					// 001 0 0110
		&Machine::jg_by_offset,		// 001 0 0111
		&Machine::jle,				// 001 0 1000
		&Machine::jle_by_offset,		// 001 0 1001
		&Machine::jge,				// 001 0 1010
		&Machine::jge_by_offset,		// 001 0 1011
		&Machine::jez,				// 001 0 1100
		&Machine::jez_by_offset,		// 001 0 1101
		&Machine::jgz,				// 001 0 1110
		&Machine::jgz_by_offset,		// 001 0 1111
		&Machine::jlz,				// 001 1 0000
		&Machine::jlz_by_offset,		// 001 1 0001
		&Machine::jgez,				// 001 1 0010
		&Machine::jgez_by_offset,		// 001 1 0011
		&Machine::jlez,				// 001 1 0100
		&Machine::jlez_by_offset,		// 001 1 0101
		&Machine::call,				// 001 1 0110
		nullptr,					// 001 1 0111
		&Machine::ret,				// 001 1 1000
		nullptr,					// 001 1 1001
		nullptr,					// 001 1 1010
		nullptr,					// 001 1 1011
		nullptr,					// 001 1 1100
		nullptr,					// 001 1 1101
		nullptr,					// 001 1 1110
		nullptr,					// 001 1 1111
		&Machine::add,				// 010 0 0000
		&Machine::sub,				// 010 0 0001
		&Machine::inc,				// 010 0 0010
		&Machine::dec,				// 010 0 0011
		&Machine::mlt,				// 010 0 0100
		&Machine::div,				// 010 0 0101
		&Machine::mod,				// 010 0 0110
		nullptr,					// 010 0 0111
		nullptr,					// 010 0 1000
		nullptr,					// 010 0 1001
		nullptr,					// 010 0 1010
		nullptr,					// 010 0 1011
		nullptr,					// 010 0 1100
		nullptr,					// 010 0 1101
		nullptr,					// 010 0 1110
		nullptr,					// 010 0 1111
		nullptr,					// 010 1 0000
		nullptr,					// 010 1 0001
		nullptr,					// 010 1 0010
		nullptr,					// 010 1 0011
		nullptr,					// 010 1 0100
		nullptr,					// 010 1 0101
		nullptr,					// 010 1 0110
		nullptr,					// 010 1 0111
		nullptr,					// 010 1 1000
		nullptr,					// 010 1 1001
		nullptr,					// 010 1 1010
		nullptr,					// 010 1 1011
		nullptr,					// 010 1 1100
		nullptr,					// 010 1 1101
		nullptr,					// 010 1 1110
		nullptr,					// 010 1 1111
		&Machine::bnot,				// 011 0 0000
		&Machine::band,				// 011 0 0001
		&Machine::bor,				// 011 0 0010
		&Machine::bxor,				// 011 0 0011
		nullptr,					// 011 0 0100
		nullptr,					// 011 0 0101
		nullptr,					// 011 0 0110
		nullptr,					// 011 0 0111
		nullptr,					// 011 0 1000
		nullptr,					// 011 0 1001
		nullptr,					// 011 0 1010
		nullptr,					// 011 0 1011
		nullptr,					// 011 0 1100
		nullptr,					// 011 0 1101
		nullptr,					// 011 0 1110
		nullptr,					// 011 0 1111
		nullptr,					// 011 1 0000
		nullptr,					// 011 1 0001
		nullptr,					// 011 1 0010
		nullptr,					// 011 1 0011
		nullptr,					// 011 1 0100
		nullptr,					// 011 1 0101
		nullptr,					// 011 1 0110
		nullptr,					// 011 1 0111
		nullptr,					// 011 1 1000
		nullptr,					// 011 1 1001
		nullptr,					// 011 1 1010
		nullptr,					// 011 1 1011
		nullptr,					// 011 1 1100
		nullptr,					// 011 1 1101
		nullptr,					// 011 1 1110
		nullptr,					// 011 1 1111
		&Machine::inp_b,				// 100 0 0000
		&Machine::inp_b_stack,		// 100 0 0001
		&Machine::inp_w,				// 100 0 0010
		&Machine::inp_w_stack,		// 100 0 0011
		&Machine::inp_d,				// 100 0 0100
		&Machine::inp_d_stack,		// 100 0 0101
		&Machine::inp_q,				// 100 0 0110
		&Machine::inp_q_stack,		// 100 0 0111
		&Machine::inp_char,			// 100 0 1000
		&Machine::inp_char_stack,		// 100 0 1001
		&Machine::out_b,				// 100 0 1010
		&Machine::out_w,				// 100 0 1011
		&Machine::out_d,				// 100 0 1100
		&Machine::out_q,				// 100 0 1101
		&Machine::out_char,			// 100 0 1110
		&Machine::out_stack,			// 100 0 1111
		nullptr,					// 100 1 0000
		nullptr,					// 100 1 0001
		nullptr,					// 100 1 0010
		nullptr,					// 100 1 0011
		nullptr,					// 100 1 0100
		nullptr,					// 100 1 0101
		nullptr,					// 100 1 0110
		nullptr,					// 100 1 0111
		nullptr,					// 100 1 1000
		nullptr,					// 100 1 1001
		nullptr,					// 100 1 1010
		nullptr,					// 100 1 1011
		nullptr,					// 100 1 1100
		nullptr,					// 100 1 1101
		nullptr,					// 100 1 1110
		nullptr,					// 100 1 1111
		&Machine::stop,				// 101 0 0000
		&Machine::stop_ecstack,		// 101 0 0001
		nullptr,					// 101 0 0010
		nullptr,					// 101 0 0011
		nullptr,					// 101 0 0100
		nullptr,					// 101 0 0101
		nullptr,					// 101 0 0110
		nullptr,					// 101 0 0111
		nullptr,					// 101 0 1000
		nullptr,					// 101 0 1001
		nullptr,					// 101 0 1010
		nullptr,					// 101 0 1011
		nullptr,					// 101 0 1100
		nullptr,					// 101 0 1101
		nullptr,					// 101 0 1110
		nullptr,					// 101 0 1111
		nullptr,					// 101 1 0000
		nullptr,					// 101 1 0001
		nullptr,					// 101 1 0010
		nullptr,					// 101 1 0011
		nullptr,					// 101 1 0100
		nullptr,					// 101 1 0101
		nullptr,					// 101 1 0110
		nullptr,					// 101 1 0111
		nullptr,					// 101 1 1000
		nullptr,					// 101 1 1001
		nullptr,					// 101 1 1010
		nullptr,					// 101 1 1011
		nullptr,					// 101 1 1100
		nullptr,					// 101 1 1101
		nullptr,					// 101 1 1110
		nullptr,					// 101 1 1111
		nullptr,					// 110 0 0000
		nullptr,					// 110 0 0001
		nullptr,					// 110 0 0010
		nullptr,					// 110 0 0011
		nullptr,					// 110 0 0100
		nullptr,					// 110 0 0101
		nullptr,					// 110 0 0110
		nullptr,					// 110 0 0111
		nullptr,					// 110 0 1000
		nullptr,					// 110 0 1001
		nullptr,					// 110 0 1010
		nullptr,					// 110 0 1011
		nullptr,					// 110 0 1100
		nullptr,					// 110 0 1101
		nullptr,					// 110 0 1110
		nullptr,					// 110 0 1111
		nullptr,					// 110 1 0000
		nullptr,					// 110 1 0001
		nullptr,					// 110 1 0010
		nullptr,					// 110 1 0011
		nullptr,					// 110 1 0100
		nullptr,					// 110 1 0101
		nullptr,					// 110 1 0110
		nullptr,					// 110 1 0111
		nullptr,					// 110 1 1000
		nullptr,					// 110 1 1001
		nullptr,					// 110 1 1010
		nullptr,					// 110 1 1011
		nullptr,					// 110 1 1100
		nullptr,					// 110 1 1101
		nullptr,					// 110 1 1110
		nullptr,					// 110 1 1111
		nullptr,					// 111 0 0000
		nullptr,					// 111 0 0001
		nullptr,					// 111 0 0010
		nullptr,					// 111 0 0011
		nullptr,					// 111 0 0100
		nullptr,					// 111 0 0101
		nullptr,					// 111 0 0110
		nullptr,					// 111 0 0111
		nullptr,					// 111 0 1000
		nullptr,					// 111 0 1001
		nullptr,					// 111 0 1010
		nullptr,					// 111 0 1011
		nullptr,					// 111 0 1100
		nullptr,					// 111 0 1101
		nullptr,					// 111 0 1110
		nullptr,					// 111 0 1111
		nullptr,					// 111 1 0000
		nullptr,					// 111 1 0001
		nullptr,					// 111 1 0010
		nullptr,					// 111 1 0011
		nullptr,					// 111 1 0100
		nullptr,					// 111 1 0101
		nullptr,					// 111 1 0110
		nullptr,					// 111 1 0111
		nullptr,					// 111 1 1000
		nullptr,					// 111 1 1001
		nullptr,					// 111 1 1010
		nullptr,					// 111 1 1011
		nullptr,					// 111 1 1100
		nullptr,					// 111 1 1101
		nullptr,					// 111 1 1110
		nullptr,					// 111 1 1111
	};

	void stack_to_mem(bool pop = false);
	void jump_if(bool (comparator)(uint64_t, uint64_t), bool by_ofs = false);
	void jump_if_unary(bool (function)(uint64_t), bool by_ofs = false);

	void nop();
	void push_ofs();
	void push_b();
	void push_w();
	void push_d();
	void push_q();
	void push_by_ofs_b();
	void push_by_ofs_w();
	void push_by_ofs_d();
	void push_by_ofs_q();
	void pop_b();
	void pop_w();
	void pop_d();
	void pop_q();
	void swp_b();
	void swp_w();
	void swp_d();
	void swp_q();
	void sav();
	void jmp();
	void jmp_by_offset();
	void je();
	void je_by_offset();
	void jl();
	void jl_by_offset();
	void jg();
	void jg_by_offset();
	void jle();
	void jle_by_offset();
	void jge();
	void jge_by_offset();
	void jez();
	void jez_by_offset();
	void jgz();
	void jgz_by_offset();
	void jlz();
	void jlz_by_offset();
	void jgez();
	void jgez_by_offset();
	void jlez();
	void jlez_by_offset();
	void call();
	void ret();
	void add();
	void sub();
	void inc();
	void dec();
	void mlt();
	void div();
	void mod();
	void bnot();
	void band();
	void bor();
	void bxor();
	void inp_b();
	void inp_b_stack();
	void inp_w();
	void inp_w_stack();
	void inp_d();
	void inp_d_stack();
	void inp_q();
	void inp_q_stack();
	void inp_char();
	void inp_char_stack();
	void out_b();
	void out_w();
	void out_d();
	void out_q();
	void out_char();
	void out_stack();
	void stop();
	void stop_ecstack();
};
