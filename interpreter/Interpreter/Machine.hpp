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

	uint8_t mask(opcode_t opcode, int begin, int len) { return (opcode >> (sizeof(opcode) * 8 - begin - len)) & ((1 << len) - 1); }
	int getLine(int ip = -1);

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
		else stack.push(read<T>(ef->getDataPtr(getValOfs(deref)), false));
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
	void input(bool to_stack, bool deref, bool forced_num = false) {
		imm64_t valnum;
		T val; 

		if (forced_num) {
			std::cin >> valnum;
			val = (T)valnum;
		}
		else std::cin >> val;
		
		if (to_stack) stack.push(val);
		else {
			ofs16_t destOfs;
			if (deref) destOfs = read<ofs16_t>(ef->getCodePtr(ip));
			else {
				destOfs = stack.top();
				stack.pop();
			}

			*(T*)ef->getDataPtr(destOfs) = val;
		}
	}

	template<typename T>
	void output(bool from_stack, bool deref, bool forced_num = false) {
		imm64_t valnum;
		T val;

		if (!from_stack) {
			ofs16_t destOfs;
			if (!deref) {
				destOfs = stack.top();
				stack.pop();
			}
			else destOfs = read<ofs16_t>(ef->getCodePtr(ip));
			val = *(T*)ef->getDataPtr(destOfs);
		}
		else {
			val = stack.top();
			stack.pop();
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
		/* 20 */ &Machine::inp,
		/* 21 */ &Machine::inp,
		/* 22 */ &Machine::inp,
		/* 23 */ &Machine::inp,
		/* 24 */ &Machine::inp,
		/* 25 */ &Machine::inp,
		/* 26 */ &Machine::inp,
		/* 27 */ &Machine::inp,
		/* 28 */ &Machine::inp,
		/* 29 */ &Machine::inp,
		/* 2A */ &Machine::inp,
		/* 2B */ &Machine::inp,
		/* 2C */ &Machine::inp,
		/* 2D */ &Machine::inp,
		/* 2E */ &Machine::inp,
		/* 2F */ &Machine::inp,
		/* 30 */ &Machine::inp,
		/* 31 */ &Machine::inp,
		/* 32 */ &Machine::inp,
		/* 33 */ &Machine::inp,
		/* 34 */ &Machine::inp,
		/* 35 */ &Machine::inp,
		/* 36 */ &Machine::inp,
		/* 37 */ &Machine::inp,
		/* 38 */ &Machine::inp,
		/* 39 */ &Machine::inp,
		/* 3A */ &Machine::inp,
		/* 3B */ &Machine::inp,
		/* 3C */ &Machine::inp,
		/* 3D */ &Machine::inp,
		/* 3E */ &Machine::inp,
		/* 3F */ &Machine::inp,
		/* 40 */ &Machine::out,
		/* 41 */ &Machine::out,
		/* 42 */ &Machine::out,
		/* 43 */ &Machine::out,
		/* 44 */ &Machine::out,
		/* 45 */ &Machine::out,
		/* 46 */ &Machine::out,
		/* 47 */ &Machine::out,
		/* 48 */ &Machine::out,
		/* 49 */ &Machine::out,
		/* 4A */ &Machine::out,
		/* 4B */ &Machine::out,
		/* 4C */ &Machine::out,
		/* 4D */ &Machine::out,
		/* 4E */ &Machine::out,
		/* 4F */ &Machine::out,
		/* 50 */ &Machine::out,
		/* 51 */ &Machine::out,
		/* 52 */ &Machine::out,
		/* 53 */ &Machine::out,
		/* 54 */ &Machine::out,
		/* 55 */ &Machine::out,
		/* 56 */ &Machine::out,
		/* 57 */ &Machine::out,
		/* 58 */ &Machine::out,
		/* 59 */ &Machine::out,
		/* 5A */ &Machine::out,
		/* 5B */ &Machine::out,
		/* 5C */ &Machine::out,
		/* 5D */ &Machine::out,
		/* 5E */ &Machine::out,
		/* 5F */ &Machine::out,
		/* 60 */ &Machine::add,
		/* 61 */ &Machine::add,
		/* 62 */ &Machine::inc,
		/* 63 */ &Machine::inc,
		/* 64 */ &Machine::mlt,
		/* 65 */ &Machine::mlt,
		/* 66 */ &Machine::mod,
		/* 67 */ &Machine::band,
		/* 68 */ &Machine::bor,
		/* 69 */ &Machine::bxor,
		/* 6A */ nullptr,
		/* 6B */ nullptr,
		/* 6C */ nullptr,
		/* 6D */ nullptr,
		/* 6E */ nullptr,
		/* 6F */ nullptr,
		/* 70 */ nullptr,
		/* 71 */ nullptr,
		/* 72 */ nullptr,
		/* 73 */ nullptr,
		/* 74 */ nullptr,
		/* 75 */ nullptr,
		/* 76 */ nullptr,
		/* 77 */ nullptr,
		/* 78 */ nullptr,
		/* 79 */ nullptr,
		/* 7A */ nullptr,
		/* 7B */ nullptr,
		/* 7C */ nullptr,
		/* 7D */ nullptr,
		/* 7E */ nullptr,
		/* 7F */ nullptr,
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
		/* C2 */ &Machine::pop_discard,
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
	void inp(opcode_t opcode);
	void out(opcode_t opcode);
	void jmp_if(opcode_t opcode);
	void jmp_if_equal(opcode_t opcode);
	void jmp(opcode_t opcode);
	void call(opcode_t opcode);
	void ret(opcode_t opcode);
	void swp(opcode_t opcode);
	void dpl(opcode_t opcode);
	void push(opcode_t opcode);
	void top(opcode_t opcode);
	void pop_discard(opcode_t opcode);
};

