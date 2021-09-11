#include "sys.h"
#include "util.h"


int main(int argc, char **argv) {
    int cfd = open("console", 0);
    int i;
    for (i = 1; i < argc; i++) {
        print(cfd, argv[i]);
        if (i + 1 != argc) {
            print(cfd, " ");
        }
    }
    puts(cfd, "");
    exit(0);
}
