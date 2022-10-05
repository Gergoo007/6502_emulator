#include "cpu.h"
#include "memory.h"
#include "opcodes.h"
#include "image_reader.h"

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

void dump_mem() {
	printf("Stack: \n");
	for (uint32_t i = 0x0; i < 0x0100; i += 16) {
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

	read_image(&mem);

	cpu.reset(&mem);
	
	//cpu.exec_by_step(0x00aa, &mem);
	cpu.exec_continous(&mem);

	// Debug
	printf("A: 0x%02x; X: 0x%02x; Y: 0x%02x\n", cpu.A, cpu.X, cpu.Y);
	printf("PC: 0x%04x; SP: 0x%02x\n", cpu.PC, cpu.SP);

	dump_stack();

	return 0;
}
