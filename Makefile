rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CSRC := $(call rwildcard, src, *.c)

COBJ := $(patsubst src/%.c, out/%.c.o, $(CSRC)) out/gresource.c.o

LIBS := $(shell pkg-config --libs gtk4) -lepoxy 
CFLAGS := $(shell pkg-config --cflags gtk4)

emulator: $(COBJ)
	gcc $^ $(LIBS) -g -o emu

out/%.c.o: src/%.c
	gcc $(CFLAGS)  -g -c $^ -o $@

out/gresource.c.o: gresources/gresource.c
	gcc $(CFLAGS)  -g -c $^ -o $@
