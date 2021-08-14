#include "util.h"
#include "types.h"
#include "memory.h"
#include "asm.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "io.h"


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
    if (((uint32_t) ptr & 0xfff) != 0) {
        panic("kmalloc: page not aligned (kfree)");
    }
    free_pages = ptr;
}


void *kmalloc(void) {
    void *ret = free_pages;
    if (!ret) {
        panic("kmalloc: no memory to allocate");
    }
    if (((uint32_t)ret & 0xfff) != 0) {
        panic("kmalloc: page not aligned (kmalloc)");
    }
    free_pages = free_pages->next;
    return ret;
}


void register_free_mem(char *start, char *end) {
    char *now;
    if (start >= end) {
        panic("Illegal free memory registeration");
    }
    for (now = PG_ROUNDUP(start); now < PG_ROUNDDOWN(end); now += PGSIZE) {
        kfree(now);
    }
}


pte_t *walk_pgdir(pde_t *pgdir, uint32_t vaddr) {
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
    } else if (size % PGSIZE == 0) {
        num_pages = size / PGSIZE;
    } else {
        num_pages = size / PGSIZE + 1;
    }
    for (i = 0; i < num_pages; i++) {
        map_page(pgdir, vaddr + i * PGSIZE, paddr + i * PGSIZE, perm);
    }
}


void alloc_map_memory(pde_t *pgdir, uint32_t vaddr, uint32_t size, uint32_t perm) {
    uint32_t i;
    uint32_t num_pages;
    if (size == 0) {
        panic("alloc_map_memory: alloc and map 0 bytes");
        return;
    } else if (size % PGSIZE == 0) {
        num_pages = size / PGSIZE;
    } else {
        num_pages = size / PGSIZE + 1;
    }
    for (i = 0; i < num_pages; i++) {
        void *pg = kmalloc();
        map_page(pgdir, vaddr + i * PGSIZE, VIRT_TO_PHYS(pg), perm);
    }
}


// It is more secure to map the kernel more fine grained
// with detialed permission management.
struct map_info kernel_map[] = {
    {0, 0, (1 << 20) * 4, PTE_RW}, // Identity map for convenience [0x0:4MB) -> [0x0:4MB)
    {KERN_BASE, KERN_BASE_PHYS, MMDEV_BASE - KERN_BASE, PTE_RW}, // Map kernel
    {MMDEV_BASE, MMDEV_BASE, 0 - MMDEV_BASE, PTE_RW}, // Identity map for memory mapped dev
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


pde_t *setupuvm_task(const char *path) {
    pde_t *pgdir = map_kernel();
    struct file *fp = get_file(path);
    if (!fp) {
        panic("setupuvm_task: No file found");
    }
    // PTE_US so user level process can access it
    alloc_map_memory(pgdir, 0, fp->size, PTE_RW | PTE_US);
    return pgdir;
}


void memcpy_to_another_space(pde_t *pgdir, void *dest, const void *src, size_t n) {
    size_t num_pages;
    if (n % PGSIZE == 0) {
        num_pages = n / PGSIZE;
    } else {
        num_pages = n / PGSIZE + 1;
    }
    size_t i;
    for (i = 0; i < num_pages; i++, n -= PGSIZE) {
        pte_t *pte_p = walk_pgdir(pgdir, (uint32_t)dest + i * PGSIZE);
        uint32_t dest_tgt_va = PHYS_TO_VIRT(*pte_p & (~0xfff));
        memcpy((void *)dest_tgt_va, src, n % PGSIZE);
    }
}


// TODO: Zero out bss at bootloader using section info.
extern char bss_start[];
extern char bss_end[];
void zero_out_bss(void) {
    memset(bss_start, 0, bss_end - bss_start);
}


void set_segment_desc(struct segment_desc *desc, uint32_t base, uint32_t limit, uint8_t type, uint32_t dpl) {
    desc->limit0 = (limit >> 12) & 0xffff;
    desc->base0 = base & 0xffff;
    desc->base1 = (base >> 16) & 0xff;
    desc->type = type;
    desc->s = 1;
    desc->dpl = (dpl & 0x3);
    desc->p = 1;
    desc->limit = (limit >> 28) & 0xff;
    desc->avl = 0;
    desc->l = 0;
    desc->d = 1;
    desc->g = 1;
}


void set_ts_segment_desc(struct segment_desc *desc, uint32_t base, uint32_t limit, uint8_t type, uint32_t dpl) {
    desc->limit0 = limit & 0xffff;
    desc->base0 = base & 0xffff;
    desc->base1 = (base >> 16) & 0xff;
    desc->type = type;
    desc->s = 0; // 0 for system segments
    desc->dpl = (dpl & 0x3);
    desc->p = 1;
    desc->limit = (limit >> 16) & 0xff;
    desc->avl = 0;
    desc->l = 0;
    desc->d = 1;
    desc->g = 0; // byte granualarity instead of 4K granualarity
}


struct gdt_desc gdtr;
extern void reload_segment_regs(void);

void init_segmentation(void) {
    memset(&gdt[NULL_SEG], 0, sizeof(struct segment_desc));
    set_segment_desc(&gdt[KERN_DATA_SEG],
                     0x0,
                     0xffffffff,
                     SEG_TYPE_RW,
                     DPL_KERN);
    set_segment_desc(&gdt[KERN_CODE_SEG],
                     0x0,
                     0xffffffff,
                     (SEG_TYPE_RW | SEG_TYPE_EX),
                     DPL_KERN);
    set_segment_desc(&gdt[USER_DATA_SEG],
                     0x0,
                     0xffffffff,
                     SEG_TYPE_RW,
                     DPL_USER);
    set_segment_desc(&gdt[USER_CODE_SEG],
                     0x0,
                     0xffffffff,
                     SEG_TYPE_RW | SEG_TYPE_EX,
                     DPL_USER);
    set_ts_segment_desc(&gdt[TSS_SEG],
                        (uint32_t)&task_state,
                        sizeof(task_state),
                        SEG_TYPE_T32A,
                        0);
    gdtr.size = sizeof(gdt) - 1;
    gdtr.offset0 = (uint32_t)gdt & 0xffff;
    gdtr.offset1 = ((uint32_t)gdt >> 16) & 0xffff;
    lgdt(&gdtr);

    reload_segment_regs();
}
