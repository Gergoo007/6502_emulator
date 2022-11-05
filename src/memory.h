#pragma once

#include "main.h"

typedef struct Memory {
	byte data[MAX_MEM];

	void (*init) (void);
	byte (*fetch) (uint32_t);
	word (*fetchw) ();

	void (*push) (byte value, uint32_t cycles);
	byte (*pop) (uint32_t cycles);
} Memory;

extern Memory mem_glob;

void emu_mem_init();
