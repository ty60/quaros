CC := gcc
LD := ld
QEMU := qemu-system-i386

CFLAGS := -m32 -Wall -Wextra -fno-stack-protector -fno-pic -fno-builtin -fno-strict-aliasing -MD -fno-omit-framepointer -fno-pie -no-pie -nostdinc -nostdlib -masm=intel -Wl,--build-id=none
LDFLAGS := -m elf_i386

image := quaros.img

boot_objs := bootasm.o
boot_ld := bootloader.ld
boot_elf := bootloader.elf


$(image): $(boot_objs)
	$(LD) $(LDFLAGS) -T $(boot_ld) -o $(boot_elf) $^
	objcopy -O binary -S -j .text -j .sign $(boot_elf) $(image)

bootasm.o: bootasm.asm
	nasm -f elf32 $<

run: $(image)
	$(QEMU) -drive 'file=$(image),format=raw'

debug: $(image)
	$(QEMU) -drive 'file=$(image),format=raw' -monitor stdio -s -S

gdb:
	gdb -q -x cmd.gdb

clean:
	rm -f $(image) $(boot_elf) $(boot_objs)

-include *.d
