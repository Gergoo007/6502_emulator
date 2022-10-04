#include "cpu.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

CPU cpu;
Memory mem;

void init_sim(void) { 
	emu_cpu_init();
	emu_mem_init();
}

int main(void) {
	init_sim();

	mem.init();

	mem.data[0xfffc] = 0x00;
	mem.data[0xfffd] = 0x80;

	mem.data[0x8000] = 0xa2;
	mem.data[0x8001] = 0x10;
	mem.data[0x8002] = 0xb5;
	mem.data[0x8003] = 0x33;

	mem.data[0x8004] = 0x6c;
	mem.data[0x8005] = 0xff;
	mem.data[0x8006] = 0xfc;
	
	cpu.reset(&mem);
	
	cpu.exec(6, &mem);

	// Debug
	printf("A: 0x%02x; X: 0x%02x; Y: 0x%02x\n", cpu.A, cpu.X, cpu.Y);
	printf("PC: 0x%04x; SP: 0x%04x\n", cpu.PC, cpu.SP);

	return 0;
}
