#ifndef FS_H
#define FS_H

#include "types.h"

#define AR_MAGIC "!<arch>\n"
#define MAX_FILES 32
#define MAX_NAME 15
#define MAX_FILE_SIZE PGSIZE

struct ar_hdr {
    uint8_t file_id[16];
    uint8_t timestamp[12];
    uint8_t owner_id[6];
    uint8_t group_id[6];
    uint8_t file_mode[8];
    uint8_t size[10];
    uint8_t end[2];
} __attribute__ ((packed));

enum file_type {
    FT_REGULAR,
    FT_DEV,
    FT_DIR,
};

struct file {
    uint32_t id;
    uint32_t size;
    uint32_t pos;
    uint32_t type;
    int in_use;
    int8_t name[MAX_NAME + 1];
    int8_t *data;
};

extern int file_id;

struct file filesystem[MAX_FILES];

void init_fs(void);
void init_dev_file(void);
struct file *get_file(const char *path);
struct file *alloc_file(const char *path);
void release_file(struct file *fp);
int read_file(struct file *file, char *buf, size_t count);
int write_file(struct file *file, char *buf, size_t count);
int list_rootdir(char *buf);


#endif
