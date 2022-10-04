#pragma once

#include <stdint.h>

#define MAX_MEM 1024 * 64

// 1 byte = 8 bits (max 0xff, 256)
typedef uint8_t byte;

// 2 bytes = 16 bits (max 0xffff, 65536)
typedef uint16_t word;

typedef struct Memory {
	byte data[MAX_MEM];

	void (*init) (void);
	byte (*fetch) (uint32_t, struct Memory*);

	void (*push) (byte value);
	byte (*pop) (void);
} Memory;

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
extern Memory mem;
