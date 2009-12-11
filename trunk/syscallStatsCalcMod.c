#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_AUTHOR("ROGER ORIOL GARCIA ALVAREZ & MIGUEL ANGEL VICO MOYA");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Intercept syscalls and calculate statistics");

#define proso_rdtsc(low,high) \
__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

static inline unsigned long long proso_get_cycles (void) {
  unsigned long eax, edx;

  proso_rdtsc(eax, edx);

  return ((unsigned long long) edx << 32) + eax;
}

#define SYSCALLS_MONITORIZED 5

/* Module Data */

int caller_process_pid; 

struct statistics {
    int pid;
    int total_calls;
    int ok_calls;
    int error_calls;
    unsigned long long total_time;
};

struct thread_info_extended {
    struct thread_info *info;
    struct statistic stats[SYSCALLS_MONITORIZED];
};

extern void *sys_call_table[];

struct old_operations {
    void *sys_open_old;
    void *sys_close_old;
    void *sys_write_old;
    void *sys_lseek_old;
    void *sys_clone_old;
};

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

int sys_open_local (const char *filename, int flags) // no es correcta la cabecera creo
{
    unsigned long long cycles;
    int res;
    if(current_thread_info()->stats[0]->pid != current()->pid) {

         /* initialize statistics */
        current_thread_info()->stats[0]->pid = current()->pid;
        current_thread_info()->stats[0]->total_calls = 0;
        current_thread_info()->stats[0]->ok_calls = 0;
        current_thread_info()->stats[0]->error_calls = 0;
        current_thread_info()->stats[0]->total_time = 0;

    }

    current_thread_info()->stats[0]->total_calls++;
    cycles = proso_get_cycles();
    res = (int) old_operations.sys_open_old(name);

    if(res < 0) current_thread_info()->stats[0]->error_calls++;
    else current_thread_info()->stats[0]->ok_calls++;

    cycles = proso_get_cycles() - cycles;
    current_thread_info()->stats[0]->total_time += cycles;

    return res;    
}  


/* Module Initialization */
static int __init syscallStatsCalcMod_init(void) {

    int caller_process_pid = current()->pid;

    old_operations.sys_open_old = sys_call_table[5];
    old_operations.sys_close_old = sys_call_table[6];
    old_operations.sys_write_old = sys_call_table[4];
    old_operations.sys_lseek_old = sys_call_table[19];
    old_operations.sys_clone_old = sys_call_table[120];

    sys_call_table[5] = (void *) sys_open_local;
    sys_call_table[6] = (void *) sys_close_local;
    sys_call_table[4] = (void *) sys_write_local;
    sys_call_table[19] = (void *) sys_lseek_local;
    sys_call_table[120] = (void *) sys_close_local;

    printk(KERN_DEBUG “Mymodule carregat amb exit\n”);
    return 0;
}

/* Module Download */
static void __exit syscallStatsCalcMod_exit(void) {

    sys_call_table[5] = old_operations.sys_open_old;
    sys_call_table[6] = old_operations.sys_close_old;
    sys_call_table[4] = old_operations.sys_write_old;
    sys_call_table[19] = old_operations.sys_lseek_old;
    sys_call_table[120] = old_operations.sys_clone_old;

    /* buscar la task_struct del proceso caller_process_pid + imprimir stats */    
}
