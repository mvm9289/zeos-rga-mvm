/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <mm_address.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

#define FREE 0
#define ALLOC 1

#define INIT 1
#define NOT_INIT 0
#define OWNER 1
#define NOT_OWNER 0

#define STD_QUANTUM 100

#define NR_SEM 10

#include <devices.h>

extern unsigned int set_eoi;
extern unsigned long next_child_pid;
extern unsigned long life;
extern struct list_head runqueue;
extern unsigned int tasks_free;


struct task_struct {
    unsigned long Pid;
    unsigned long PPid;

    unsigned long quantum;
    unsigned long nbtics_cpu;
    unsigned long nbtrans;
    unsigned long remaining_life;

    unsigned long phys_frames[NUM_PAG_DATA];

    unsigned long sems_owner[NR_SEM];

    struct channel channel_table[CTABLE_SIZE];

    unsigned long request_chars_to_keyboard;
    char *buff_location; 

    struct list_head rq_list;

    unsigned long allocation;
};

union task_union {
    struct task_struct task;
    unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

struct protected_task_struct {
    unsigned long task_protect[KERNEL_STACK_SIZE];  /* This field protects the different task_structs, so any acces to this field will generate a PAGE FAULT exeption */
    union task_union t;
};

extern struct protected_task_struct task[NR_TASKS];


struct semaphore {
    int count;

    struct list_head blockqueue;

    unsigned int initialization;
};

extern struct semaphore sems[NR_SEM];

/* System Initialization */

/* Tasks Structs Initialization */
inline void init_tasks();

/* Sems Initialization */
inline void init_sems();

/* Inicialitza les dades del proces inicial */
void init_task0(void);

/* Task Struct Management */
struct task_struct* current();
struct task_struct* list_head_to_task_struct(struct list_head *l);
struct task_struct* search_task(int pid);

/* Scheduling */
void task_switch(union task_union *t);
void scheduler();

#endif  /* __SCHED_H__ */
