#include "main.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>

CPU cpu;
Memory mem;

void _reset_cpu(Memory* mem) {
	cpu.PC = mem->data[0xfffd] * 0x100 + mem->data[0xfffc];
	cpu.SP = 0x0100;
	cpu.X = cpu.Y = cpu.A = 0;
	memset(&cpu.P, 0, sizeof(cpu.P));
}

void _exec_cpu(uint32_t cycles, Memory *mem) {
	while (cycles > 0) {
		byte instruct = mem->fetch(cycles, mem);

		execute(instruct, &cpu, mem, cycles);

		cycles--;
	}
}

byte _fetch_mem(uint32_t cycles, Memory* mem) {
	cycles--;
	return mem->data[(cpu.PC)++];
}

void _init_mem(void) { for (uint32_t i = 0; i < MAX_MEM; i++) { mem.data[i] = 0x00; } }

void init_sim(void) { 
	cpu.reset = _reset_cpu; 
	cpu.exec = _exec_cpu;

	mem.fetch = _fetch_mem;
	mem.init = _init_mem;
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
