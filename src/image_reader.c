#include "image_reader.h"
#include <stdio.h>

void read_image(Memory *mem) {
	FILE *file = fopen("rom.bin", "rb");

	fread(&(mem->data[0x8000]), sizeof(mem->data), 1, file);
}