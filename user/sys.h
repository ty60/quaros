#ifndef SYS_H
#define SYS_H

typedef unsigned int uint32_t;
typedef uint32_t size_t;

int open(const char *path, int flags);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int close(int fd);
int fork(void);
int execv(const char *path, char *const argv[]);
int exit(int status);
int _wait(void);

#endif
