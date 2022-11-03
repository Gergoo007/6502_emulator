#!/bin/bash

toolchain/vasm6502 rom/main.s -dotdir -Fbin -o rom/rom.bin
make
./emu
