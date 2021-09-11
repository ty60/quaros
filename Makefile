CC := gcc
LD := ld
QEMU := qemu-system-i386

# -O (optimization) option is required to inline functions
CFLAGS := -m32 -Wall -Wextra -fno-stack-protector -fno-pic -fno-builtin -fno-strict-aliasing -MD -fno-omit-frame-pointer -fno-pie -no-pie -nostdinc -nostdlib -masm=intel -Wl,--build-id=none -O -ggdb
LDFLAGS := -m elf_i386


image := quaros.img

boot_objs := bootasm.o bootc.o
boot_ld := bootloader.ld
boot_elf := bootloader.elf

kernel_objs := start.o main.o memory.o util.o segment.o lapic.o uart.o io.o string.o ioapic.o int_common.o context_switch.o vectors.o interrupt.o fs.o proc.o syscall.o sys_file.o sys_proc.o
kernel_ld := kernel.ld
kernel_elf := kernel.elf

user_files := $(wildcard user/*.c) $(wildcard user/*.asm) $(wildcard user/root/*)
fs := fs.elf

all: $(image)

$(image): $(boot_elf) $(kernel_elf)
	objcopy -O binary -S -j .text -j .sign $(boot_elf) $(image)
	cat $(kernel_elf) >> $(image)

$(boot_elf): $(boot_objs)
	$(LD) $(LDFLAGS) -T $(boot_ld) -o $(boot_elf) $^

$(kernel_elf): $(kernel_objs) $(fs)
	$(CC) $(CFLAGS) -T $(kernel_ld) -o $@ $^

$(fs): $(user_files)
	make -C ./user
	ar -r fs.a ./user/bin/* ./user/root/*
	objcopy -I binary -O elf32-i386 -B i386 --rename-section .data=.fsar fs.a $@

bootasm.o: bootasm.asm
	nasm -f elf32 $<

start.o: start.asm
	nasm -f elf32 $<

segment.o: segment.asm
	nasm -f elf32 $<

vectors.o: gen_vectors.py
	python3 gen_vectors.py
	nasm -f elf32 vectors.asm

int_common.o: int_common.asm
	nasm -f elf32 int_common.asm

context_switch.o: context_switch.asm
	nasm -f elf32 context_switch.asm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(image)
	$(QEMU) -drive 'file=$(image),format=raw' -serial mon:stdio -nographic

debug: $(image)
	$(QEMU) -drive 'file=$(image),format=raw' -serial mon:stdio -s -S -monitor telnet::1235,server,nowait -nographic

monitor:
	telnet localhost 1235

gdb:
	gdb -q -x cmd.gdb

clean:
	rm -f $(image) $(boot_elf) $(boot_objs) $(kernel_objs) $(kernel_elf) vectors.asm $(fs) fs.a
	make -C ./user clean

-include *.d
