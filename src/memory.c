#include "memory.h"
#include "cpu.h"

byte _fetch_mem(uint32_t cycles, Memory* mem) {
	cycles--;
	return mem->data[(cpu.PC)++];
}

void _init_mem(void) {
	for (uint32_t i = 0; i < MAX_MEM; i++) {
		mem.data[i] = 0x00;
	}
}

void emu_mem_init() {
	mem.fetch = _fetch_mem;
	mem.init = _init_mem;
}