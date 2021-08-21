#include "sys.h"

#define INTERVAL 100000000
#define CHILD_MSG "Hello from child!\n"

int main(void) {
    int fd = open("console");
    int i, j;
    for (i = 0; i < 10; i++) {
        write(fd, CHILD_MSG, strlen(CHILD_MSG));
        for (j = 0; j < INTERVAL; j++) {
        }
    }
    write(fd, "Bye!\n", 5);
    exit(0);
}
