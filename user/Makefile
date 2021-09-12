CC := gcc
LD := ld

MAKEFLAGS += --no-builtin-rules

# -O (optimization) option is required to inline functions
CFLAGS := -m32 -Wall -Wextra -fno-stack-protector -fno-pic -fno-builtin -fno-strict-aliasing -MD -fno-omit-frame-pointer -fno-pie -no-pie -nostdinc -nostdlib -masm=intel -Wl,--build-id=none -O

LDFLAGS := -m elf_i386 --build-id=none -N -T ./user.ld

progs := $(patsubst %.c, %, $(filter-out util.c, $(wildcard *.c)))
ulib_obj := sys.o util.o

# ulib_obj has to be in the front.
.PHONY: all
all: $(ulib_obj) $(progs) 

%: %.o
	mkdir -p bin
	$(LD) $(LDFLAGS) -o bin/$@ $@.o $(ulib_obj)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

sys.o: sys.asm
	nasm -f elf32 $<

.PHONY: clean
clean:
	rm -f bin/* *.o

-include *.d
