/*
 * sys.c - Syscalls implementation
 */

#include <devices.h>
#include <utils.h>
#include <errno.h>

#define ESCRIPTURA 0
#define LECTURA 1

#define W_SIZE 256

int comprova_fd(int fd, int operacio) {
    if(fd != 1) return -EBADF;
    if(operacio != ESCRIPTURA) return -EINVAL;

    return 0;
}

int sys_ni_syscall() {
    return -ENOSYS;
}

int sys_write(int fd,char *buffer, int size)
{
    char to_write[W_SIZE];
    int bytes=0;
    int err = comprova_fd(fd, ESCRIPTURA);

    if(err != 0) return err;
    if(!buffer) return -EFAULT;
    if(size < 0) return -EINVAL;

    while (size > W_SIZE) {
        copy_from_user(buffer+bytes, to_write, W_SIZE);
        bytes += sys_write_console(to_write, W_SIZE);
        size -= W_SIZE;
    }
    copy_from_user(buffer+bytes, to_write, size);
    bytes += sys_write_console(to_write, size);

    return bytes;
}

