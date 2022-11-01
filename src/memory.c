#include "memory.h"
#include "cpu.h"

byte _fetch_mem(uint32_t cycles) {
	cycles--;
	return mem_glob.data[(cpu_glob.PC)++];
}

void _init_mem(void) {
	for (uint32_t i = 0; i < MAX_MEM; i++) {
		mem_glob.data[i] = 0x00;
	}
}

void push(byte value, uint32_t cycles) {
	cycles--;
	mem_glob.data[0x0100 + cpu_glob.SP] = value;
	cpu_glob.SP--;
}

byte pop(uint32_t cycles) {
	byte val = mem_glob.data[0x100 + cpu_glob.SP + 1];
	mem_glob.data[0x100 + cpu_glob.SP + 1] = 0x00;
	cycles--;
	cpu_glob.SP++;
	return val;
}

void emu_mem_init() {
	mem_glob.fetch = _fetch_mem;
	mem_glob.init = _init_mem;
	mem_glob.push = push;
	mem_glob.pop = pop;
}
