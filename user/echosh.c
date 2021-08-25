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


int main(void) {
    int fd = open("console", 0);
    char buf[1024];
    while (1) {
        int n;
        print(fd, "$ ");
        n = read_line(fd, buf, sizeof(buf));
        buf[n] = '\0';
        print(fd, buf);
    }
}
