while true; do
	vasm6502 rom/main.s -dotdir -Fbin -o rom/rom.bin
	make
	./emu
done
