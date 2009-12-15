
#ifndef __SYSCALLS_MON__
#define __SYSCALLS_MON__

#define OPEN_CALL 0
#define CLOSE_CALL 1
#define WRITE_CALL 2
#define LSEEK_CALL 3
#define CLONE_CALL 4

#define ALL -1

struct t_stats {
    int pid;
    int total_calls;
    int ok_calls;
    int error_calls;
    unsigned long long total_time;
};

#endif
