#include "proc.h"
#include "util.h"
#include "memory.h"
#include "asm.h"
#include "string.h"
#include "io.h"
#include "elf.h"
#include "fs.h"

void ret_to_int_site(void);
void context_switch(struct task_struct *next_task);


struct task_struct *curr_task;

void switch_to(struct task_struct *next_task) {
    // Configure tss
    task_state.ss0 = (KERN_DATA_SEG << 3);
    task_state.esp0 = (uint32_t)next_task->kstack_top;
    task_state.iomb = sizeof(task_state);
    // switch page pable
    lcr3(next_task->pgdir);
    // change task states
    // Do something about it.
    curr_task->state = RUNNABLE;
    next_task->state = RUNNING;

    curr_task = next_task;

    // Switch to next task.
    // i.e.
    // 1. Save current regs on stack (context)
    // 2. Save pointer to context in curr_task->context
    // 3. Set next_task->context as new stack
    // 4. Return to next_task's eip
    context_switch(next_task);

    // When this task is scheduled again later
    // it will `ret` here from another task.
}

extern pde_t *kpgdir;

void init_tasks(void) {
    int i;
    memset(tasks, 0, sizeof(tasks));
    for (i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = UNUSED;
    }
    curr_task = &tasks[0];
    tasks[0].state = RUNNING;
    // context can be NULL since it will be overwritte in context_switch
    tasks[0].context = NULL;
    tasks[0].kstack_top = NULL; // TODO can kstack_top be NULL?
    tasks[0].pgdir = kpgdir;
    tasks[0].pid = -1; // TODO do something about negative pid
}

void *memset_debug(void *s, int c, uint32_t n) {
    uint32_t i;
    for (i = 0; i < (n / sizeof(int)); i++) {
        printnum(i);
        if (i % 16 == 0) {
            puts("");
        } else {
            print(" ");
        }
        *((int *)(s) + i) = c;
    }
    puts("return");
    return s;
}


void switch_success(void) {
    panic("Switch success!!!");
}


int load_elf(struct task_struct *task, Elf32_Ehdr *ehdr) {
    int i;
    Elf32_Phdr *phdr;

    if (!CHECK_ELF(ehdr)) {
        return -1;
    }

    for (i = 0; i < ehdr->e_phnum; i++) {
        phdr = ELF_PHDR(ehdr, i);
        if (phdr->p_type == PT_LOAD) {
            /*
            print("p_type: ");
            printnum(phdr->p_type);
            print("\np_vaddr: ");
            printnum(phdr->p_vaddr);
            print("\np_memsz: ");
            printnum(phdr->p_memsz);
            puts("");
            */
            alloc_map_memory(task->pgdir,
                             phdr->p_vaddr,
                             phdr->p_memsz,
                             PTE_RW | PTE_US);
            memcpy_to_another_space(task->pgdir,
                                    (void *)phdr->p_vaddr,
                                    (const void *)ehdr + phdr->p_offset,
                                    phdr->p_filesz);
        }
    }

    return 0;
}


struct task_struct *alloc_task(const char *path) {
    int i;
    struct task_struct *tp = NULL;
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == UNUSED) {
            tp = tasks + i;
            break;
        }
    }
    if (!tp) {
        puts("No empty task slot");
        return NULL;
    }

    tp->kstack_top = kmalloc();
    if (!tp->kstack_top) {
        puts("Out of memory for kstack");
        return NULL;
    }
    tp->kstack_top += PGSIZE - 4;

    tp->pgdir = map_kernel();

    struct file *fp = get_file(path);
    if (!fp) {
        puts("Cannot find specified file");
        kfree(tp->kstack_top - PGSIZE);
        return NULL;
    }
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)(fp->data);
    if (load_elf(tp, ehdr) < 0) {
        puts("Loading elf failed");
        kfree(tp->kstack_top - PGSIZE);
        return NULL;
    }

    // Setup dummy int_regs frame in order to return from
    // kernel space to user space.
    struct int_regs *int_regs_p;
    int_regs_p = (struct int_regs *)(tp->kstack_top - sizeof(struct int_regs));
    memset(int_regs_p, 0, sizeof(struct int_regs));
    // bits [0, 1] of ds, es, ss encodes priv required to access these segments.
    // Allow user level priv to access, by setting DPL_USER.
    int_regs_p->ds = (USER_DATA_SEG << 3) | DPL_USER;
    int_regs_p->es = (USER_DATA_SEG << 3) | DPL_USER;
    int_regs_p->eip = ehdr->e_entry;
    // bits [0, 1] of cs encodes the CPL (current privilege level).
    // Set to user level privilege.
    int_regs_p->cs = (USER_CODE_SEG << 3) | DPL_USER;
    int_regs_p->eflags = FL_IF;
    int_regs_p->esp = PGSIZE - 4; // user space stack
    int_regs_p->ss = (USER_DATA_SEG << 3) | DPL_USER;

    // Setup dummy context frame in order to return to ret_to_int_site
    // and consume the dummy int_regs frame, after switching to this
    // context for the first time.
    // This dummy context frame is constructed on top of dummy int_regs frame.
    struct context *context_p;
    context_p = (struct context *)(tp->kstack_top
                                   - sizeof(struct int_regs)
                                   - sizeof(struct context));
    memset(context_p, 0, sizeof(struct context));
    context_p->eip = (uint32_t)ret_to_int_site;

    tp->context = context_p;

    switch_to(tp);
    return tp;
}
