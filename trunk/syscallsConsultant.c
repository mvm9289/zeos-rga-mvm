
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm-i386/uaccess.h>
#include "include/syscallsMonitor.h"
#include "include/syscallsConsultant.h"
#include <linux/errno.h>

MODULE_AUTHOR("ROGER ORIOL GARCIA ALVAREZ & MIGUEL ANGEL VICO MOYA");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Consultant of syscalls statistics of monitored processes.");

/* Consultant Initialization */
static int __init syscallsConsultant_init(void) {
    int res;

    device = MKDEV(CONSULTANT_MAJOR, CONSULTANT_MINOR);
    res = register_chrdev_region(device, 1, "syscallsConsultant");
    if (res < 0) return -1;

    consultant_cdev = cdev_alloc();
    consultant_cdev->owner = THIS_MODULE;
    consultant_cdev->ops = &consultant_ops;
    res = cdev_add(consultant_cdev, device, 1);
    if (res < 0) return -1;

    printk(KERN_EMERG "Syscalls Consultant Module loaded!\n\n");

    return 0; 
}

/* Module Operations */
int consultant_open(struct inode *i, struct file *f) {
    if (current->uid != 0) return -EPERM;

    if (open) return -EBUSY;
    open = 1;

    first_pid = current->pid;
    actual_process = current;
    monitor_syscall = OPEN_CALL;

    return 0;
}

ssize_t consultant_read(struct file *f, char __user *buffer, size_t s, loff_t *off) {
    struct thread_info_extended *th_info;
    int size;
    unsigned long res;

    th_info = (struct thread_info_extended *)actual_process->thread_info;
    size = s;
    if (s > sizeof(struct t_stats)) size = sizeof(struct t_stats);

    res = copy_to_user(buffer, &th_info->stats[monitor_syscall], size);

    return (ssize_t)(size-res);
}

int consultant_ioctl(struct inode *i, struct file *f, unsigned int arg1, unsigned long arg2) {
    struct task_struct *task;
    int process_pid;
    int res;

    switch (arg1) {
        case SWITCH_PROCESS:
            if((int *)arg2 == NULL) task = find_task_by_pid(first_pid);
            else {
                res = copy_from_user(&process_pid, (int *)arg2, sizeof(int));
                task = find_task_by_pid(process_pid);
            }

            if (task == NULL) return -ESRCH;
            else actual_process = task;

            if (((struct thread_info_extended *)actual_process->thread_info)->pid != actual_process->pid)
                reset_stats(((struct thread_info_extended *)actual_process->thread_info)->stats);
            break;
        case SWITCH_SYSCALL:
            monitor_syscall = (int)arg2;
            break;
        case RESET:
            reset_stats(((struct thread_info_extended *)actual_process->thread_info)->stats);
            break;
        case RESET_ALL:
            for_each_process(task) {
                reset_stats(((struct thread_info_extended *)task->thread_info)->stats);
            }
            break;
        case ACTIVATE_MONITOR:
            if ((int)arg2 != ALL && ((int)arg2 < 0 || (int)arg2 > SYSCALLS_MONITORIZED-1)) return -EINVAL;
            activate_monitor((int)arg2);
            break;
        case DEACTIVATE_MONITOR:
            if ((int)arg2 != ALL && ((int)arg2 < 0 || (int)arg2 > SYSCALLS_MONITORIZED-1)) return -EINVAL;
            deactivate_monitor((int)arg2);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

int consultant_release(struct inode *i, struct file *f) {
    open = 0;

    return 0;
}

/* Module Download */
static void __exit syscallsConsultant_exit(void) {
    cdev_del(consultant_cdev);
    unregister_chrdev_region(device, 1);
    
    printk(KERN_EMERG "\nSyscalls Consultant Module downloaded!\n\n");
}

module_init(syscallsConsultant_init);
module_exit(syscallsConsultant_exit);
