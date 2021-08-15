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

int file_id = 0;

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

        filesystem[file_cnt].id = file_id++;
        filesystem[file_cnt].type = FT_REGULAR;

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


struct file console_file;
void init_dev_file(void) {
    console_file.id = file_id++;
    console_file.type = FT_DEV;
    strcpy(console_file.name, "console");
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


int read_file(struct file *file, char *buf, size_t count) {
    uint32_t left = file->size - file->pos;
    if (left < count) {
        memcpy(buf, file->data, left);
        file->pos += left;
        return left;
    } else {
        memcpy(buf, file->data, count);
        file->pos += count;
        return count;
    }
}


/*
 * TODO: write_file
 * This is difficult because current (struct file *)->data points directly
 * to the original ar file.  So memory size of data can't be extended.
 * data region can be directly allocated, but since kmalloc() only supports
 * single page allocation per call, file size more than 4K won't be allowed.
 */
int write_file(void) {
    // Stub
    return 0;
}
