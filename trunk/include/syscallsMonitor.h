
#ifndef __SYSCALLS_MONITOR__
#define __SYSCALLS_MONITOR__

#include "syscallsmon.h"

#define SYSCALLS_MONITORIZED 5

#define SYSCALL_OPEN 5
#define SYSCALL_WRITE 4
#define SYSCALL_LSEEK 19
#define SYSCALL_CLOSE 6
#define SYSCALL_CLONE 120

#define STATS_NO_INIT -1

#define proso_rdtsc(low,high) \
__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

static inline unsigned long long proso_get_cycles (void) {
  unsigned long eax, edx;

  proso_rdtsc(eax, edx);

  return ((unsigned long long) edx << 32) + eax;
}


/* Monitor Data */
typedef struct t_stats statistics[SYSCALLS_MONITORIZED];

struct thread_info_extended {
    struct thread_info info;
    statistics stats;
    int pid;
};

extern void *sys_call_table[];
void *old_sys_calls[SYSCALLS_MONITORIZED];
void *new_sys_calls[SYSCALLS_MONITORIZED];

char *names[SYSCALLS_MONITORIZED] = {"OPEN", "CLOSE", "WRITE", "LSEEK", "CLONE"};
int syscall_pos_of[SYSCALLS_MONITORIZED] = {SYSCALL_OPEN, SYSCALL_CLOSE, SYSCALL_WRITE, SYSCALL_LSEEK, SYSCALL_CLONE};

/* Monitor Operations */
long new_sys_open (const char __user *filename, int flags, int mode);
long new_sys_close(unsigned int fd);
ssize_t new_sys_write(unsigned int fd, const char __user * buf, size_t count);
off_t new_sys_lseek(unsigned int fd, off_t offset, unsigned int origin);
int new_sys_clone(struct pt_regs regs);

void activate_monitor(int syscall);
void deactivate_monitor(int syscall);
void reset_stats(struct t_stats *stats);
void print_stats(void);

#endif
