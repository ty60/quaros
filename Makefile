CC := gcc
LD := ld
QEMU := qemu-system-i386

# -O (optimization) option is required to inline functions
CFLAGS := -m32 -Wall -Wextra -fno-stack-protector -fno-pic -fno-builtin -fno-strict-aliasing -MD -fno-omit-frame-pointer -fno-pie -no-pie -nostdinc -nostdlib -masm=intel -Wl,--build-id=none -O
LDFLAGS := -m elf_i386


image := quaros.img

boot_objs := bootasm.o bootc.o
boot_ld := bootloader.ld
boot_elf := bootloader.elf

kernel_objs := start.o main.o memory.o util.o segment.o
kernel_ld := kernel.ld
kernel_elf := kernel.elf


$(image): $(boot_elf) $(kernel_elf)
	objcopy -O binary -S -j .text -j .sign $(boot_elf) $(image)
	cat $(kernel_elf) >> $(image)

$(boot_elf): $(boot_objs)
	$(LD) $(LDFLAGS) -T $(boot_ld) -o $(boot_elf) $^

$(kernel_elf): $(kernel_objs)
	$(CC) $(CFLAGS) -T $(kernel_ld) -o $@ $^

bootasm.o: bootasm.asm
	nasm -f elf32 $<

start.o: start.asm
	nasm -f elf32 $<

segment.o: segment.asm
	nasm -f elf32 $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(image)
	$(QEMU) -drive 'file=$(image),format=raw'

debug: $(image)
	$(QEMU) -drive 'file=$(image),format=raw' -monitor stdio -s -S

gdb:
	gdb -q -x cmd.gdb

clean:
	rm -f $(image) $(boot_elf) $(boot_objs) $(kernel_objs) $(kernel_elf)

-include *.d
