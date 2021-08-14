#include "proc.h"
#include "util.h"
#include "memory.h"
#include "asm.h"
#include "string.h"
#include "io.h"

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
    // curr_task->state = RUNNABLE;
    next_task->state = RUNNING;

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


void init_tasks(void) {
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = UNUSED;
    }
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


void alloc_task(void) {
    int i;
    struct task_struct *tp = NULL;
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == UNUSED) {
            tp = tasks + i;
            break;
        }
    }
    if (!tp) {
        panic("No empty task slot");
    }

    tp->kstack_top = kmalloc();
    if (!tp->kstack_top) {
        panic("Out of memory for kstack");
    }
    tp->kstack_top += PGSIZE;

    // TODO: This sets up uvm for "init". Make it more general.
    tp->pgdir = setupuvm_init();

    // Setup dummy int_regs frame in order to return from
    // kernel space to user space.
    struct int_regs *int_regs_p;
    int_regs_p = (struct int_regs *)(tp->kstack_top - sizeof(struct int_regs));
    memset(int_regs_p, 0, sizeof(struct int_regs));
    int_regs_p->ds = (USER_DATA_SEG << 3);
    int_regs_p->es = (USER_DATA_SEG << 3);
    int_regs_p->eip = (uint32_t)switch_success; // TODO: Should be entry point of ELF?
    int_regs_p->cs = (USER_CODE_SEG << 3);
    int_regs_p->eflags = FL_IF;
    int_regs_p->esp = PGSIZE; // user space stack
    int_regs_p->ss = (USER_DATA_SEG << 3);

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
}
