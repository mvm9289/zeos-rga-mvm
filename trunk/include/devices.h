#ifndef DEVICES_H__
#define  DEVICES_H__

#define FILE_NAME_SIZE 10
#define DIR_ENTRYS 10
#define CTABLE_SIZE 10

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RWONLY 2

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
};

struct channel {
    int seq_pos;
    int init_acces_mode;
};

int sys_write_console(char *buffer,int size);
int sys_read_keyboard();
#endif /* DEVICES_H__*/
