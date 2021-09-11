#include "syscall.h"
#include "types.h"
#include "memory.h"
#include "proc.h"
#include "fs.h"
#include "string.h"
#include "io.h"
#include "util.h"
#include "sys_proc.h"
#include "asm.h"


extern struct task_struct *scheduler_task;


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
    // Reuse load_elf() or something.
    // This function doesn't assume that only the first page is mapped.
    alloc_map_memory(tp->pgdir, 0, PGSIZE, PTE_RW | PTE_US);
    // Copy current task's user space memory ([0, PGSIZE)) to new task.
    memcpy_to_another_space(tp->pgdir, 0, 0, PGSIZE);

    // copy opened files
    memcpy(tp->open_files, curr_task->open_files, sizeof(curr_task->open_files));

    // Change state
    tp->state = RUNNABLE;

    // Set parent
    tp->parent = curr_task;

    // return child pid to parent
    return tp->pid;
}


static int prepare_ustack(pde_t *pgdir, uint32_t *espp, const char **argv) {
    char *argv_ps[MAX_ARGC];
    int argc = 0;
    void *esp = (void *)*espp;

    if (!argv) {
        // No arguments
        memcpy_to_another_space(pgdir, (int *)esp, &argc, sizeof(argc));
        *espp = (uint32_t)esp - 4;
        return 0;
    }

    while (argc < MAX_ARGC) {
        const char *arg;
        if (read_user_mem((void *)argv, (uint32_t *)&arg) < 0) {
            return -1;
        }
        if (!arg) {
            break;
        }
        int len = strlen(arg);
        if (len > (PGSIZE / 3) / MAX_ARGC) {
            break;
        }
        esp -= (len + 1);
        memcpy_to_another_space(pgdir, esp, *argv++, len + 1); // +1 to copy NULL byte
        argv_ps[argc++] = (char *)esp;
    }

    esp -= 4; // So pointer to argvs won't overlap with argv on stack
    esp = (void *)((uint32_t)esp & ~(4 - 1)); // Round down to 0x4 alignment
    int i;
    for (i = argc - 1; i >= 0; i--) {
        memcpy_to_another_space(pgdir, (char **)(esp), &argv_ps[i], sizeof(argv_ps[i]));
        esp -= 4;
    }

    char **uargv = esp + 4;
    memcpy_to_another_space(pgdir, (char **)(esp), &uargv, sizeof(uargv));
    esp -= 4;

    memcpy_to_another_space(pgdir, (int *)esp, &argc, sizeof(argc));
    *espp = (uint32_t)esp - 4;
    return 0;
}


int sys_execv(struct int_regs *frame) {
    char *path = NULL;
    if (read_syscall_arg((void *)frame->esp, 0, (uint32_t *)&path) < 0) {
        return -1;
    }
    const char **argv = NULL;
    if (read_syscall_arg((void *)frame->esp, 1, (uint32_t *)&argv) < 0) {
        return -1;
    }

    pde_t *old_pgdir = curr_task->pgdir;
    curr_task->pgdir = map_kernel();

    struct file *fp = get_file(path);
    if (!fp) {
        destroy_address_space(curr_task->pgdir);
        curr_task->pgdir = old_pgdir;
        return -1;
    }
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)(fp->data);
    if (load_elf(curr_task, ehdr) < 0) {
        destroy_address_space(curr_task->pgdir);
        curr_task->pgdir = old_pgdir;
        return -1;
    }

    frame->eip = ehdr->e_entry;
    frame->esp = PGSIZE - 8;
    if (prepare_ustack(curr_task->pgdir, &frame->esp, argv) < 0) {
        destroy_address_space(curr_task->pgdir);
        curr_task->pgdir = old_pgdir;
        return -1;
    }

    memset(curr_task->open_files, 0, sizeof(curr_task->open_files));
    destroy_address_space(old_pgdir);
    lcr3(curr_task->pgdir);
    return 0;
}


extern int zombie_exists;

int sys_exit(struct int_regs *frame) {
    // TODO: Don't ignore exit code of user process.
    if (curr_task == scheduler_task) {
        panic("sys_exit: Exit scheulder");
    }

    zombie_exists = 1;
    curr_task->state = ZOMBIE;
    wakeup_parent();
    switch_to(scheduler_task);
    panic("sys_exit: Should not return from scheduler");
    return 0;
}


int sys_wait(struct int_regs *frame) {
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].parent != curr_task) {
            continue;
        }
        if (tasks[i].state == ZOMBIE) {
            return tasks[i].pid;
        } else {
            int pid = tasks[i].pid;
            sleep();
            return pid;
        }
    }
    return -1;
}
