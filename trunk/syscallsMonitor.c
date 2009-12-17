
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "include/syscallsMonitor.h"

MODULE_AUTHOR("ROGER ORIOL GARCIA ALVAREZ & MIGUEL ANGEL VICO MOYA");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Syscalls Monitor.");

int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "Process identifier to monitor (default is pid of current process)");

/* Monitor Initialization */
static int __init syscallsMonitor_init(void) {
    struct task_struct *task;

    if (!pid) pid = current->pid;

    for_each_process(task) {
        ((struct thread_info_extended *)task->thread_info)->pid = STATS_NO_INIT;
    }

    old_sys_calls[OPEN_CALL] = sys_call_table[SYSCALL_OPEN];
    old_sys_calls[CLOSE_CALL] = sys_call_table[SYSCALL_CLOSE];
    old_sys_calls[WRITE_CALL] = sys_call_table[SYSCALL_WRITE];
    old_sys_calls[LSEEK_CALL] = sys_call_table[SYSCALL_LSEEK];
    old_sys_calls[CLONE_CALL] = sys_call_table[SYSCALL_CLONE];

    new_sys_calls[OPEN_CALL] = (void *) new_sys_open;
    new_sys_calls[CLOSE_CALL] = (void *) new_sys_close;
    new_sys_calls[WRITE_CALL] = (void *) new_sys_write;
    new_sys_calls[LSEEK_CALL] = (void *) new_sys_lseek;
    new_sys_calls[CLONE_CALL] = (void *) new_sys_clone;

    activate_monitor(-1);
    printk(KERN_EMERG "\nSyscalls Monitor Module loaded!\n\n");

    return 0;
}

/* Monitor Operations */
long new_sys_open (const char __user *filename, int flags, int mode) {
    unsigned long long cycles;
    long res;
    struct thread_info_extended *th_info = (struct thread_info_extended *)current_thread_info();
    
    try_module_get(THIS_MODULE);

    if(th_info->pid != current->pid) {
        reset_stats(th_info->stats);
        th_info->pid = current->pid;
    }

    cycles = proso_get_cycles();
    res = ((long (*)(const char __user *, int, int))old_sys_calls[OPEN_CALL])(filename, flags, mode);
    cycles = proso_get_cycles() - cycles;

    th_info->stats[OPEN_CALL].total_calls++;
    if(res < 0) th_info->stats[OPEN_CALL].error_calls++;
    else th_info->stats[OPEN_CALL].ok_calls++;
    th_info->stats[OPEN_CALL].total_time += cycles;

    module_put(THIS_MODULE);

    return res;
}

long new_sys_close(unsigned int fd) {
    unsigned long long cycles;
    long res;
    struct thread_info_extended *th_info = (struct thread_info_extended *)current_thread_info();
    
    try_module_get(THIS_MODULE);

    if(th_info->pid != current->pid) {
        reset_stats(th_info->stats);
        th_info->pid = current->pid;
    }

    cycles = proso_get_cycles();
    res = ((long (*)(unsigned int))old_sys_calls[CLOSE_CALL])(fd);
    cycles = proso_get_cycles() - cycles;

    th_info->stats[CLOSE_CALL].total_calls++;
    if(res < 0) th_info->stats[CLOSE_CALL].error_calls++;
    else th_info->stats[CLOSE_CALL].ok_calls++;
    th_info->stats[CLOSE_CALL].total_time += cycles;

    module_put(THIS_MODULE);

    return res;
}

ssize_t new_sys_write(unsigned int fd, const char __user * buf, size_t count) {
    unsigned long long cycles;
    ssize_t res;
    struct thread_info_extended *th_info = (struct thread_info_extended *)current_thread_info();
    
    try_module_get(THIS_MODULE);

    if(th_info->pid != current->pid) {
        reset_stats(th_info->stats);
        th_info->pid = current->pid;
    }

    cycles = proso_get_cycles();
    res = ((ssize_t (*)(unsigned int, const char __user *, size_t))old_sys_calls[WRITE_CALL])(fd, buf, count);
    cycles = proso_get_cycles() - cycles;

    th_info->stats[WRITE_CALL].total_calls++;
    if(res < 0) th_info->stats[WRITE_CALL].error_calls++;
    else th_info->stats[WRITE_CALL].ok_calls++;
    th_info->stats[WRITE_CALL].total_time += cycles;

    module_put(THIS_MODULE);

    return res;
}

