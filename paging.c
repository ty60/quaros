#include "memory.h"


__attribute__((__aligned__(PGSIZE)))
pde_t entry_pgdir[PDE_SIZE] = {
    [0] = (0 | PDE_P | PDE_RW | PDE_US | PDE_PS), // [0x0:4MB) -> [0x0:4MB)
    [KERN_BASE >> 22] = (0 | PDE_P | PDE_RW | PDE_US | PDE_PS), // [0xc0000000:0xc0400000)
};
