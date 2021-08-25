#include "sys.h"
#include "util.h"

#define INTERVAL 100000000
#define CHILD_MSG 

int main(void) {
    int fd = open("console", 0);
    int i, j;
    puts(fd, "Hello from child!");
    for (i = 0; i < 10; i++) {
        print(fd, "child: ");
        printnum(fd, i);
        puts(fd, "");
        for (j = 0; j < INTERVAL; j++) {
        }
    }
    puts(fd, "Bye!");
    exit(0);
}
