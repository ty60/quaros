#include "types.h"
#include "util.h"
#include "string.h"
#include "fs.h"
#include "io.h"

extern char fsar_start[];
extern char fsar_end[];


// In ar format ascii spaces (0x20) are used for padding.
// These could be annoying when parsing it.
// NULL it out with \0
void null_out_space(char *str, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) {
        if (str[i] == ' ') {
            str[i] = '\0';
        }
    }
}

void init_fs(void) {
    // TODO
    // Check if magic header is correct

    memset(filesystem, 0, sizeof(filesystem));

    int file_cnt = 0;
    struct ar_hdr* arp = (struct ar_hdr *)(fsar_start + strlen(AR_MAGIC));
    while (arp < (struct ar_hdr *)(fsar_end)) {
        int i, size;
        char tmp[16];

        // parse file name
        memcpy(tmp, arp->file_id, sizeof(arp->file_id));
        null_out_space(tmp, sizeof(arp->file_id));
        for (i = 0; i < (int)sizeof(arp->file_id); i++) {
            if (tmp[i] == '/') // '/' represents end of file name
                tmp[i] = '\0';
        }
        strcpy(filesystem[file_cnt].name, tmp);

        // parse file size
        memcpy(tmp, arp->size, sizeof(arp->size));
        null_out_space(tmp, sizeof(arp->size));
        size = atoi((char *)tmp);
        filesystem[file_cnt].size = size;

        // parse and point to file data
        char *data = (char *)arp + sizeof(struct ar_hdr);
        filesystem[file_cnt].data = data;

        file_cnt++;
        if (file_cnt >= MAX_FILES) {
            panic("Too many files");
        }

        // move to next ar entry
        arp = (struct ar_hdr *)((char *)(arp) + sizeof(struct ar_hdr) + size);
        if (size % 2 == 1) { // ar members are aligned to even byte boundaries
            arp = (struct ar_hdr *)((char *)(arp) + 1);
        }
    }
}


struct file *get_file(const char *path) {
    int i;
    if (path[0] == '\0') {
        return NULL;
    }

    for (i = 0; i < MAX_FILES; ++i) {
        if (strcmp(path, filesystem[i].name) == 0) {
            return &filesystem[i];
        }
    }
    return NULL;
}
