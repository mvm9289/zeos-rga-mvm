/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */

#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

/* ERRNO */
#include <errno.h>
int errno;
void perror(); /* print errno error message */



/* Wrapper of  write system call*/
int write(int fd,char *buffer,int size);

#endif  /* __LIBC_H__ */
