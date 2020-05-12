#include "types.h"
#include "memory.h"


__attribute__((__aligned__(PGSIZE)))
pde_t entry_pgdir[PDE_SIZE] = {
    [0] = (0 | PDE_P | PDE_RW | PDE_US | PDE_PS), // [0x0:4MB) -> [0x0:4MB)
    [KERN_BASE >> 22] = (0 | PDE_P | PDE_RW | PDE_US | PDE_PS), // [0xc0000000:0xc0400000)
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
    free_pages = free_pages->next;
    return ret;
}


void register_free_mem(char *start, char *end) {
    char *now;
    for (now = PG_ROUNDUP(start); now < PG_ROUNDDOWN(end); now += PGSIZE) {
        kfree(now);
    }
}
