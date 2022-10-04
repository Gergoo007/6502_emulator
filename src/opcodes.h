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
#define LDA_IM 0xa9
#define LDA_ZP 0xa5
#define LDA_ZX 0xb5

#define LDX_IM 0xa2
#define LDX_ZP 0xa6
#define LDX_ZY 0xb6

#define LDY_IM 0xa0
#define LDY_ZP 0xa4
#define LDY_ZX 0xb4

//
//	JUMP
//
#define JMP_ABS 0x4c
#define JMP_IND 0x6c

extern void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles);