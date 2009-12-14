#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_AUTHOR("ROGER ORIOL GARCIA ALVAREZ & MIGUEL ANGEL VICO MOYA");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Intercept syscalls and calculate statistics");


#define SYSCALLS_MONITORIZED 5

#define OPEN 0
#define WRITE 1
#define LSEEK 2
#define CLOSE 3
#define CLONE 4

#define SYSCALL_OPEN 5
#define SYSCALL_WRITE 4
#define SYSCALL_LSEEK 19
#define SYSCALL_CLOSE 6
#define SYSCALL_CLONE 120

#define STATS_NO_INIT 0

#define proso_rdtsc(low,high) \
__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

static inline unsigned long long proso_get_cycles (void) {
  unsigned long eax, edx;

  proso_rdtsc(eax, edx);

  return ((unsigned long long) edx << 32) + eax;
}


/* Module Data */

struct t_stats {
    int pid;
    int total_calls;
    int ok_calls;
    int error_calls;
    unsigned long long total_time;
};

typedef struct t_stats statistics[SYSCALLS_MONITORIZED];

struct thread_info_extended {
    struct thread_info info;
    statistics stats;
    int pid;
};

extern void *sys_call_table[];
void *old_sys_calls[SYSCALLS_MONITORIZED];

int pid = 0;
module_param(pid, int, 0);

char *names[5] = {"OPEN", "WRITE", "LSEEK", "CLOSE", "CLONE"};

long new_sys_open (const char __user *filename, int flags, int mode);
ssize_t new_sys_write(unsigned int fd, const char __user * buf, size_t count);
int new_sys_clone(struct pt_regs regs);
long new_sys_close(unsigned int fd);
off_t new_sys_lseek(unsigned int fd, off_t offset, unsigned int origin);

void reset_stats(struct t_stats *stats);
void print_stats(int pid);

/* Module Initialization */
static int __init syscallStatsCalcMod_init(void) {
    struct task_struct *task;

    if (pid == 0) pid = current->pid;

    for_each_process(task) {
        ((struct thread_info_extended *)task->thread_info)->pid = STATS_NO_INIT;
    } 

    old_sys_calls[OPEN] = sys_call_table[SYSCALL_OPEN];
    old_sys_calls[WRITE] = sys_call_table[SYSCALL_WRITE];
    old_sys_calls[LSEEK] = sys_call_table[SYSCALL_LSEEK];
    old_sys_calls[CLOSE] = sys_call_table[SYSCALL_CLOSE];
    old_sys_calls[CLONE] = sys_call_table[SYSCALL_CLONE];

    sys_call_table[SYSCALL_OPEN] = (void *) new_sys_open;
    sys_call_table[SYSCALL_WRITE] = (void *) new_sys_write;
    sys_call_table[SYSCALL_LSEEK] = (void *) new_sys_lseek;
    sys_call_table[SYSCALL_CLOSE] = (void *) new_sys_close;
    sys_call_table[SYSCALL_CLONE] = (void *) new_sys_clone;

    printk(KERN_DEBUG "Syscall Stats Calculator Module loaded!\n\n");

    return 0;
}

