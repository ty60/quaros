#include "sys.h"

#define INTERVAL 100000000

#define INIT_MSG "init process\n"
#define PARENT_MSG "Hello from parent!\n"
#define EXEC_MSG "Execing child process\n"
#define ERR_MSG "exec() failed\n"


int main(void) {
    int fd = open("console");
    int i;

    write(fd, INIT_MSG, strlen(INIT_MSG));

    int pid = 0;
    pid = fork();
    if (pid > 0) {
        while (1) {
            write(fd, PARENT_MSG, strlen(PARENT_MSG));
            for (i = 0; i < INTERVAL; i++) {
            }
        }
    } else {
        write(fd, EXEC_MSG, strlen(EXEC_MSG));
        exec("child");
        write(fd, ERR_MSG, strlen(ERR_MSG));
        while (1) {
        }
    }
    return 0;
}
