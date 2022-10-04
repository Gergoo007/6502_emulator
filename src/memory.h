#pragma once

#include "main.h"

typedef struct Memory {
	byte data[MAX_MEM];

	void (*init) (void);
	byte (*fetch) (uint32_t, struct Memory*);

	void (*push) (byte value);
	byte (*pop) (void);
} Memory;

extern Memory mem;

void emu_mem_init();
