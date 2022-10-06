#include "opcodes.h"
#include "main.h"

#include <stdio.h>

void dump_stackk() {
	printf("Stack: \n");
	for (uint32_t i = 0x100; i < 0x200; i += 16) {
		printf("0x%04x: ", i);
		for (uint32_t j = 0; j < 16; j++) {
			printf("0x%02x ", mem.data[i + j]);
		}
		printf("\n");
	}
}


void lda_flagcheck() {
	if(cpu.A == 0)
		cpu.P.Z = 1;
	else
		cpu.P.Z = 0;
	
	if((cpu.A & 0b10000000) > 0)
		cpu.P.N = 1;
	else
		cpu.P.N = 0;
}

void ldx_flagcheck() {
	if(cpu.X == 0)
		cpu.P.Z = 1;
	else
		cpu.P.Z = 0;

	if((cpu.A & 0b10000000) > 0)
		cpu.P.N = 1;
	else
		cpu.P.N = 0;
}

void ldy_flagcheck() {
	if(cpu.Y == 0)
		cpu.P.Z = 1;
	else
		cpu.P.Z = 0;

	if((cpu.Y & 0b10000000) > 0)
		cpu.P.N = 1;
	else
		cpu.P.N = 0;
}

void execute(byte instruct, CPU *cpu, Memory *mem, uint32_t cycles) {
	byte arg1, arg2, arg3;
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

		case LDA_ABS:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			printf("\n %04x \n", (arg2 << 8) + arg1);
			cpu->A = mem->data[(arg2 << 8) + arg1];
			lda_flagcheck();
			break;

		case LDA_ZP:
			cpu->A = mem->data[mem->fetch(cycles)];
			lda_flagcheck();
			break;

		case LDA_ZX:
			cpu->A = mem->data[mem->fetch(cycles) + cpu->X];
			lda_flagcheck();
			break;

		case LDX_IM:
			cpu->X = mem->fetch(cycles);
			ldx_flagcheck();
			break;

		case LDX_ZP:
			cpu->X = mem->data[mem->fetch(cycles)];
			ldx_flagcheck();
			break;

		case LDX_ZY:
			cpu->X = mem->data[mem->fetch(cycles) + cpu->Y];
			ldx_flagcheck();
			break;

		case LDY_IM:
			cpu->Y = mem->fetch(cycles);
			ldy_flagcheck();
			break;

		case LDY_ZP:
			cpu->Y = mem->data[mem->fetch(cycles)];
			ldy_flagcheck();
			break;
		
		case LDY_ZX:
			cpu->Y = mem->data[mem->fetch(cycles) + cpu->X];
			ldy_flagcheck();
			break;

		case JMP_ABS:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			cpu->PC = arg1 + (arg2 << 8);
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

		case RTI:
			arg1 = mem->pop(cycles);
			arg2 = mem->pop(cycles);
			cpu->PC = arg2 + arg1 * 0x100;
			break;

		case INX:
			cpu->X++;
			ldx_flagcheck();
			break;

		case INY:
			cpu->Y++;
			ldy_flagcheck();
			break;

		case BNE:
			arg1 = mem->fetch(cycles);
			if(!(cpu->P.Z)) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BEQ:
			arg1 = mem->fetch(cycles);
			if(cpu->P.Z) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BCS:
			arg1 = mem->fetch(cycles);
			if(cpu->P.C) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BCC:
			arg1 = mem->fetch(cycles);
			if(!(cpu->P.C)) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BVS:
			arg1 = mem->fetch(cycles);
			if(cpu->P.V) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BVC:
			arg1 = mem->fetch(cycles);
			if(!(cpu->P.Z)) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BPL:
			arg1 = mem->fetch(cycles);
			if(!(cpu->P.N)) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case BMI:
			arg1 = mem->fetch(cycles);
			if(cpu->P.N) {
				if (arg1 <= 0x7f) {
					cpu->PC = cpu->PC + arg1;
				} else {
					arg1 = 0xff - arg1;
					cpu->PC -= arg1;
					cpu->PC--;
				}
			}
			break;

		case CMP_IM:
			arg1 = mem->fetch(cycles);
			cpu->P.Z = cpu->A == arg1;
			cpu->P.C = cpu->A >= arg1;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_ZP:
			arg1 = mem->data[mem->fetch(cycles)];
			cpu->P.Z = cpu->A == arg1;
			cpu->P.C = cpu->A >= arg1;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_ZP_X:
			arg1 = mem->data[mem->fetch(cycles) + cpu->X];
			cpu->P.Z = cpu->A == arg1;
			cpu->P.C = cpu->A >= arg1;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_ABS:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg1 = mem->data[(arg2 << 8) + arg1];
			cpu->P.Z = cpu->A == arg1;
			cpu->P.C = cpu->A >= arg1;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_ABS_X:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg1 = mem->data[(arg2 << 8) + arg1 + cpu->X];
			cpu->P.Z = cpu->A == arg1;
			cpu->P.C = cpu->A >= arg1;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_ABS_Y:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg1 = mem->data[(arg2 << 8) + arg1 + cpu->Y];
			cpu->P.Z = cpu->A == arg1;
			cpu->P.C = cpu->A >= arg1;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_IND_X:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg3 = (mem->data[(arg2 << 8) + arg1 + cpu->X] << 8) + mem->data[(arg2 << 8) + arg1 + cpu->X + 1];
			cpu->P.Z = cpu->A == arg3;
			cpu->P.C = cpu->A >= arg3;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CMP_IND_Y:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg3 = (mem->data[(arg2 << 8) + arg1] << 8) + mem->data[(arg2 << 8) + arg1 + 1];
			arg3 += cpu->Y;
			cpu->P.Z = cpu->A == arg3;
			cpu->P.C = cpu->A >= arg3;
			cpu->P.N = (cpu->A - arg1) < 0;
			break;

		case CPX_IM:
			arg1 = mem->fetch(cycles);
			cpu->P.Z = cpu->X == arg1;
			cpu->P.C = cpu->X >= arg1;
			cpu->P.N = (cpu->X - arg1) < 0;
			break;

		case CPX_ZP:
			arg1 = mem->data[mem->fetch(cycles)];
			cpu->P.Z = cpu->X == arg1;
			cpu->P.C = cpu->X >= arg1;
			cpu->P.N = (cpu->X - arg1) < 0;
			break;

		case CPX_ABS:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg1 = mem->data[(arg2 << 8) + arg1];
			cpu->P.Z = cpu->X == arg1;
			cpu->P.C = cpu->X >= arg1;
			cpu->P.N = (cpu->X - arg1) < 0;
			break;

		case CPY_IM:
			arg1 = mem->fetch(cycles);
			cpu->P.Z = cpu->Y == arg1;
			cpu->P.C = cpu->Y >= arg1;
			cpu->P.N = (cpu->Y - arg1) < 0;
			break;

		case CPY_ZP:
			arg1 = mem->data[mem->fetch(cycles)];
			cpu->P.Z = cpu->Y == arg1;
			cpu->P.C = cpu->Y >= arg1;
			cpu->P.N = (cpu->Y - arg1) < 0;
			break;

		case CPY_ABS:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			arg1 = mem->data[(arg2 << 8) + arg1];
			cpu->P.Z = cpu->Y == arg1;
			cpu->P.C = cpu->Y >= arg1;
			cpu->P.N = (cpu->Y - arg1) < 0;
			break;

		case STA_ABS:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			printf("\n %04x \n", ((arg2 << 8) + arg1));
			mem->data[((arg2 << 8) + arg1)] = cpu->A;
			break;

		case STA_ABS_X:
			arg1 = mem->fetch(cycles);
			arg2 = mem->fetch(cycles);
			mem->data[(arg2 << 8) + arg1 + cpu->X] = cpu->A;
			break;

		case STA_ABS_Y:
			mem->data[(arg2 << 8) + arg1 + cpu->Y] = cpu->A;
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
			printf("Illegal instruction: %02x! PC: %04x\n", instruct, cpu->PC);
			break;
	}
}