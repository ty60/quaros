#define VRAM 0xb8000
#define COLOR 0x2a


char msg[] = "Hello world!";


void initbg(void) {
    int i;
    short *now = (short *)VRAM;
    for (i = 0; i < 2000; i++) {
        *now = COLOR << 8;
        now++;
    }
}


void print(char *msg, int n) {
    int i;
    char *now = (char *)VRAM;
    for (i = 0; i < n; i++) {
        *now = msg[i];
        now += 2;
    }
}


int main(void) {
    initbg();
    print(msg, sizeof(msg));
}
