#ifndef DEVICES_H__
#define  DEVICES_H__

#define FILE_NAME_SIZE 10
#define DIR_ENTRIES 10
#define CTABLE_SIZE 10

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

struct {
    int (*sys_read_dep)(int, char *, int);
    int (*sys_write_dep)(int, const char *, int);
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



void init_devices();
inline int getFile(char *name);
inline int getFreeChannel(struct channel *channels);
inline int getFreeOFTpos();
int sys_write_console(int pos, const char *buffer,int size);
int sys_read_keyboard(int pos, char *buffer, int size);

#endif /* DEVICES_H__*/
