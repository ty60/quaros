#include "sys.h"
#include "util.h"

#define MAX_ARG (8)


int fd;
static int tokenize(char *buf, char **tokens) {
    int num = 0;
    int len = strlen(buf);
    char* lp;
    char* rp;

    tokens[0] = NULL;

    lp = buf;
    while (*lp == ' ')
        lp++;

    rp = lp;
    while (len > (rp - buf)) {
        while (*rp != ' ' && *rp != '\0') {
            rp++;
        }
        *rp++ = '\0';
        tokens[num++] = lp;
        if (num == MAX_ARG) {
            return 0;
        }
        while (*rp == ' ') {
            rp++;
        }
        lp = rp;
    }

    return num;
}


int main(void) {
    fd = open("console", 0);

    while (1) {
        char buf[1024];
        char *argv[MAX_ARG];
        int n, argc;

        print(fd, "$ ");

        // -1 so that rp won't exceed buf while tokenizing
        n = read_line(fd, buf, sizeof(buf) - 1);
        buf[n - 1] = '\0';

        if ((argc = tokenize(buf, argv)) == 0) {
            continue;
        }
        argv[argc] = NULL;

        int pid = fork();
        if (pid == 0) {
            // child process
            execv(argv[0], argv);
        } else {
            // TODO: implement wait in kernel
            int waited_for = _wait();
            print(fd, "waited for pid: ");
            printnum(fd, waited_for);
            puts(fd, "");
        }
    }
    exit(0);
}
