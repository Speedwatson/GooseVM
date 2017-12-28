#pragma once
#include "stdafx.h"
#include "ExecutableFile.hpp"

#define ROOT_CSZ -1
#define CONTINUE_EXIT_CODE -1
typedef int64_t val_t;
typedef uint8_t opcode_t;

class Machine {
public:
	int run(ExecutableFile* execfile);

private:
	int32_t csz = ROOT_CSZ;
	ofs16_t ip = 0;

	std::stack<val_t> stack;
	ExecutableFile* ef;

	int exit = CONTINUE_EXIT_CODE;

	template<typename T>
	T read(void* ptr, bool incIP = true) {
		if (incIP) ip += sizeof(T);
		return *(T*)ptr;
	}

	uint8_t mask(opcode_t opcode, int begin, int len) { return (opcode >> (sizeof(opcode) - begin - len)) & ((1 << len) - 1); }

	enum valtype_t {
		vtB = 0b00,
		vtW = 0b01,
		vtD = 0b10,
		vtQ = 0b11
	};

	ofs16_t getValOfs(bool deref) {
		if (deref) return read<ofs16_t>(ef->getCodePtr(ip));
		else {
			ofs16_t valAddr = stack.top();
			stack.pop();
			return valAddr;
		}
	}

	template<typename T>
	void mem_to_stack(bool plain, bool deref) {
		if (plain) stack.push(read<T>(ef->getCodePtr(ip)));
		else stack.push(read<T>(ef->getDataPtr(getValOfs(deref)));
	}

	template<typename T>
	void stack_to_mem(bool save, bool deref) {
		void* destPtr = ef->getDataPtr(getValOfs(deref));

		val_t top = stack.top();
		if (!save) stack.pop();

		*(T*)destPtr = (T)top;
	}

	ofs16_t Machine::getJumpTargetOfs(bool plain, bool deref);
	void jump_if(bool (comparator)(val_t, val_t), ofs16_t targetOfs);
	void jump_if_unary(bool (function)(val_t), ofs16_t targetOfs);

	template<typename T>
	void input(bool deref, bool forced_num = false) {
		imm64_t valnum;
		T val;

		if (forced_num) {
			std::cin >> valnum;
			val = (T)valnum;
		}
		else std::cin >> val;
		
		ofs16_t destOfs;
		if (deref) destOfs = read<ofs16_t>(ef->getCodePtr(ip));
		else {
			destOfs = stack.top();
			stack.pop();
		}
		
		*(T*)ef->getDataPtr(destOfs) = val;
	}

	template<typename T>
	void output(bool deref, bool forced_num = false) {
		imm64_t valnum;
		T val;

		if (!deref) {
			val = stack.top();
			stack.pop();
		}
		else {
			ofs16_t destOfs = read<ofs16_t>(ef->getCodePtr(ip));
			val = *(T*)ef->getDataPtr(destOfs);
		}

		if (forced_num) {
			valnum = val;
			std::cout << valnum;
		}
		else std::cout << val;
	}

	typedef void(Machine::*hndlptr_t)(opcode_t);

	hndlptr_t handlers[256] = {
		/* 00 */ &Machine::nop,
		/* 01 */ &Machine::brk,
		/* 02 */ &Machine::stop,
		/* 03 */ &Machine::stop,
		/* 04 */ nullptr,
		/* 05 */ nullptr,
		/* 06 */ nullptr,
		/* 07 */ nullptr,
		/* 08 */ nullptr,
		/* 09 */ nullptr,
		/* 0A */ nullptr,
		/* 0B */ nullptr,
		/* 0C */ nullptr,
		/* 0D */ nullptr,
		/* 0E */ nullptr,
		/* 0F */ nullptr,
		/* 10 */ nullptr,
		/* 11 */ nullptr,
		/* 12 */ nullptr,
		/* 13 */ nullptr,
		/* 14 */ nullptr,
		/* 15 */ nullptr,
		/* 16 */ nullptr,
		/* 17 */ nullptr,
		/* 18 */ nullptr,
		/* 19 */ nullptr,
		/* 1A */ nullptr,
		/* 1B */ nullptr,
		/* 1C */ nullptr,
		/* 1D */ nullptr,
		/* 1E */ nullptr,
		/* 1F */ nullptr,
		/* 20 */ nullptr,
		/* 21 */ nullptr,
		/* 22 */ nullptr,
		/* 23 */ nullptr,
		/* 24 */ nullptr,
		/* 25 */ nullptr,
		/* 26 */ nullptr,
		/* 27 */ nullptr,
		/* 28 */ nullptr,
		/* 29 */ nullptr,
		/* 2A */ nullptr,
		/* 2B */ nullptr,
		/* 2C */ nullptr,
		/* 2D */ nullptr,
		/* 2E */ nullptr,
		/* 2F */ nullptr,
		/* 30 */ nullptr,
		/* 31 */ nullptr,
		/* 32 */ nullptr,
		/* 33 */ nullptr,
		/* 34 */ nullptr,
		/* 35 */ nullptr,
		/* 36 */ nullptr,
		/* 37 */ nullptr,
		/* 38 */ nullptr,
		/* 39 */ nullptr,
		/* 3A */ nullptr,
		/* 3B */ nullptr,
		/* 3C */ nullptr,
		/* 3D */ nullptr,
		/* 3E */ nullptr,
		/* 3F */ nullptr,
		/* 40 */ nullptr,
		/* 41 */ nullptr,
		/* 42 */ nullptr,
		/* 43 */ nullptr,
		/* 44 */ nullptr,
		/* 45 */ nullptr,
		/* 46 */ nullptr,
		/* 47 */ nullptr,
		/* 48 */ nullptr,
		/* 49 */ nullptr,
		/* 4A */ nullptr,
		/* 4B */ nullptr,
		/* 4C */ nullptr,
		/* 4D */ nullptr,
		/* 4E */ nullptr,
		/* 4F */ nullptr,
		/* 50 */ &Machine::add,
		/* 51 */ &Machine::add,
		/* 52 */ &Machine::inc,
		/* 53 */ &Machine::inc,
		/* 54 */ &Machine::mlt,
		/* 55 */ &Machine::mlt,
		/* 56 */ &Machine::mod,
		/* 57 */ &Machine::band,
		/* 58 */ &Machine::bor,
		/* 59 */ &Machine::bxor,
		/* 5A */ nullptr,
		/* 5B */ nullptr,
		/* 5C */ nullptr,
		/* 5D */ nullptr,
		/* 5E */ nullptr,
		/* 5F */ nullptr,
		/* 60 */ &Machine::io,
		/* 61 */ &Machine::io,
		/* 62 */ &Machine::io,
		/* 63 */ &Machine::io,
		/* 64 */ &Machine::io,
		/* 65 */ &Machine::io,
		/* 66 */ &Machine::io,
		/* 67 */ &Machine::io,
		/* 68 */ &Machine::io,
		/* 69 */ &Machine::io,
		/* 6A */ &Machine::io,
		/* 6B */ &Machine::io,
		/* 6C */ &Machine::io,
		/* 6D */ &Machine::io,
		/* 6E */ &Machine::io,
		/* 6F */ &Machine::io,
		/* 70 */ &Machine::io,
		/* 71 */ &Machine::io,
		/* 72 */ &Machine::io,
		/* 73 */ &Machine::io,
		/* 74 */ &Machine::io,
		/* 75 */ &Machine::io,
		/* 76 */ &Machine::io,
		/* 77 */ &Machine::io,
		/* 78 */ &Machine::io,
		/* 79 */ &Machine::io,
		/* 7A */ &Machine::io,
		/* 7B */ &Machine::io,
		/* 7C */ &Machine::io,
		/* 7D */ &Machine::io,
		/* 7E */ &Machine::io,
		/* 7F */ &Machine::io,
		/* 80 */ &Machine::jmp_if,
		/* 81 */ &Machine::jmp_if,
		/* 82 */ &Machine::jmp_if,
		/* 83 */ &Machine::jmp_if,
		/* 84 */ &Machine::jmp_if,
		/* 85 */ &Machine::jmp_if,
		/* 86 */ &Machine::jmp_if,
		/* 87 */ &Machine::jmp_if,
		/* 88 */ &Machine::jmp_if,
		/* 89 */ &Machine::jmp_if,
		/* 8A */ &Machine::jmp_if,
		/* 8B */ &Machine::jmp_if,
		/* 8C */ &Machine::jmp_if,
		/* 8D */ &Machine::jmp_if,
		/* 8E */ &Machine::jmp_if,
		/* 8F */ &Machine::jmp_if,
		/* 90 */ &Machine::jmp_if,
		/* 91 */ &Machine::jmp_if,
		/* 92 */ &Machine::jmp_if,
		/* 93 */ &Machine::jmp_if,
		/* 94 */ &Machine::jmp_if,
		/* 95 */ &Machine::jmp_if,
		/* 96 */ &Machine::jmp_if,
		/* 97 */ &Machine::jmp_if,
		/* 98 */ &Machine::jmp_if,
		/* 99 */ &Machine::jmp_if,
		/* 9A */ &Machine::jmp_if,
		/* 9B */ &Machine::jmp_if,
		/* 9C */ &Machine::jmp_if,
		/* 9D */ &Machine::jmp_if,
		/* 9E */ &Machine::jmp_if,
		/* 9F */ &Machine::jmp_if,
		/* A0 */ &Machine::jmp_if_equal,
		/* A1 */ &Machine::jmp_if_equal,
		/* A2 */ &Machine::jmp_if_equal,
		/* A3 */ &Machine::jmp_if_equal,
		/* A4 */ &Machine::jmp_if_equal,
		/* A5 */ &Machine::jmp_if_equal,
		/* A6 */ &Machine::jmp_if_equal,
		/* A7 */ &Machine::jmp_if_equal,
		/* A8 */ &Machine::jmp,
		/* A9 */ &Machine::jmp,
		/* AA */ &Machine::jmp,
		/* AB */ &Machine::jmp,
		/* AC */ &Machine::call,
		/* AD */ &Machine::ret,
		/* AE */ nullptr,
		/* AF */ nullptr,
		/* B0 */ nullptr,
		/* B1 */ nullptr,
		/* B2 */ nullptr,
		/* B3 */ nullptr,
		/* B4 */ nullptr,
		/* B5 */ nullptr,
		/* B6 */ nullptr,
		/* B7 */ nullptr,
		/* B8 */ nullptr,
		/* B9 */ nullptr,
		/* BA */ nullptr,
		/* BB */ nullptr,
		/* BC */ nullptr,
		/* BD */ nullptr,
		/* BE */ nullptr,
		/* BF */ nullptr,
		/* C0 */ &Machine::swp,
		/* C1 */ &Machine::dpl,
		/* C2 */ nullptr,
		/* C3 */ nullptr,
		/* C4 */ nullptr,
		/* C5 */ nullptr,
		/* C6 */ nullptr,
		/* C7 */ nullptr,
		/* C8 */ nullptr,
		/* C9 */ nullptr,
		/* CA */ nullptr,
		/* CB */ nullptr,
		/* CC */ nullptr,
		/* CD */ nullptr,
		/* CE */ nullptr,
		/* CF */ nullptr,
		/* D0 */ nullptr,
		/* D1 */ nullptr,
		/* D2 */ nullptr,
		/* D3 */ nullptr,
		/* D4 */ nullptr,
		/* D5 */ nullptr,
		/* D6 */ nullptr,
		/* D7 */ nullptr,
		/* D8 */ nullptr,
		/* D9 */ nullptr,
		/* DA */ nullptr,
		/* DB */ nullptr,
		/* DC */ nullptr,
		/* DD */ nullptr,
		/* DE */ nullptr,
		/* DF */ nullptr,
		/* E0 */ &Machine::push,
		/* E1 */ &Machine::push,
		/* E2 */ &Machine::push,
		/* E3 */ &Machine::push,
		/* E4 */ &Machine::push,
		/* E5 */ &Machine::push,
		/* E6 */ &Machine::push,
		/* E7 */ &Machine::push,
		/* E8 */ &Machine::push,
		/* E9 */ &Machine::push,
		/* EA */ &Machine::push,
		/* EB */ &Machine::push,
		/* EC */ &Machine::push,
		/* ED */ &Machine::push,
		/* EE */ &Machine::push,
		/* EF */ &Machine::push,
		/* F0 */ &Machine::top,
		/* F1 */ &Machine::top,
		/* F2 */ &Machine::top,
		/* F3 */ &Machine::top,
		/* F4 */ &Machine::top,
		/* F5 */ &Machine::top,
		/* F6 */ &Machine::top,
		/* F7 */ &Machine::top,
		/* F8 */ &Machine::top,
		/* F9 */ &Machine::top,
		/* FA */ &Machine::top,
		/* FB */ &Machine::top,
		/* FC */ &Machine::top,
		/* FD */ &Machine::top,
		/* FE */ &Machine::top,
		/* FF */ &Machine::top,
	};

	void nop(opcode_t opcode);
	void brk(opcode_t opcode);
	void stop(opcode_t opcode);
	void add(opcode_t opcode);
	void inc(opcode_t opcode);
	void mlt(opcode_t opcode);
	void mod(opcode_t opcode);
	void band(opcode_t opcode);
	void bor(opcode_t opcode);
	void bxor(opcode_t opcode);
	void io(opcode_t opcode);
	void jmp_if(opcode_t opcode);
	void jmp_if_equal(opcode_t opcode);
	void jmp(opcode_t opcode);
	void call(opcode_t opcode);
	void ret(opcode_t opcode);
	void swp(opcode_t opcode);
	void dpl(opcode_t opcode);
	void push(opcode_t opcode);
	void top(opcode_t opcode);
};