off_t new_sys_lseek(unsigned int fd, off_t offset, unsigned int origin) {
    unsigned long long cycles;
    off_t res;
    struct thread_info_extended *th_info = (struct thread_info_extended *)current_thread_info();
    
    try_module_get(THIS_MODULE);

    if(th_info->pid != current->pid) {
        reset_stats(th_info->stats);
        th_info->pid = current->pid;
    }

    cycles = proso_get_cycles();
    res = ((off_t (*)(unsigned int, off_t, unsigned int))old_sys_calls[LSEEK_CALL])(fd, offset, origin);
    cycles = proso_get_cycles() - cycles;

    th_info->stats[LSEEK_CALL].total_calls++;
    if(res < 0) th_info->stats[LSEEK_CALL].error_calls++;
    else th_info->stats[LSEEK_CALL].ok_calls++;
    th_info->stats[LSEEK_CALL].total_time += cycles;

    module_put(THIS_MODULE);

    return res;
}

int new_sys_clone(struct pt_regs regs) {
    unsigned long long cycles;
    int res;
    struct thread_info_extended *th_info = (struct thread_info_extended *)current_thread_info();
 
    try_module_get(THIS_MODULE);

    if(th_info->pid != current->pid) {
        reset_stats(th_info->stats);
        th_info->pid = current->pid;
    }

    cycles = proso_get_cycles();
    res = ((int (*)(struct pt_regs))old_sys_calls[CLONE_CALL])(regs);
    cycles = proso_get_cycles() - cycles;

    th_info->stats[CLONE_CALL].total_calls++;
    if(res < 0) th_info->stats[CLONE_CALL].error_calls++;
    else th_info->stats[CLONE_CALL].ok_calls++;
    th_info->stats[CLONE_CALL].total_time += cycles;

    module_put(THIS_MODULE);

    return res;
}

/* Monitor Download */
static void __exit syscallsMonitor_exit(void) {
    deactivate_monitor(-1);
    print_stats();
    printk(KERN_EMERG "\nSyscalls Monitor Module downloaded!\n\n");
}

/* Auxiliary Operations */
EXPORT_SYMBOL(activate_monitor);
void activate_monitor(int syscall) {
    if (syscall == ALL) {
        sys_call_table[SYSCALL_OPEN] = new_sys_calls[OPEN_CALL];
        sys_call_table[SYSCALL_CLOSE] = new_sys_calls[CLOSE_CALL];
        sys_call_table[SYSCALL_WRITE] = new_sys_calls[WRITE_CALL];
        sys_call_table[SYSCALL_LSEEK] = new_sys_calls[LSEEK_CALL];
        sys_call_table[SYSCALL_CLONE] = new_sys_calls[CLONE_CALL];
    }
    else sys_call_table[syscall_pos_of[syscall]] = new_sys_calls[syscall];
}

EXPORT_SYMBOL(deactivate_monitor);
void deactivate_monitor(int syscall) {
    if (syscall == ALL) {
        sys_call_table[SYSCALL_OPEN] = old_sys_calls[OPEN_CALL];
        sys_call_table[SYSCALL_CLOSE] = old_sys_calls[CLOSE_CALL];
        sys_call_table[SYSCALL_WRITE] = old_sys_calls[WRITE_CALL];
        sys_call_table[SYSCALL_LSEEK] = old_sys_calls[LSEEK_CALL];
        sys_call_table[SYSCALL_CLONE] = old_sys_calls[CLONE_CALL];
    }
    else sys_call_table[syscall_pos_of[syscall]] = old_sys_calls[syscall];
}

EXPORT_SYMBOL(reset_stats);
void reset_stats(struct t_stats *stats) {
    int i;

    for (i = 0; i < SYSCALLS_MONITORIZED; ++i) {
        stats[i].total_calls = 0;
        stats[i].ok_calls = 0;
        stats[i].error_calls = 0;
        stats[i].total_time = 0;
    }
}

void print_stats(void) {
    struct task_struct *task;
    struct t_stats *stats;
    int i;

    task = find_task_by_pid(pid);

    if (task == NULL) printk(KERN_EMERG "\nERROR: Stats of process %d can not be displayed because the process is dead.\n", pid);
    else {
        stats = ((struct thread_info_extended *)task->thread_info)->stats;
        for (i = 0; i < SYSCALLS_MONITORIZED; ++i) {
            printk(KERN_EMERG "Total calls of %s: %d\n", names[i], stats[i].total_calls);
            printk(KERN_EMERG "Good calls: %d\n", stats[i].ok_calls);
            printk(KERN_EMERG "Error calls: %d\n", stats[i].error_calls);
            printk(KERN_EMERG "Total time: %lld\n\n", stats[i].total_time);
        }
    }
}

module_init(syscallsMonitor_init);
module_exit(syscallsMonitor_exit);
