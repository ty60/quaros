#ifndef SYS_H
#define SYS_H

typedef unsigned int uint32_t;
typedef uint32_t size_t;

int open(const char *path, int flags);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int fork(void);
int exec(const char *path);
int exit(int status);

#endif
