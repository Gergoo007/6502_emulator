#include "opcodes.h"
#include "main.h"

#include <stdio.h>

void lda_flagcheck() {
	if(cpu.A == 0)
		cpu.P.Z = 1;
	else
		cpu.P.Z = 0;
	
	if((cpu.A & 0b01000000) > 0)
		cpu.P.N = 1;
	else
		cpu.P.N = 0;
}

void ldx_flagcheck() {
	if(cpu.X == 0)
		cpu.P.Z = 1;
	else
		cpu.P.Z = 0;

	if((cpu.A & 0b01000000) > 0)
		cpu.P.N = 1;
	else
		cpu.P.N = 0;
}

void ldy_flagcheck() {
	if(cpu.Y == 0)
		cpu.P.Z = 1;
	else
		cpu.P.Z = 0;

	if((cpu.Y & 0b01000000) > 0)
		cpu.P.N = 1;
	else
		cpu.P.N = 0;
}

void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles) {
	byte arg1, arg2;
	switch (instruct) {
		case BRK:
			cycles -= 2;
			break;
		case NOP:
			cycles -= 2;
			break;

		case LDA_IM:
			cpu->A = mem->fetch(cycles);
			lda_flagcheck();
			break;

		case LDX_IM:
			cpu->X = mem->fetch(cycles);
			ldx_flagcheck();
			break;

		case LDY_IM:
			cpu->Y = mem->fetch(cycles);
			ldy_flagcheck();
			break;

		case LDA_ZP:
			cpu->A = mem->data[mem->fetch(cycles)];
			lda_flagcheck();
			break;

		case LDX_ZP:
			cpu->X = mem->data[mem->fetch(cycles)];
			ldx_flagcheck();
			break;

		case LDY_ZP:
			cpu->Y = mem->data[mem->fetch(cycles)];
			ldy_flagcheck();
			break;

		case LDA_ZX:
			cpu->A = mem->data[mem->fetch(cycles) + cpu->X];
			lda_flagcheck();
			break;
		
		case LDX_ZY:
			cpu->X = mem->data[mem->fetch(cycles) + cpu->Y];
			ldx_flagcheck();
			break;
		
		case LDY_ZX:
			cpu->Y = mem->data[mem->fetch(cycles) + cpu->X];
			ldy_flagcheck();
			break;

		case JMP_ABS:
			cpu->PC = mem->fetch(cycles) - 1;
			break;

		case JMP_IND:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			cpu->PC = mem->data[((arg1 * 0x100 + arg2) + 1)] * 0x100 + mem->data[(arg1 * 0x100 + arg2)];
			break;

		case PHA:
			mem->push(cpu->A, 2);
			cycles--;
			break;

		case PLA:
			cpu->SP--;
			cpu->A = mem->pop(cpu->SP - 1);
			cycles--;
			break;

		case PHP:
			mem->push(cpu->status_to_byte(cpu->P), cycles);
			break;

		case JSR:
			mem->push((cpu->PC & 0x00ff) + 2, cycles);
			mem->push(cpu->PC >> 8, cycles);

			cpu->PC = (mem->fetch(cycles) + (mem->fetch(cycles) << 8));
			break;

		case RTS:
			cpu->PC = (mem->pop(cycles) << 8) + mem->pop(cycles);
			break;

		case INX:
			cpu->X++;
			ldx_flagcheck();
			break;

		case INY:
			cpu->Y++;
			ldy_flagcheck();
			break;

		case STA_ABS:
			mem->data[(mem->fetch(cycles) << 8) + mem->fetch(cycles)] = cpu->A;
			break;

		case STA_ABS_X:
			mem->data[((mem->fetch(cycles) << 8) + mem->fetch(cycles)) + cpu->X] = cpu->A;
			break;

		case STA_ABS_Y:
			mem->data[((mem->fetch(cycles) << 8) + mem->fetch(cycles)) + cpu->Y] = cpu->A;
			break;

		case STA_ZP:
			mem->data[mem->fetch(cycles)] = cpu->A;
			break;

		case STA_ZP_X:
			mem->data[mem->fetch(cycles) + cpu->X] = cpu->A;
			break;

		case STA_IND_X:
			mem->data[mem->data[(mem->fetch(cycles) << 8) + mem->fetch(cycles) + cpu->X]] = cpu->A;
			cpu->A += cpu->X;
			break;

		case STA_IND_Y:
			mem->data[mem->data[(mem->fetch(cycles) << 8) + mem->fetch(cycles)] + cpu->Y] = cpu->A;
			break;

		case STX_ABS:
			mem->data[(mem->fetch(cycles) << 8) + mem->fetch(cycles)] = cpu->X;
			break;

		case STX_ZP:
			mem->data[mem->fetch(cycles)] = cpu->X;
			break;

		case STX_ZP_Y:
			mem->data[mem->fetch(cycles) + cpu->Y] = cpu->X;
			break;

		case STY_ABS:
			mem->data[(mem->fetch(cycles) << 8) + mem->fetch(cycles)] = cpu->Y;
			break;

		case STY_ZP:
			mem->data[mem->fetch(cycles)] = cpu->Y;
			break;

		case STY_ZP_X:
			mem->data[mem->fetch(cycles) + cpu->X] = cpu->Y;
			break;

		case TAX:
			cpu->X = cpu->A;
			break;

		case TXA:
			cpu->A = cpu->X;
			break;

		case TAY:
			cpu->Y = cpu->A;
			break;

		case TYA:
			cpu->A = cpu->Y;
			break;

		case TXS:
			cpu->X = cpu->SP;
			break;

		case TSX:
			cpu->SP = cpu->X;
			break;

		default:
			printf("Illegal instruction: %02x\n", instruct);
			break;
	}
}