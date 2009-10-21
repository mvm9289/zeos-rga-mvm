/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <list.h>
#include <mm.h>
#include <io.h>

unsigned int set_eoi=0;
unsigned long next_child_pid=1;
unsigned long life;
unsigned int tasks_free=NR_TASKS;

LIST_HEAD(runqueue);


struct protected_task_struct task[NR_TASKS]
            __attribute__((__section__(".data.task")));

struct semaphore sems[NR_SEM];

void init_tasks() {
    int i;

    for(i=0; i<NR_TASKS; i++)
        task[i].t.task.allocation=FREE;
}

void init_sems() {
    int i;

    for(i=0; i<NR_SEM; i++)
        sems[i].allocation=FREE;
}

void init_task0(void)
{
    int i;

    /* System Initialization */
    init_tasks();
    init_sems();
    
    /* Initializes paging for task0 adress space */
    initialize_P0_frames();
    set_user_pages();
    set_cr3();

    /* Initializes task0 task_struct */

    task[0].t.task.PPid=0;
    task[0].t.task.Pid=0;
    task[0].t.task.quantum=STD_QUANTUM;
    life=STD_QUANTUM;
    task[0].t.task.nbtics_cpu=0;
    /* Initializes phys frames for task 0 */
    for(i=0; i<NUM_PAG_DATA; i++)
        task[0].t.task.phys_frames[i]=pagusr_table[PAG_LOG_INIT_DATA_P0+i].bits.pbase_addr;
    /* Initializes task0 sems_owner */
    for(i=0; i<NR_SEM; i++)
        task[0].t.task.sems_owner[i]=0;
    

    task[0].t.task.allocation=ALLOC;

    /* Insert task0 in runqueue */
    list_add(&(task[0].t.task.rq_list), &runqueue);
}

struct task_struct* current()
{
	unsigned int esp;
	__asm__ __volatile__ (" movl %%esp, %0" :"=g" (esp));
	return (struct task_struct *)(esp&0xFFFFF000);
}

struct task_struct* list_head_to_task_struct(struct list_head *l)
{
	return list_entry(l, struct task_struct, rq_list);
}

struct task_struct* search_task(int pid) {
    int i;

    for(i=0; i<NR_TASKS; i++) {
        if(task[i].t.task.allocation==ALLOC && task[i].t.task.Pid==pid)
            return &(task[i].t.task);
    }

    return NULL;
}

void task_switch(union task_union *t) {
    int i;

    /* Update TSS */
    tss.esp0=(DWord) &(t->stack[KERNEL_STACK_SIZE]);

    /* Update TP */
    for(i=0; i<NUM_PAG_DATA; i++)
        set_ss_pag((unsigned)(PAG_LOG_INIT_DATA_P0+i), (unsigned)(t->task.phys_frames[i]));

    /* Flush TLB */
    set_cr3();

    /* Update Kernel Stack */
    __asm__ __volatile__ ("movl %0, %%esp" :: "g" ((unsigned long) &(t->stack[KERNEL_STACK_SIZE-16])));

    /* EOI? */
    if(set_eoi) {
        __asm__ ("movb $0x20, %al\n" "outb %al, $0x20");
        set_eoi=0;
    }

    /* Restore Regs */
    __asm__ (
        "popl %ebx\n"
        "popl %ecx\n"
        "popl %edx\n"
        "popl %esi\n"
        "popl %edi\n"
        "popl %ebp\n"
        "popl %eax\n"
        "popl %ds\n"
        "popl %es\n"
        "popl %fs\n"
        "popl %gs" );

    /* Return */
    __asm__ ("iret");
}

void scheduler() { /* Round Robin */
    /* Scheduling */
    union task_union *new_entry;

    if(RR_need_context_switch()) {
        RR_update_runqueue();
        new_entry=RR_next_process();
        RR_update_vars(new_entry);
        task_switch(new_entry);
    }
}

void RR_update_vars(union task_union *t) {
    life=t->task.quantum;
    t->task.nbtrans++;
    t->task.remaining_life=life;
}

unsigned int RR_need_context_switch() {
    --life;
    current()->nbtics_cpu++;
    current()->remaining_life--;
    
    if(life==0) {
        if(list_is_last(runqueue.next, &runqueue)) {
            life=current()->quantum;
            current()->remaining_life=life;
            return 0;
        }
        return 1;
    }
    return 0;
}

void RR_update_runqueue() {
    list_del(&(current()->rq_list));
    list_add_tail(&(current()->rq_list), &runqueue);
}

union task_union *RR_next_process() {
    return (union task_union *) list_head_to_task_struct(runqueue.next);
}
