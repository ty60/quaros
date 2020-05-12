#include "types.h"
#include "asm.h"
#include "elf.h"


#define KERN_BASE_PHYS 0x00100000


#define SECT_SIZE 512

#define ATA_RDY (1 << 6)
#define ATA_BSY (1 << 7)


typedef void (*func_t)(void);


#define CHECK_ATA_READY(status) \
    (((status & 0xff) & (ATA_RDY | ATA_BSY)) == ATA_RDY)


char *read_sect(char *dst, uint32_t lba) {
    while (!CHECK_ATA_READY(inb(0x1f7)))
        ;

    outb(0x1f6, 0xe0 | ((lba >> 24) & 0x0f));
    outb(0x1f2, 1);
    outb(0x1f3, (uint8_t)lba);
    outb(0x1f4, (uint8_t)((lba >> 8) & 0xff));
    outb(0x1f5, (uint8_t)((lba >> 16) & 0xff));
    outb(0x1f7, 0x20);

    while (!CHECK_ATA_READY(inb(0x1f7)))
        ;

    insd(0x1f0, (uint8_t *)dst, SECT_SIZE / 4);
    return dst + SECT_SIZE;
}


void read_offset(char *dst, uint32_t offset, uint32_t size) {
    uint32_t lba = offset / SECT_SIZE;
    char *p = dst - (offset % SECT_SIZE);
    while (p < dst + size) {
        p = read_sect(p, lba++);
    }
}


void bootc(void) {
    int i;
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;

    ehdr = (Elf32_Ehdr *)KERN_BASE_PHYS;
    read_offset((char *)ehdr, SECT_SIZE, SECT_SIZE);

    for (i = 0; i < ehdr->e_phnum; i++) {
        phdr = ELF_PHDR(ehdr, i);
        if (phdr->p_type & PT_LOAD) {
            read_offset((char *)phdr->p_paddr, SECT_SIZE + phdr->p_offset, phdr->p_filesz);
        }
    }

    func_t entry = (func_t)ehdr->e_entry;
    entry();
}
