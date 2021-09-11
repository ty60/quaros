#include "sys.h"
#include "util.h"


int main(void) {
    int cfd = open("console", 0);
    int rfd = open("/", 0);

    char buf[1024];
    if (read(rfd, buf, sizeof(buf)) < 0) {
        puts(cfd, "ls: Error");
        exit(1);
    }

    print(cfd, buf);
    exit(0);
}
