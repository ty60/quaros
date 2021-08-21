#include "sys.h"

#define INTERVAL 100000000
#define CHILD_MSG "Hello from child!\n"

int main(void) {
    int fd = open("console");
    int i;
    while (1) {
        write(fd, CHILD_MSG, strlen(CHILD_MSG));
        for (i = 0; i < INTERVAL; i++) {
        }
    }
    return 0;
}
