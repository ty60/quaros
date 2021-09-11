#include "sys.h"
#include "util.h"


int main(void) {
    int fd = open("console", 0);
    puts(fd, "Hello from child process!!");
    exit(0);
}
