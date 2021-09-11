/*
 * Shell like program that simply echos back input data.
 */

#include "sys.h"
#include "util.h"


int main(int argc, char **argv) {
    int fd = open("console", 0);
    char buf[1024];
    puts(fd, argv[0]);
    print(fd, "argc: "); printnum(fd, argc); puts(fd, "");
    puts(fd, "Before I loop let me show you argvs");
    if (argc == 0) {
        puts(fd, "No arguments...");
    } else {
        int i;
        for (i = 0; i < argc; i++) {
            puts(fd, argv[i]);
        }
    }
    while (1) {
        int n;
        print(fd, "$ ");
        n = read_line(fd, buf, sizeof(buf));
        buf[n] = '\0';
        print(fd, buf);
    }
}
