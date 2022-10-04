#include "cpu.h"

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

void emu_cpu_init() {
	cpu.reset = _reset_cpu;
	cpu.exec = _exec_cpu;
}
