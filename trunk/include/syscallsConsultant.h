
#ifndef __SYSCALLS_CONSULTANT__
#define __SYSCALLS_CONSULTANT__

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define CONSULTANT_MAJOR 256
#define CONSULTANT_MINOR 0

/* Consultant Data */
dev_t device;
struct cdev *consultant_cdev;
unsigned int open = 0;
int first_pid;
struct task_struct *actual_process;
int monitor_syscall;

/* Consultant Operations */
int consultant_open(struct inode *i, struct file *f);
ssize_t consultant_read(struct file *f, char __user *buffer, size_t s, loff_t *off);
int consultant_ioctl(struct inode *i, struct file *f, unsigned int arg1, unsigned long arg2);
int consultant_release(struct inode *i, struct file *f);

struct file_operations consultant_ops = {
    owner: THIS_MODULE,
    read: consultant_read,
    ioctl: consultant_ioctl,
    open: consultant_open,
    release: consultant_release,
};

#endif
