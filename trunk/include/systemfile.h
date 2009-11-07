#ifndef __SYSTEMFILE_H__
#define  __SYSTEMFILE_H__

#define FILE_NAME_SIZE 10
#define DIR_ENTRIES 10
#define CTABLE_SIZE 10

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

struct {
    int (*sys_read_dev)(int, char *, int);
    int (*sys_write_dev)(int, const char *, int);
} file_operations[DIR_ENTRIES];

struct logic_device {
    char name[FILE_NAME_SIZE];
    int acces_mode;
    unsigned int ops_indx;
};

struct logic_device DIR[DIR_ENTRIES];

struct channel {
    unsigned int free;
    unsigned int OFT_indx;
    struct logic_device *log_device;
};

#include <sched.h>

struct {
    int num_refs;
    int seq_pos;
    int init_acces_mode;
} OFT[CTABLE_SIZE*NR_TASKS];

#endif
