
#ifndef __SYSCALLS_MON__
#define __SYSCALLS_MON__

#define OPEN 0
#define CLOSE 1
#define WRITE 2
#define LSEEK 3
#define CLONE 4

#define ALL -1

struct t_stats {
    int pid;
    int total_calls;
    int ok_calls;
    int error_calls;
    unsigned long long total_time;
};

#endif
