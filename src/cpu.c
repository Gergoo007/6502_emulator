#include "cpu.h"
#include "opcodes.h"
#include <stdio.h>

void _reset_cpu(Memory* mem) {
	cpu.PC = mem->data[0xfffd] * 0x100 + mem->data[0xfffc];
	cpu.SP = 0xff;
	cpu.X = cpu.Y = cpu.A = 0;
	memset(&cpu.P, 0, sizeof(cpu.P));
}

void _exec_cpu_step(uint32_t steps, Memory *mem) {
	while (steps > 0) {
		byte instruct = mem->fetch(0xff);

		if(instruct == 0x00)
			break;

		execute(instruct, &cpu, mem, 0xff);
		steps--;
	}
}

void _exec_cpu_cycle(uint32_t cycles, Memory *mem) {
	while (cycles > 0) {
		byte instruct = mem->fetch(cycles);

		execute(instruct, &cpu, mem, cycles);

		cycles--;
	}
}

void _exec_cpu_cont(Memory *mem) {
	cpu.irq();
	while (1) {
		byte instruct = mem->fetch(0xffff);
		if(instruct == BRK)
			break;
		execute(instruct, &cpu, mem, 0xffff);
	}
}

void _nmi() {
	mem.push((cpu.PC & 0x00ff), 0xff);
	mem.push(cpu.PC >> 8, 0xff);
	cpu.PC = (mem.data[0xfffb] << 8) + mem.data[0xfffa];
}

void _irq() {
	mem.push((cpu.PC & 0x00ff), 0xff);
	mem.push(cpu.PC >> 8, 0xff);
	cpu.PC = (mem.data[0xffff] << 8) + mem.data[0xfffe];
}

byte status_to_byte(P_type P) {
	byte result = 0b00000000;
	result |= cpu.P.C << 7;
	result |= cpu.P.Z << 6;
	result |= cpu.P.I << 5;
	result |= cpu.P.D << 4;
	result |= cpu.P.B << 3;
	result |= cpu.P.U << 2;
	result |= cpu.P.V << 1;
	result |= cpu.P.N;

	return result;
}

void emu_cpu_init() {
	cpu.reset = _reset_cpu;
	cpu.exec_by_cycles 	= _exec_cpu_cycle;
	cpu.exec_by_step 	= _exec_cpu_step;
	cpu.exec_continous 	= _exec_cpu_cont;
	cpu.status_to_byte 	= status_to_byte;
	cpu.nmi = _nmi;
	cpu.irq = _irq;
}
