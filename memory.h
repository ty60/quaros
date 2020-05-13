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


void kfree(void *ptr);
void *kmalloc(void);
void register_free_mem(char *start, char *end);
void init_kernel_memory(void);


#endif
