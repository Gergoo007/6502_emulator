#pragma once

#include "main.h"
#include "cpu.h"
#include "memory.h"

//
// instructions paired to their byte
//

//
//	LOAD
//
#define LDA_IM 	0xa9
#define LDA_ZP 	0xa5
#define LDA_ZX 	0xb5

#define LDX_IM 	0xa2
#define LDX_ZP 	0xa6
#define LDX_ZY 	0xb6

#define LDY_IM 	0xa0
#define LDY_ZP 	0xa4
#define LDY_ZX 	0xb4

//
// INCREMENT
//
#define INX 	0xe8
#define INY 	0xc8

//
//	JUMP
//
#define JMP_ABS 0x4c
#define JMP_IND	0x6c

//
// STACK
//
#define PHA 	0x48
#define PLA		0x68
#define PHP		0x08
#define PLP		0x28

//
// SUBROUTINES
//
#define JSR		0x20
#define RTS		0x60

//
//	MISCELLANEOUS
//	
#define NOP		0xea

extern void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles);