#ifndef _SYS_H_
#define _SYS_H_

#include <stats.h>
#include <devices.h>

#define W_SIZE 256

int comprova_fd(int fd, int operacio);
int sys_ni_syscall();
int sys_open (char *path, int flags);
int sys_write(int fd,char *buffer, int size);
int sys_getpid(void);
int sys_fork(void);
int sys_nice(int quantum);
int sys_sem_init(int n_sem, unsigned int value);
int sys_sem_wait(int n_sem);
int sys_sem_signal(int n_sem);
int sys_sem_destroy(int n_sem);
void sys_exit(void);
int sys_getstats(int pid, struct stats *st);

#endif
