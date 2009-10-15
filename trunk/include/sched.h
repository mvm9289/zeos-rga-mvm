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

#define STD_QUANTUM 69

extern unsigned int set_eoi;
extern unsigned long next_child_pid;
extern unsigned long life;
extern struct list_head runqueue;

extern unsigned int tasks_free;



struct task_struct* current();

struct task_struct* list_head_to_task_struct(struct list_head *l);

struct task_struct {
    unsigned long Pid;
    unsigned long PPid;

    unsigned long quantum;
    unsigned long nbtics_cpu;
    unsigned long nbtrans;
    unsigned long remaining_life;

    unsigned long phys_frames[NUM_PAG_DATA];

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

/* Inicialitza les dades del proces inicial */
void init_task0(void);

/* Scheduling */
void task_switch(union task_union *t);
void scheduler();

/* Round Robin */
void RR_update_vars(union task_union *t);
unsigned int RR_need_context_switch();
void RR_update_runqueue();
union task_union *RR_next_process();

#endif  /* __SCHED_H__ */
