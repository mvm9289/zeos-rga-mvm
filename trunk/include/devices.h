#ifndef DEVICES_H__
#define  DEVICES_H__

#define FILE_NAME_SIZE 10
#define DIR_ENTRIES 10
#define CTABLE_SIZE 10
#define OFT_MAX_SIZE 50

struct file_operations {
    int (*sys_read_dep)(int *, char *, int);
    int (*sys_write_dep)(int *, const char *, int);
};

struct file_operations file_ops[DIR_ENTRIES];

struct logic_device {
    char name[FILE_NAME_SIZE];
    int access_mode;
    struct file_operations *ops;
};

struct logic_device DIR[DIR_ENTRIES];

struct channel {
    unsigned int free;
    struct OFT_item *opened_file;
};

#include <sched.h>

struct OFT_item {
    int num_refs;
    int seq_pos;
    int init_access_mode;
    struct logic_device *file;
};

struct OFT_item OFT[OFT_MAX_SIZE];



void init_devices();
inline int pathlen_isOK(const char *path);
inline struct logic_device* getFile(char *name);
inline int getFreeChannel(struct channel *channels);
inline struct OFT_item* getNewOpenedFile();
int sys_write_console(int *pos, const char *buffer,int size);
int sys_read_keyboard(int *pos, char *buffer, int size);

#endif /* DEVICES_H__*/
