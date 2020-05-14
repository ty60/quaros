#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#define KERN_BASE 0xc0000000
#define KERN_LINK 0xc0100000
#define KERN_BASE_PHYS 0x00000000
#define KERN_LINK_PHYS 0x00100000
#define VIRT_TO_PHYS(va) (((uint32_t)(va)) - KERN_BASE)
#define PHYS_TO_VIRT(pa) (((uint32_t)(pa)) + KERN_BASE)

typedef uint32_t pde_t;
typedef uint32_t pte_t;

#define PDE_SIZE 1024

#define PGSIZE 0x1000
#define PG_ROUNDDOWN(addr) \
    ((char *)((uint32_t)(addr) & (uint32_t)(~(PGSIZE - 1))))
#define PG_ROUNDUP(addr) \
    ((char *)((uint32_t)(addr + PGSIZE - 1) & (uint32_t)(~(PGSIZE - 1))))


#define PDE_P  (1 << 0)
#define PDE_RW (1 << 1)
#define PDE_US (1 << 2)
#define PDE_PS (1 << 7)

#define PTE_P  PDE_P
#define PTE_RW PDE_RW
#define PTE_US PDE_US


#define NUM_SEGMENTS 5
#define SEG_TYPE_RW (1 << 1)
#define SEG_TYPE_EX (1 << 3)
#define SEG_FLAG_GRAN (1 << 2)

#define NULL_SEG (0)
#define KERN_DATA_SEG (1)
#define KERN_CODE_SEG (2)
#define USER_DATA_SEG (3)
#define USER_CODE_SEG (4)

#define DPL_KERN (0)
#define DPL_USER (3)


struct map_info {
    uint32_t va;
    uint32_t pa;
    uint32_t size;
    uint32_t perm;
};


struct segment_desc {
    uint16_t limit0;
    uint16_t base0;
    uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
    uint16_t limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__ ((packed));
// packed attribute
// https://gcc.gnu.org/onlinedocs/gcc-3.3/gcc/Type-Attributes.html
// > specified that the minimum required memory be used to represent the type.

struct gdt_desc {
    uint16_t size;
    uint16_t offset0;
    uint16_t offset1;
} __attribute__((packed));


void kfree(void *ptr);
void *kmalloc(void);
void register_free_mem(char *start, char *end);
void init_kernel_memory(void);
void init_segmentation(void);
void zero_out_bss(void);


#endif
