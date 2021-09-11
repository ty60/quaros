#include "sys.h"
#include "util.h"


int main(int argc, char **argv) {
    int cfd;
    cfd = open("console", 0);

    if (argc == 1) {
        puts(cfd, "Usage: cat [FILE]...");
        exit(0);
    }

    int i;
    for (i = 1; i < argc; i++) {
        int fd;
        if ((fd = open(argv[i], O_RDONLY)) < 0) {
            print(cfd, "cat: Failed to open file ");
            print(cfd, argv[i]);
            puts(cfd, "");
            exit(0);
        }
        char buf[1024];
        read(fd, buf, sizeof(buf));
        write(cfd, buf, strlen(buf));
        close(fd);
    }
    exit(0);
}
