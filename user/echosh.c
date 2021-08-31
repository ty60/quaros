/*
 * Shell like program that simply echos back input data.
 */

#include "sys.h"
#include "util.h"


int read_line(int fd, char *buf, int buf_size) {
    int i = 0;
    while (i < buf_size) {
        char ch;
        if (read(fd, &ch, 1) <= 0) {
            // TODO: Implement sleep() in kernel
            continue;
        }
        buf[i++] = ch;
        if (ch == '\n')
            break;
    }
    return i;
}


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
