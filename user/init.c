#include "sys.h"

#define INTERVAL 10000000

#define PARENT_MSG "Hello from parent!\n"
#define CHILD_MSG "Hello from child!\n"


int main(void) {
    int fd = open("console");
    int i;

    int pid = 0;
    pid = fork();
    if (pid > 0) {
        while (1) {
            write(fd, PARENT_MSG, strlen(PARENT_MSG));
            for (i = 0; i < INTERVAL; i++) {
            }
        }
    } else {
        while (1) {
            write(fd, CHILD_MSG, strlen(CHILD_MSG));
            for (i = 0; i < INTERVAL; i++) {
            }
        }
    }
    return 0;
}
