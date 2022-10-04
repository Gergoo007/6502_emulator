#include "opcodes.h"
#include "main.h"

#include <stdio.h>

void lda_flagcheck() {
	if(cpu.A == 0)
		cpu.P.Z = 1;
	if((cpu.A & 0b10000000) > 0)
		cpu.P.N = 1;
}

void ldx_flagcheck() {
	if(cpu.X == 0)
		cpu.P.Z = 1;
	if((cpu.A & 0b10000000) > 0)
		cpu.P.N = 1;
}

void ldy_flagcheck() {
	if(cpu.Y == 0)
		cpu.P.Z = 1;
	if((cpu.Y & 0b10000000) > 0)
		cpu.P.N = 1;
}

void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles) {
	byte arg1, arg2;
	switch (instruct) {
		case 0x00:
			break;

		case LDA_IM:
			cpu->A = mem->fetch(cycles, mem);
			lda_flagcheck();
			break;

		case LDX_IM:
			cpu->X = mem->fetch(cycles, mem);
			ldx_flagcheck();
			break;

		case LDY_IM:
			cpu->Y = mem->fetch(cycles, mem);
			ldy_flagcheck();
			break;

		case LDA_ZP:
			cpu->A = mem->data[mem->fetch(cycles, mem)];
			lda_flagcheck();
			break;

		case LDX_ZP:
			cpu->X = mem->data[mem->fetch(cycles, mem)];
			ldx_flagcheck();
			break;

		case LDY_ZP:
			cpu->Y = mem->data[mem->fetch(cycles, mem)];
			ldy_flagcheck();
			break;

		case LDA_ZX:
			cpu->A = mem->data[mem->fetch(cycles, mem) + cpu->X];
			lda_flagcheck();
			break;
		
		case LDX_ZY:
			cpu->X = mem->data[mem->fetch(cycles, mem) + cpu->Y];
			ldx_flagcheck();
			break;
		
		case LDY_ZX:
			cpu->Y = mem->data[mem->fetch(cycles, mem) + cpu->X];
			ldy_flagcheck();
			break;

		case JMP_ABS:
			cpu->PC = mem->fetch(cycles, mem) - 1;
			break;

		case JMP_IND:
			arg1 = mem->fetch(cycles, mem); // ff
			arg2 = mem->fetch(cycles, mem); // fc
			cpu->PC = mem->data[((arg1 * 0x100 + arg2) + 1)] * 0x100 + mem->data[(arg1 * 0x100 + arg2)];
			break;

		default:
			printf("Illegal instruction: %02x\n", instruct);
			break;
	}
}