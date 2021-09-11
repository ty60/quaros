#include "types.h"
#include "util.h"
#include "string.h"
#include "fs.h"
#include "io.h"
#include "memory.h"

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
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        filesystem[i].in_use = 0;
    }

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

        if (size > PGSIZE) {
            // TODO: Solving this is pretty difficult.
            // Since we currently only support page level dynamic memory allocation.
            panic("init_fs: file larger than 4K is unsupported");
        }

        // parse and point to file data
        char *data = (char *)arp + sizeof(struct ar_hdr);
        filesystem[file_cnt].data = kmalloc();
        memset(filesystem[file_cnt].data, 0, PGSIZE);
        memcpy(filesystem[file_cnt].data, data, size);

        filesystem[file_cnt].id = file_id++;
        filesystem[file_cnt].type = FT_REGULAR;
        filesystem[file_cnt].in_use = 1;

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

    for (i = 0; i < MAX_FILES; i++) {
        if (strcmp(path, filesystem[i].name) == 0) {
            return &filesystem[i];
        }
    }
    return NULL;
}


struct file *alloc_file(const char *path) {
    int i;
    struct file *fp = NULL;
    for (i = 0; i < MAX_FILES; i++) {
        if (!filesystem[i].in_use) {
            fp = &filesystem[i];
            break;
        }
    }
    if (!fp || sizeof(fp->name) - 1 < strlen(path)) {
        return NULL;
    }
    strcpy(filesystem[i].name, path);
    filesystem[i].data = kmalloc();
    filesystem[i].in_use = 1;
    filesystem[i].size = 0;
    return fp;
}


void release_file(struct file *fp) {
    if (!(filesystem <= fp && filesystem + sizeof(filesystem) < fp)) {
        panic("release_file: realeasing file out of filesystem");
    }
    memset(fp->name, 0, sizeof(fp->name));
    if (fp->data) {
        kfree(fp->data);
    }
    fp->in_use = 0;
}


int list_rootdir(char *buf) {
    int i;
    char *p = buf;
    for (i = 0; i < MAX_FILES; i++) {
        int len;
        if (!(filesystem[i].in_use && filesystem[i].type == FT_REGULAR)) {
            continue;
        }
        len = strlen(filesystem[i].name);
        if (len == 0) {
            // TODO:
            // For some unknown reasons there are files with strlen(name) == 0
            continue;
        }
        if ((p + len + 1) - buf > MAX_FILE_SIZE) {
            return -1;
        }
        strcpy(p, filesystem[i].name);
        p += len;
        *p++ = '\n';
    }
    return p - buf;
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

int write_file(struct file *file, char *buf, size_t count) {
    if (file->pos + count > PGSIZE) {
        // TODO: Support this.
        panic("File larger than PGSIZE is unsupported");
    }
    memcpy(file->data + file->pos, buf, count);
    file->pos += count;
    return 0;
}
