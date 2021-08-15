#include "syscall.h"
#include "types.h"
#include "memory.h"
#include "proc.h"
#include "fs.h"
#include "string.h"
#include "io.h"
#include "util.h"


typedef int (*handler_t)(struct int_regs *int_frame);

extern struct task_struct *curr_task;


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

extern struct file console_file;

int sys_open(struct int_regs *int_frame) {
    uint32_t user_esp = int_frame->esp;
    char *path = NULL;
    if (read_syscall_arg((void *)user_esp, 0, (uint32_t *)&path) < 0) {
        return -1;
    }

    int fd = 0;
    for (fd = 0; fd < MAX_OPEN_FILES; fd++) {
        if (!(curr_task->open_files[fd])) {
            // found empty slot
            break;
        }
    }

    if (strcmp("console", path)) {
        curr_task->open_files[fd] = &console_file;
    } else if (strcmp("/", path)) {
        // TODO: List files
        return -1;
    } else {
        // Regular files
        struct file *fp;
        if (!(fp = get_file(path))) {
            return -1;
        }
        curr_task->open_files[fd] = fp;
    }
    return fd;
}


int sys_read(struct int_regs *int_frame) {
    uint32_t user_esp = int_frame->esp;
    int fd = 0;
    char *buf = NULL;
    size_t count = 0;
    if (read_syscall_arg((void *)user_esp, 0, (uint32_t *)&fd) < 0) {
        return -1;
    }
    if (read_syscall_arg((void *)user_esp, 1, (uint32_t *)&buf) < 0) {
        return -1;
    }
    if (read_syscall_arg((void *)user_esp, 2, &count) < 0) {
        return -1;
    }
    if (!curr_task->open_files[fd]) {
        return -1;
    } else if (curr_task->open_files[fd]->type == FT_DEV) {
        // TODO: Implement reading from console
        panic("sys_read: Read from console currently not supported");
        return -1;
    } else if (curr_task->open_files[fd]->type == FT_REGULAR) {
        return read_file(curr_task->open_files[fd], buf, count);
    }
    return -1;
}

int sys_write(struct int_regs *int_frame) {
    uint32_t user_esp = int_frame->esp;
    int fd;
    char *buf;
    size_t count;
    if (read_syscall_arg((void *)user_esp, 0, (uint32_t *)&fd) < 0) {
        return -1;
    }
    if (read_syscall_arg((void *)user_esp, 1, (uint32_t *)&buf) < 0) {
        return -1;
    }
    if (read_syscall_arg((void *)user_esp, 2, &count) < 0) {
        return -1;
    }
    if (!curr_task->open_files[fd]) {
        return -1;
    } else if (curr_task->open_files[fd]->type == FT_DEV) {
        return printn(buf, count);
    } else if (curr_task->open_files[fd]->type == FT_REGULAR) {
        // TODO: Implement writing to regular file
        panic("sys_write: Write to regular file currently not supported");
        return -1;
    }
    return -1;
}

handler_t handlers[] = {
    [SYS_read] = sys_read,
    [SYS_write] = sys_write,
    [SYS_open] = sys_open,
};

int handle_syscall(struct int_regs *int_frame) {
    int num = int_frame->saved_regs.eax;
    if (num >= NUM_SYSCALL) {
        // syscall number that doesn't exist
        return 0;
    }
    return handlers[num](int_frame);
}
