#ifndef ELF_H
#define ELF_H

#include "types.h"


typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;


#define EI_NIDENT 16

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define PT_NULL 0
#define PT_LOAD 1

#define CHECK_ELF(ehdr) \
    ((ehdr->e_ident[EI_MAG0] == ELFMAG0) && \
     (ehdr->e_ident[EI_MAG1] == ELFMAG1) && \
     (ehdr->e_ident[EI_MAG2] == ELFMAG2) && \
     (ehdr->e_ident[EI_MAG3] == ELFMAG3))

#define ELF_PHDR(ehdr, pi) \
    ((Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff + pi * ehdr->e_phentsize))


typedef struct {
    uint8_t e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;


typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;


#endif
