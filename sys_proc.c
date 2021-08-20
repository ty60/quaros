#include "syscall.h"
#include "types.h"
#include "memory.h"
#include "proc.h"
#include "fs.h"
#include "string.h"
#include "io.h"
#include "util.h"
#include "sys_proc.h"


int sys_fork(struct int_regs *frame) {
    struct task_struct *tp = alloc_task();

    // Copy interrupt frame from parent to child kernel stack
    struct int_regs *child_frame = tp->kstack_top - sizeof(struct int_regs);
    build_int_frame(child_frame, frame->eip);
    // *child_frame = *frame;
    // Copy context frame from parent to child kernel stack
    struct context *child_context = (void *)child_frame - sizeof(struct context);
    build_context(child_context);
    // *child_context = *curr_task->context;
    tp->context = child_context;

    // Copy callee saved registers
    child_frame->saved_regs = frame->saved_regs; // Just copy all regs
    child_frame->esp = frame->esp;
    // Return pid 0 to child process
    child_frame->saved_regs.eax = 0;

    // map same virtual memory space as parent process.
    // Currently assume that only the first page is mapped [0, PGSIZE).
    // TODO: Do something better. Too dumb.
    // TODO: I think the memcpy_to_another_space is broken.
    // It seems that kernel space is returning address of pid == 2,
    // but the program seems to be not loaded at that part.
    alloc_map_memory(tp->pgdir, 0, PGSIZE, PTE_RW | PTE_US);
    // Copy current task's user space memory ([0, PGSIZE)) to new task.
    memcpy_to_another_space(tp->pgdir, 0, 0, PGSIZE);

    // copy opened files
    memcpy(tp->open_files, curr_task->open_files, sizeof(curr_task->open_files));

    // Change state
    tp->state = RUNNABLE;

    // return child pid to parent
    return tp->pid;
}


int sys_exec(struct int_regs *frame) {
}
