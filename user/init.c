#include "sys.h"
#include "util.h"

int main(void) {
    execv("sh", NULL);
    return 0;
}
