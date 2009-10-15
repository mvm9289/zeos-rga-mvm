/*
 * libc.c 
 */

#include <libc.h>
#include <string.h>
#include <stats.h>

const char *sys_errlist[] = { "Operation not permitted",
                              "No such file or directory",
                              "No such process",
                              "Interrupted system call",
                              "I/O error",
                              "No such device or address",
                              "Argument list too long",
                              "Exec format error",
                              "Bad file number",
                              "No child processes",
                              "Try again",
                              "Out of memory",
                              "Permission denied",
                              "Bad address",
                              "Block device required",
                              "Device or resource busy",
                              "File exists",
                              "Cross-device link",
                              "No such device",
                              "Not a directory",
                              "Is a directory",
                              "Invalid argument",
                              "File table overflow",
                              "Too many open files",
                              "Not a typewriter",
                              "Text file busy",
                              "File too large",
                              "No space left on device",
                              "Illegal seek",
                              "Read-only file system",
                              "Too many links",
                              "Broken pipe",
                              "Math argument out of domain of func",
                              "Math result not representable",
                              "Resource deadlock would occur",
                              "File name too long",
                              "No record locks available",
                              "Function not implemented",
                              "Directory not empty",
                              "Too many symbolic links encountered",
                              "Operation would block",
                              "No message of desired type",
                              "Identifier removed",
                              "Channel number out of range",
                              "Level 2 not synchronized",
                              "Level 3 halted",
                              "Level 3 reset",
                              "Link number out of range",
                              "Protocol driver not attached",
                              "No CSI structure available",
                              "Level 2 halted",
                              "Invalid exchange",
                              "Invalid request descriptor",
                              "Exchange full",
                              "No anode",
                              "Invalid request code",
                              "Invalid slot",
                              "Resource deadlock would occur",
                              "Bad font file format",
                              "Device not a stream",
                              "No data available",
                              "Timer expired",
                              "Out of streams resources",
                              "Machine is not on the network",
                              "Package not installed",
                              "Object is remote",
                              "Link has been severed",
                              "Advertise error",
                              "Srmount error",
                              "Communication error on send",
                              "Protocol error",
                              "Multihop attempted",
                              "RFS specific error",
                              "Not a data message",
                              "Value too large for defined data type",
                              "Name not unique on network",
                              "File descriptor in bad state",
                              "Remote address changed",
                              "Can not access a needed shared library",
                              "Accessing a corrupted shared library",
                              ".lib section in a.out corrupted",
                              "Attempting to link in too many shared libraries",
                              "Cannot exec a shared library directly",
                              "Illegal byte sequence",
                              "Interrupted system call should be restarted",
                              "Streams pipe error",
                              "Too many users",
                              "Socket operation on non-socket",
                              "Destination address required",
                              "Message too long",
                              "Protocol wrong type for socket",
                              "Protocol not available",
                              "Protocol not supported",
                              "Socket type not supported",
                              "Operation not supported on transport endpoint",
                              "Protocol family not supported",
                              "Address family not supported by protocol",
                              "Address already in use",
                              "Cannot assign requested address",
                              "Network is down",
                              "Network is unreachable",
                              "Network dropped connection because of reset",
                              "Software caused connection abort",
                              "Connection reset by peer",
                              "No buffer space available",
                              "Transport endpoint is already connected",
                              "Transport endpoint is not connected",
                              "Cannot send after transport endpoint shutdown",
                              "Too many references: cannot splice",
                              "Connection timed out",
                              "Connection refused",
                              "Host is down",
                              "No route to host",
                              "Operation already in progress",
                              "Operation now in progress",
                              "Stale NFS file handle",
                              "Structure needs cleaning",
                              "Not a XENIX named type file",
                              "No XENIX semaphores available",
                              "Is a named type file",
                              "Remote I/O error",
                              "Quota exceeded",
                              "No medium found",
                              "Wrong medium type",
                              "Operation Canceled",
                              "Required key not available",
                              "Key has expired",
                              "Key has been revoked",
                              "Key was rejected by service" };

void perror() {
    write(1, "Error: ", 7);
    write(1, (char *)sys_errlist[errno-1], strlen((char *)sys_errlist[errno-1]));
    write(1, "\n", 1);
}


/* Wrapper of  write system call*/
int write(int fd,char *buffer,int size)
{
    int res=0;
    __asm__ __volatile (
        "pushl %%ebx\n"
        "movl 8(%%ebp), %%ebx\n"
        "movl 12(%%ebp), %%ecx\n"
        "movl 16(%%ebp), %%edx\n"
        "movl $4, %%eax\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        "popl %%ebx": "=g" (res) );

    if(res < 0) {
        errno = -res;
        res = -1;
    }

    return res;
}

int getpid (void)
{

    int res=0;
    __asm__ __volatile (
        "movl $20, %%eax\n"
        "int $0x80\n"
        "movl %%eax, %0" : "=g" (res)
    );
	
    return res;

}

int fork(void)
{
    int res=0;
    __asm__ __volatile__ (
        "movl $2, %%eax\n"
        "int $0x80\n"
        "movl %%eax, %0\n" : "=g" (res)
    );

    if(res < 0) {
        errno = -res;
        res = -1;
    }

    return res;
}

int nice (int quantum)
{

    int res=0;
    __asm__ __volatile__ (
        "pushl %%ebx\n"
        "movl 8(%%ebp), %%ebx\n"
        "movl $34, %%eax\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        "popl %%ebx" : "=g" (res)
    );

    if(res < 0) {
        errno = -res;
        res = -1;
    }

    return res;
}

void exit (void)    
{
    __asm__ __volatile__ (
        "movl  $1, %eax\n"
        "int $0x80"  // algo mas??
    );	

}

int get_stats(int pid, struct stats *st) {
    int res=0;
    __asm__ __volatile (
        "pushl %%ebx\n"
        "movl 8(%%ebp), %%ebx\n"
        "movl 12(%%ebp), %%ecx\n"
        "movl $35, %%eax\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        "popl %%ebx": "=g" (res) );

    if(res < 0) {
        errno = -res;
        res = -1;
    }

    return res;
}

