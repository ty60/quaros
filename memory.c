#include "util.h"
#include "types.h"
#include "memory.h"
#include "asm.h"


__attribute__((__aligned__(PGSIZE)))
pde_t entry_pgdir[PDE_SIZE] = {
    [0] = (0 | PDE_P | PDE_RW | PDE_US | PDE_PS), // [0x0:4MB) -> [0x0:4MB)
    [KERN_LINK >> 22] = (0 | PDE_P | PDE_RW | PDE_US | PDE_PS), // [0xc0000000:0xc0400000)
};


typedef struct _free_node {
    struct _free_node *next;
} free_node;


free_node *free_pages = NULL;

void kfree(void *ptr) {
    if (!free_pages) {
        ((free_node *)(ptr))->next = NULL;
    } else {
        ((free_node *)(ptr))->next = free_pages;
    }
    free_pages = ptr;
}


void *kmalloc(void) {
    void *ret = free_pages;
    if (!ret) {
        panic("kmalloc: no memory to allocate");
    }
    free_pages = free_pages->next;
    return ret;
}


void register_free_mem(char *start, char *end) {
    char *now;
    for (now = PG_ROUNDUP(start); now < PG_ROUNDDOWN(end); now += PGSIZE) {
        kfree(now);
    }
}


pde_t *walk_pgdir(pde_t *pgdir, uint32_t vaddr) {
    pde_t *pde_p;
    pte_t *pgtab;
    pde_p = pgdir + ((vaddr >> 22) & 0x3ff);
    if (!(*pde_p & PDE_P)) {
        panic("walk_pgdir: unmapped memory");
    }
    pgtab = (pte_t *)PHYS_TO_VIRT(*pde_p & (~0xfff));
    return pgtab + ((vaddr >> 12) & 0x3ff);
}


static void map_page(pde_t *pgdir, uint32_t vaddr, uint32_t paddr, uint32_t perm) {
    pde_t *pde_p;
    pte_t *pgtab;
    pte_t *pte_p;
    pde_p = pgdir + ((vaddr >> 22) & 0x3ff);
    if (!(*pde_p & PDE_P)) {
        pgtab = kmalloc();
        memset(pgtab, 0, PGSIZE);
        *pde_p = (uint32_t)VIRT_TO_PHYS(pgtab) | PDE_P | PDE_RW | PDE_US;
    } else {
        pgtab = (pte_t *)PHYS_TO_VIRT(*pde_p & (~0xfff));
    }
    pte_p = pgtab + ((vaddr >> 12) & 0x3ff);
    *pte_p = paddr | PTE_P | perm;
}


void map_memory(pde_t *pgdir, uint32_t vaddr, uint32_t paddr, uint32_t size, uint32_t perm) {
    uint32_t i;
    uint32_t num_pages;
    if (size == 0) {
        panic("map_memory: map 0 bytes");
        return;
    } else {
        num_pages = size / PGSIZE;
    }
    for (i = 0; i < num_pages; i++) {
        map_page(pgdir, vaddr + i * PGSIZE, paddr + i * PGSIZE, perm);
    }
}


// It is more secure to map the kernel more fine grained
// with detialed permission management.
struct map_info kernel_map[] = {
    {0, 0, (1 << 20) * 4, PTE_RW}, // Identity map for convenience [0x0:4MB) -> [0x0:4MB)
    {KERN_BASE, KERN_BASE_PHYS, 0 - KERN_BASE, PTE_RW}, // Map kernel
};


pde_t *map_kernel(void) {
    uint32_t i;
    pde_t *pgdir;
    pgdir = kmalloc();
    memset(pgdir, 0, PGSIZE);
    for (i = 0; i < (sizeof(kernel_map) / sizeof(kernel_map[0])); i++) {
        map_memory(pgdir,
                   kernel_map[i].va,
                   kernel_map[i].pa,
                   kernel_map[i].size,
                   kernel_map[i].perm);
    }
    return pgdir;
}


pde_t *kpgdir = NULL;
void init_kernel_memory(void) {
    kpgdir = map_kernel();
    lcr3(kpgdir);
}


struct segment_desc gdt[NUM_SEGMENTS];

void set_segment_desc(struct segment_desc *desc, uint32_t base, uint32_t limit, uint8_t type, uint32_t dpl) {
    desc->limit0 = limit & 0xffff;
    desc->base0 = base & 0xffff;
    desc->base1 = (base >> 16) & 0xff;
    desc->type = type;
    desc->s = 1;
    desc->dpl = (dpl & 0x3);
    desc->p = 1;
    desc->limit = (limit >> 16) & 0xff;
    desc->avl = 0;
    desc->l = 0;
    desc->d = 1;
    desc->g = 1;
}


struct gdt_desc gdtr;
extern void reload_segment_regs(void);

void init_segmentation(void) {
    memset(&gdt[NULL_SEG], 0, sizeof(struct segment_desc));
    set_segment_desc(&gdt[KERN_DATA_SEG],
                     0x0,
                     0xfffff,
                     SEG_TYPE_RW,
                     DPL_KERN);
    set_segment_desc(&gdt[KERN_CODE_SEG],
                     0x0,
                     0xfffff,
                     // (SEG_TYPE_RW | SEG_TYPE_EX),
                     0 | 2 | 8,
                     DPL_KERN);
    set_segment_desc(&gdt[USER_DATA_SEG],
                     0x0,
                     0xfffff,
                     SEG_TYPE_RW,
                     DPL_USER);
    set_segment_desc(&gdt[USER_CODE_SEG],
                     0x0,
                     0xfffff,
                     SEG_TYPE_RW | SEG_TYPE_EX,
                     DPL_USER);

    gdtr.size = sizeof(gdt) - 1;
    gdtr.offset0 = (uint32_t)gdt & 0xffff;
    gdtr.offset1 = ((uint32_t)gdt >> 16) & 0xffff;
    lgdt(&gdtr);

    reload_segment_regs();
}
