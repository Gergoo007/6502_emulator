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
#define LDA_ABS	0xad
#define LDA_ZP 	0xa5
#define LDA_ZX 	0xb5

#define LDX_IM 	0xa2
#define LDX_ZP 	0xa6
#define LDX_ZY 	0xb6

#define LDY_IM 	0xa0
#define LDY_ZP 	0xa4
#define LDY_ZX 	0xb4

//
// STORE
//
#define STA_ABS		0x8d
#define STA_ABS_X	0x9d
#define STA_ABS_Y	0x99
#define STA_ZP		0x85
#define STA_ZP_X	0x95
#define STA_IND_X	0x81
#define STA_IND_Y	0x91

#define STX_ABS		0x8e
#define STX_ZP		0x86
#define STX_ZP_Y	0x96

#define STY_ABS		0x8c
#define STY_ZP		0x84
#define STY_ZP_X	0x94

//
//	TRANSFER
//
#define TAX 		0xaa
#define TAY 		0xa8
#define TSX			0xba
#define TXS			0x9a
#define TXA			0x8a
#define TYA			0x98

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
// BRANCH
//
#define BEQ 	0xf0
#define BNE 	0xd0
#define BCC		0x90
#define BCS		0xb0
#define BPL		0x10
#define BMI		0x30
#define BVC		0x50
#define BVS		0x70

//
//	MISCELLANEOUS
//	
#define NOP		0xea
#define BRK		0x00

extern void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles);