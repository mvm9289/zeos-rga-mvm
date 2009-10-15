/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */

#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>
#include <string.h>

/* ERRNO */
#include <errno.h>
int errno;
void perror(); /* print errno error message */



/* Wrapper of system calls*/
int write(int fd,char *buffer,int size);

int getpid(void);
int fork(void);
void exit(void);
int nice (int quantum);

int get_stats(int pid, struct stats *st);

#endif  /* __LIBC_H__ */
