#ifndef DEVICES_H__
#define  DEVICES_H__

#define FILE_NAME_SIZE 10
#define DIR_ENTRIES 10
#define CTABLE_SIZE 10
#define OFT_MAX_SIZE 50

struct logic_device {
    char name[FILE_NAME_SIZE];
    int free;
    int nb_refs;
    int access_mode;
    struct file_operations *ops;
};

struct logic_device DIR[DIR_ENTRIES];

struct file_operations {
    int (*sys_open_dep) (const char *, int);
    int (*sys_read_dep)(int *, char *, int);
    int (*sys_write_dep)(int *, const char *, int);
    int (*sys_release_dep)(struct logic_device *);
    int (*sys_unlink_dep)(struct logic_device *);
};

struct file_operations file_ops[DIR_ENTRIES];

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
inline struct logic_device* searchFile(char *name);
inline int getFreeChannel(struct channel *channels);
inline struct OFT_item* getNewOpenedFile();
int sys_open_console(const char *path, int flags);
int sys_write_console(int *pos, const char *buffer,int size);
int sys_open_keyboard(const char *path, int flags);
int sys_read_keyboard(int *pos, char *buffer, int size);
int sys_unlink_dev (struct logic_device *file);
int sys_open_file(const char *path, int flags);
int sys_read_file(int *pos, char *buffer, int size);
int sys_write_file(int *pos, char *buffer, int size);
int sys_unlink_file(struct logic_device *file);
int sys_release_std(struct logic_device *file);

#endif /* DEVICES_H__*/
