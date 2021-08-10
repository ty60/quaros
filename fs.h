#ifndef FS_H
#define FS_H

#include "types.h"

#define AR_MAGIC "!<arch>\n"
#define MAX_FILES 32
#define MAX_NAME 15

struct ar_hdr {
    uint8_t file_id[16];
    uint8_t timestamp[12];
    uint8_t owner_id[6];
    uint8_t group_id[6];
    uint8_t file_mode[8];
    uint8_t size[10];
    uint8_t end[2];
} __attribute__ ((packed));

struct file {
    uint32_t size;
    int8_t name[MAX_NAME + 1];
    int8_t *data;
};

struct file filesystem[MAX_FILES];

void init_fs(void);
struct file *get_file(const char *path);


#endif
