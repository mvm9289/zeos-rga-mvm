
#ifndef __SYSCALLS_MON__
#define __SYSCALLS_MON__

#define OPEN_CALL 0
#define CLOSE_CALL 3
#define WRITE_CALL 1
#define LSEEK_CALL 2
#define CLONE_CALL 4
#define ALL -1

#define SWITCH_PROCESS 0
#define SWITCH_SYSCALL 1
#define RESET 2
#define RESET_ALL 3
#define ACTIVATE_MONITOR 4
#define DEACTIVATE_MONITOR 5

struct t_stats {
    int total_calls;
    int ok_calls;
    int error_calls;
    unsigned long long total_time;
};

#endif
