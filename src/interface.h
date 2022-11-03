#pragma once

#include "cpu.h"
int open_gui(int argc, char* argv[], CPU* cpu, Memory* mem);
void on_step();
//void parse_ram(uint8_t mem[MAX_MEM]);