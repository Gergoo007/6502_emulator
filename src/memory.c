#include "memory.h"
#include "cpu.h"

byte _fetch_mem(uint32_t cycles) {
	cycles--;
	return mem.data[(cpu.PC)++];
}

void _init_mem(void) {
	for (uint32_t i = 0; i < MAX_MEM; i++) {
		mem.data[i] = 0x00;
	}
}

void push(byte value, uint32_t cycles) {
	cycles--;
	mem.data[0x0100 + cpu.SP] = value;
	cpu.SP--;
}

byte pop(uint32_t cycles) {
	cycles--;
	cpu.SP++;
	byte val = mem.data[0x100 + cpu.SP];
	mem.data[0x100 + cpu.SP + 1] = 0x00;
	return val;
}

void emu_mem_init() {
	mem.fetch = _fetch_mem;
	mem.init = _init_mem;
	mem.push = push;
	mem.pop = pop;
}
