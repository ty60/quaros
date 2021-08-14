CC := gcc
LD := ld

# -O (optimization) option is required to inline functions
CFLAGS := -m32 -Wall -Wextra -fno-stack-protector -fno-pic -fno-builtin -fno-strict-aliasing -MD -fno-omit-frame-pointer -fno-pie -no-pie -nostdinc -nostdlib -masm=intel -Wl,--build-id=none -O

LDFLAGS := -m elf_i386 --build-id=none

progs := init

all: $(progs)

init: init.o
	$(LD) $(LDFLAGS) -N -T ./user.ld -o bin/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(progs) *.o


-include *.d
