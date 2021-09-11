#include "sys.h"
#include "util.h"

#define INTERVAL 100000000

#define PARENT_MSG "Hello from parent!\n"
#define EXEC_MSG "Execing child process\n"
#define ERR_MSG "exec() failed\n"
#define INIT_MSG "init calling sh\n"


int main(void) {
    int fd = open("console", 0);
    // int pid;
    puts(fd, INIT_MSG);
    execv("sh", NULL);
    /*
    pid = fork();
    if (pid > 0) {
        int wfd = open("test.txt", O_RDWR);
        close(wfd);
        exit(1);
    } else {
        char *argv[] = { "hogehoge", "fugafugafuga", NULL };
        execv("echosh", argv);
        puts(fd, "exec() failed");
        while (1) {
        }
    }
    */
    return 0;
}
