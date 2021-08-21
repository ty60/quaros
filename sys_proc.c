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
    struct int_regs *child_frame = KSTACK_TOP(tp->kstack) - sizeof(struct int_regs);
    build_int_frame(child_frame, frame->eip);
    // Copy context frame from parent to child kernel stack
    struct context *child_context = (void *)child_frame - sizeof(struct context);
    build_context(child_context);
    tp->context = child_context;

    // Copy callee saved registers
    child_frame->saved_regs = frame->saved_regs; // Just copy all regs
    child_frame->esp = frame->esp;
    // Return pid 0 to child process
    child_frame->saved_regs.eax = 0;

    // map same virtual memory space as parent process.
    // Currently assume that only the first page is mapped [0, PGSIZE).
    // TODO: Do something better. Too dumb.
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
    // TODO: Don't ignore exit code of user process.
    char *path = NULL;
    if (read_syscall_arg((void *)frame->esp, 0, (uint32_t *)&path) < 0) {
        return -1;
    }

    // Clean up stuff.
    // DON'T DESTROY USER MEMORY WHEN EXEC FAILS AND RETURNS TO USER PROC.
    destroy_user_address_space(curr_task->pgdir);
    // Close files
    memset(curr_task->open_files, 0, sizeof(curr_task->open_files));

    struct file *fp = get_file(path);
    if (!fp) {
        panic("register_task: Cannot find file");
    }
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)(fp->data);
    if (load_elf(curr_task, ehdr) < 0) {
        panic("register_task: Cannot load ELF");
    }

    frame->eip = ehdr->e_entry;
    return 0;
}


extern int zombie_exists;


int sys_exit(struct int_regs *frame) {
    if (curr_task == scheduler_task) {
        panic("sys_exit: Exit scheulder");
    }

    zombie_exists = 1;
    curr_task->state = ZOMBIE;
    switch_to(scheduler_task);
    panic("sys_exit: Should not return from scheduler");
    return 0;
}
