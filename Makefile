rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CSRC := $(call rwildcard, src, *.c)

COBJ := $(patsubst src/%.c, out/%.c.o, $(CSRC))

kernel: $(COBJ)
	gcc $^ -g -o sim

out/%.c.o: src/%.c
	gcc $(CFLAGS) -g -c $^ -o $@