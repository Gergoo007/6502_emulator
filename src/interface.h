#pragma once

#include "cpu.h"
int open_gui(int argc, char* argv[], CPU* cpu, Memory* mem);

void parse_ram(char result[(0x8000 * 55) + 1], uint8_t mem[MAX_MEM]);