#ifndef DEVICES_H__
#define  DEVICES_H__

#define FILE_NAME_SIZE 10
#define DIR_ENTRYS 10
#define CTABLE_SIZE 10

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

#define NR_TASKS      10 //RETOCAR

struct file_operations {
    void *read;
    void *write;
};

struct logic_device {
    char name[FILE_NAME_SIZE];
    int acces_mode;
    struct file_operations *operations;
};

struct directory {
    struct logic_device dir_entrys[DIR_ENTRYS];
};

struct TFO_item {
    int num_refs;
    int seq_pos;
    int init_acces_mode;
};

struct channel {
    unsigned int free;
    struct TFO_item *TFO_pointer;
    struct logic_device *log_device;
};

struct directory DIR;
struct file_operations ops[DIR_ENTRYS];
struct TFO_item TFO[CTABLE_SIZE*NR_TASKS];

int sys_write_console(char *buffer,int size);
int sys_read_keyboard();
#endif /* DEVICES_H__*/
