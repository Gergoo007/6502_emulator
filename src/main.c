#include "cpu.h"
#include "memory.h"
#include "opcodes.h"
#include "interface.h"

#include <stdio.h>
#include <string.h>

CPU cpu_glob;
Memory mem_glob;

void init_sim(void) { 
	emu_cpu_init();
	emu_mem_init();
}

void dump_stack() {
	printf("Stack: \n");
	for (uint32_t i = 0x100; i < 0x200; i += 16) {
		printf("0x%04x: ", i);
		for (uint32_t j = 0; j < 16; j++) {
			printf("0x%02x ", mem_glob.data[i + j]);
		}
		printf("\n");
	}
}

void dump_mem() {
	printf("Stack: \n");
	for (uint32_t i = 0x0; i < 0x0100; i += 16) {
		printf("0x%04x: ", i);
		for (uint32_t j = 0; j < 16; j++) {
			printf("0x%02x ", mem_glob.data[i + j]);
		}
		printf("\n");
	}
}


int main(int argc, char** argv) {
	init_sim();

	mem_glob.init();

	cpu_glob.reset(&mem_glob);
	
	//cpu.exec_by_step(0x0004, &mem);
	//cpu.exec_continous(&mem);

	// Debug
	//printf("A: 0x%02x; X: 0x%02x; Y: 0x%02x\n", cpu.A, cpu.X, cpu.Y);
	//printf("PC: 0x%04x; SP: 0x%02x\n", cpu.PC, cpu.SP);

	//dump_stack();

	//printf("\nBuffer:\n%s", buffer);

	open_gui(argc, argv, &cpu_glob, &mem_glob);

	return 0;
}
