#include "cpu.h"
#include "memory.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

CPU cpu;
Memory mem;

void init_sim(void) { 
	emu_cpu_init();
	emu_mem_init();
}

void dump_stack() {
	printf("Stack: \n");
	for (uint32_t i = 0x100; i < 0x200; i += 16) {
		printf("0x%04x: ", i);
		for (uint32_t j = 0; j < 16; j++) {
			printf("0x%02x ", mem.data[i + j]);
		}
		printf("\n");
	}
}

int main(void) {
	init_sim();

	mem.init();

	mem.data[0xfffc] = 0x00;
	mem.data[0xfffd] = 0x80;

	mem.data[0x8000] = JSR;
	mem.data[0x8001] = 0x03;
	mem.data[0x8002] = 0x80;
	mem.data[0x8003] = LDA_IM;
	mem.data[0x8004] = 0x36;
	mem.data[0x8005] = PHP;
	cpu.reset(&mem);
	
	cpu.exec(0x0c, &mem);

	// Debug
	printf("A: 0x%02x; X: 0x%02x; Y: 0x%02x\n", cpu.A, cpu.X, cpu.Y);
	printf("PC: 0x%04x; SP: 0x%02x\n", cpu.PC, cpu.SP);

	dump_stack();

	return 0;
}
