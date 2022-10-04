#pragma once

#include <stdint.h>
#include <string.h>

#define MAX_MEM 1024 * 64

// 1 byte = 8 bits (max 0xff, 256)
typedef uint8_t byte;

// 2 bytes = 16 bits (max 0xffff, 65536)
typedef uint16_t word;
