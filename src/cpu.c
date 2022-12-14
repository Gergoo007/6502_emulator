#include "cpu.h"
#include "opcodes.h"
#include "interface.h"
#include "image_reader.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

extern uint8_t kill_emu_thread;

void _reset_cpu(Memory* mem) {
	cpu_glob.SP = 0xff;
	cpu_glob.X = cpu_glob.Y = cpu_glob.A = 0;
	memset(&cpu_glob.P, 0, sizeof(cpu_glob.P));
	memset(mem_glob.data, 0, 0x10000);
	read_image(&mem_glob);
	cpu_glob.PC = mem->data[0xfffd] * 0x100 + mem->data[0xfffc];
}

void _exec_cpu_step(uint32_t steps, Memory *mem) {
	while (steps > 0) {
		byte instruct = mem->fetch(0xff);

		if(instruct == 0x00)
			break;

		execute(instruct, &cpu_glob, mem, 0xff);
		steps--;
	}
}

void _exec_cpu_cycle(uint32_t cycles, Memory *mem) {
	while (cycles > 0) {
		byte instruct = mem->fetch(cycles);

		execute(instruct, &cpu_glob, mem, cycles);

		cycles--;
	}
}

void _exec_cpu_cont(Memory *mem) {
	while (!kill_emu_thread) {
		byte instruct = mem->fetch(0xffff);
		if(instruct == BRK)
			break;
		
		execute(instruct, &cpu_glob, mem, 0xffff);
	}
}

void _nmi() {
	mem_glob.push((cpu_glob.PC & 0x00ff), 0xff);
	mem_glob.push(cpu_glob.PC >> 8, 0xff);
	cpu_glob.PC = (mem_glob.data[0xfffb] << 8) + mem_glob.data[0xfffa];
}

void _irq() {
	mem_glob.push((cpu_glob.PC & 0x00ff), 0xff);
	mem_glob.push(cpu_glob.PC >> 8, 0xff);
	cpu_glob.PC = (mem_glob.data[0xffff] << 8) + mem_glob.data[0xfffe];
}

byte status_to_byte(P_type P) {
	byte result = 0b00000000;
	result |= cpu_glob.P.C << 7;
	result |= cpu_glob.P.Z << 6;
	result |= cpu_glob.P.I << 5;
	result |= cpu_glob.P.D << 4;
	result |= cpu_glob.P.B << 3;
	result |= cpu_glob.P.U << 2;
	result |= cpu_glob.P.V << 1;
	result |= cpu_glob.P.N;

	return result;
}

void emu_cpu_init() {
	cpu_glob.reset = _reset_cpu;
	cpu_glob.exec_by_cycles 	= _exec_cpu_cycle;
	cpu_glob.exec_by_step 	= _exec_cpu_step;
	cpu_glob.exec_continous 	= _exec_cpu_cont;
	cpu_glob.status_to_byte 	= status_to_byte;
	cpu_glob.nmi = _nmi;
	cpu_glob.irq = _irq;
}
