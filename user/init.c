#include "sys.h"

#define MSG "Hello from user world!\n"


int main(void) {
    int fd = open("console");
    write(fd, MSG, strlen(MSG));
    while (1) ;
    return 0;
}
