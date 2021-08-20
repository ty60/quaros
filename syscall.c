#include "syscall.h"
#include "types.h"
#include "memory.h"
#include "proc.h"
#include "fs.h"
#include "string.h"
#include "io.h"
#include "util.h"
#include "sys_file.h"
#include "sys_proc.h"


typedef int (*handler_t)(struct int_regs *int_frame);


int read_user_mem(void *addr, uint32_t *data) {
    if ((uint32_t)addr + sizeof(uint32_t) >= KERN_BASE) {
        // Syscall should not access kernel memory
        // on behalf of user level program.
        return -1;
    }
    *data = *(uint32_t *)addr;
    return 0;
}


int read_syscall_arg(void *esp, int argnum, uint32_t *data) {
    return read_user_mem(esp + 4 + argnum * 4, data);
}


handler_t handlers[] = {
    [SYS_read] = sys_read,
    [SYS_write] = sys_write,
    [SYS_open] = sys_open,
    [SYS_fork] = sys_fork,
    [SYS_exec] = sys_exec,
};

int handle_syscall(struct int_regs *int_frame) {
    int num = int_frame->saved_regs.eax;
    if (num >= NUM_SYSCALL) {
        // syscall number that doesn't exist
        return 0;
    }
    return handlers[num](int_frame);
}