/* Module operations */
long new_sys_open (const char __user *filename, int flags, int mode) {
    unsigned long long cycles;
    long res;
    struct thread_info_extended *th_info = (struct thread_info_extended *)current_thread_info();
    
    try_module_get(THIS_MODULE);
    if(th_info->pid != current->pid) {
        reset_stats(th_info->stats);
        th_info->pid = current->pid;
    }

    th_info->stats[OPEN].total_calls++;
    cycles = proso_get_cycles();
    res = ((long (*)(const char __user *, int, int))old_sys_calls[OPEN])(filename, flags, mode);
    cycles = proso_get_cycles() - cycles;
    th_info->stats[OPEN].total_time += cycles;

    if(res < 0) th_info->stats[OPEN].error_calls++;
    else th_info->stats[OPEN].ok_calls++;

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

    th_info->stats[WRITE].total_calls++;
    cycles = proso_get_cycles();
    res = ((ssize_t (*)(unsigned int, const char __user *, size_t))old_sys_calls[WRITE])(fd, buf, count);
    cycles = proso_get_cycles() - cycles;
    th_info->stats[WRITE].total_time += cycles;

    if(res < 0) th_info->stats[WRITE].error_calls++;
    else th_info->stats[WRITE].ok_calls++;

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

    th_info->stats[CLONE].total_calls++;
    cycles = proso_get_cycles();
    res = ((int (*)(struct pt_regs))old_sys_calls[CLONE])(regs);
    cycles = proso_get_cycles() - cycles;
    th_info->stats[CLONE].total_time += cycles;

    if(res < 0) th_info->stats[CLOSE].error_calls++;
    else th_info->stats[CLONE].ok_calls++;

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

    th_info->stats[CLOSE].total_calls++;
    cycles = proso_get_cycles();
    res = ((long (*)(unsigned int))old_sys_calls[CLOSE])(fd);
    cycles = proso_get_cycles() - cycles;
    th_info->stats[CLOSE].total_time += cycles;

    if(res < 0) th_info->stats[CLOSE].error_calls++;
    else th_info->stats[CLOSE].ok_calls++;

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

    th_info->stats[LSEEK].total_calls++;
    cycles = proso_get_cycles();
    res = ((off_t (*)(unsigned int, off_t, unsigned int))old_sys_calls[LSEEK])(fd, offset, origin);
    cycles = proso_get_cycles() - cycles;
    th_info->stats[LSEEK].total_time += cycles;

    if(res < 0) th_info->stats[LSEEK].error_calls++;
    else th_info->stats[LSEEK].ok_calls++;

    module_put(THIS_MODULE);

    return res;
}

/* Module Download */
static void __exit syscallStatsCalcMod_exit(void) {
    sys_call_table[SYSCALL_OPEN] = old_sys_calls[OPEN];
    sys_call_table[SYSCALL_WRITE] = old_sys_calls[WRITE];
    sys_call_table[SYSCALL_LSEEK] = old_sys_calls[LSEEK];
    sys_call_table[SYSCALL_CLOSE] = old_sys_calls[CLOSE];
    sys_call_table[SYSCALL_CLONE] = old_sys_calls[CLONE];

    print_stats(pid);
}

void reset_stats(struct t_stats *stats) {
    int i;

    for (i = 0; i < SYSCALLS_MONITORIZED; ++i) {
        stats[i].total_calls = 0;
        stats[i].ok_calls = 0;
        stats[i].error_calls = 0;
        stats[i].total_time = 0;
    }
}

void print_stats(int pid) { //BORRAR ATRIBUTO SI SOLO SE USA EN EL EXIT
    struct task_struct *task;
    struct t_stats *stats;
    int i;

    task = find_task_by_pid(pid);

    if (task == NULL) printk(KERN_DEBUG "ERROR: Stats of process %d can not be displayed because the process is dead.", pid);
    else {
        stats = ((struct thread_info_extended *)task->thread_info)->stats;
        for (i = 0; i < SYSCALLS_MONITORIZED; ++i) {
            printk("Total calls of %s: %d\n", names[i], stats[i].total_calls);
            printk("Ok calls: %d\n", stats[i].ok_calls);
            printk("Error calls: %d\n", stats[i].error_calls);
            printk("Total time: %lld\n\n", stats[i].total_time);
        }
    }
}

module_init(syscallStatsCalcMod_init);
module_exit(syscallStatsCalcMod_exit);




/*
struct file_operations {
    struct module *owner;
    ssize_t(*read (struct file *, char __user *, size_t, loff_t *);
    int (*ioctl (struct inode *, struct file *, unsigned int, unsigned long);
    int (*open (struct inode *, struct file *);
    int (*release (struct inode *, struct file *);
}


struct file_operations mymod_fops = {
  owner: THIS_MODULE,
  read: mymod_read,
  ioctl: mymod_ioctl,
  open: mymod_open,
  release: mymod_release,
};
*/

/* Module Operations */

/* no se hasta que punto van aquí estas funciones, pero yo las pongo por tenerlas aki a mano
dev_t MKDEV(unsigned int major, unsigned int minor);
int register_chrdev_region (dev_t first, unsigned int count, const char *name);
void unregister_chrdev_region(dev_t first, unsigned int count);
*/
