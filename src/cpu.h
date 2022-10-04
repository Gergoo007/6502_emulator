#pragma once

#include "main.h"
#include "memory.h"

typedef struct P_type {
	byte C : 1; // Carry
	byte Z : 1;	// Zero
	byte I : 1; // IRQ enabled
	byte D : 1; // Decimal mode
	byte B : 1; // Break
	byte U : 1; // Unused
	byte V : 1; // Overflow
	byte N : 1; // Negative
} P_type;

typedef struct CPU {
	word 	PC; 			// Program Counter
	word 	SP; 			// Stack Pointer
	P_type 	P;				// Processor status

	byte 	A; 				// Accumulator
	byte 	X, Y;			// Index

	void (*reset) (Memory*);			// Reset the processor
	void (*exec) (uint32_t, Memory*);	// Execute instruction
} CPU;

extern CPU cpu;

void emu_cpu_init();
void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles);