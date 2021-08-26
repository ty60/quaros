#include "syscall.h"
#include "types.h"
#include "memory.h"
#include "proc.h"
#include "fs.h"
#include "string.h"
#include "io.h"
#include "util.h"
#include "sys_file.h"

extern struct file console_file;

int sys_open(struct int_regs *int_frame) {
    uint32_t user_esp = int_frame->esp;
    char *path = NULL;
    if (read_syscall_arg((void *)user_esp, 0, (uint32_t *)&path) < 0) {
        return -1;
    }
    int flags;
    if (read_syscall_arg((void *)user_esp, 1, (uint32_t *)&flags) < 0) {
        return -1;
    }

    int fd = 0;
    for (fd = 0; fd < MAX_OPEN_FILES; fd++) {
        if (!(curr_task->open_files[fd])) {
            // found empty slot
            break;
        }
    }

    struct file *fp = NULL;
    if (strcmp("console", path) == 0) {
        curr_task->open_files[fd] = &console_file;
    } else if (strcmp("/", path) == 0) {
        // TODO: List files
        return -1;
    } else if (flags == O_RDONLY) { // Read regular file
        if (!(fp = get_file(path))) {
            return -1;
        }
    } else if (flags == O_WRONLY || flags == O_RDWR) { // Write regular file
        if ((fp = get_file(path))) {
            // Delete data if file already exists
            memset(fp->data, 0, sizeof(MAX_FILE_SIZE));
        } else if (!(fp = alloc_file(path))) {
            return -1;
        }
    } else {
        return -1;
    }
    if (fp) {
        curr_task->open_files[fd] = fp;
        fp->pos = 0;
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
    }

    if (curr_task->open_files[fd]->type == FT_DEV) {
        return read_console(buf, count);
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
    }

    if (curr_task->open_files[fd]->type == FT_DEV) {
        return printn(buf, count);
    } else if (curr_task->open_files[fd]->type == FT_REGULAR) {
        write_file(curr_task->open_files[fd], buf, count);
    }
    return -1;
}


int sys_close(struct int_regs *int_frame) {
    uint32_t user_esp = int_frame->esp;
    int fd;
    if (read_syscall_arg((void *)user_esp, 0, (uint32_t *)&fd) < 0) {
        return -1;
    }
    struct file *fp = curr_task->open_files[fd];
    if (!fp)
        return -1;
    if (!fp->in_use)
        return -1;
    curr_task->open_files[fd] = NULL;
    return 0;
}
