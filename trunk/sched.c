/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <list.h>
#include <mm.h>
#include <devices.h>
#include <openmodes.h>

unsigned int set_eoi=0;
unsigned long next_child_pid=0;
unsigned long life;
unsigned int tasks_free=NR_TASKS;

LIST_HEAD(runqueue);


struct protected_task_struct task[NR_TASKS]
            __attribute__((__section__(".data.task")));

struct semaphore sems[NR_SEM];

inline void init_tasks() {
    int i;

    for(i=0; i<NR_TASKS; i++)
        task[i].t.task.allocation=FREE;
}

inline void init_sems() {
    int i;

    for(i=0; i<NR_SEM; i++)
        sems[i].initialization=NOT_INIT;
}

void init_task0(void) {
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
    task[0].t.task.request_chars_to_keyboard=0;
    /* Initializes phys frames for task 0 */
    for(i=0; i<NUM_PAG_DATA; i++)
        task[0].t.task.phys_frames[i]=pagusr_table[PAG_LOG_INIT_DATA_P0+i].bits.pbase_addr;
    /* Initializes task0 sems_owner */
    for(i=0; i<NR_SEM; i++)
        task[0].t.task.sems_owner[i]=NOT_OWNER;
    /* Initializes CT */
    for(i=3; i<CTABLE_SIZE; i++)
        task[0].t.task.channel_table[i].free=1;
    task[0].t.task.channel_table[0].opened_file = &OFT[0];
    task[0].t.task.channel_table[0].free=0;
    task[0].t.task.channel_table[1].opened_file = &OFT[1];
    task[0].t.task.channel_table[1].free=0;
    task[0].t.task.channel_table[2].opened_file = &OFT[2];
    task[0].t.task.channel_table[2].free=0;
    /* Update Opened Files Table */
    OFT[0].num_refs=1;
    OFT[0].seq_pos=0;
    OFT[0].init_access_mode=O_RDONLY;
    OFT[0].file = &DIR[0];
    DIR[0].nb_refs++;
    OFT[1].num_refs=1;
    OFT[1].seq_pos=0;
    OFT[1].init_access_mode=O_WRONLY;
    OFT[1].file = &DIR[1];///////////////////////MIRAR DE HACER CON OPENS
    DIR[1].nb_refs++;
    OFT[2].num_refs=1;
    OFT[2].seq_pos=0;
    OFT[2].init_access_mode=O_WRONLY;
    OFT[2].file = &DIR[1];
    DIR[1].nb_refs++;

    /* Mark this task is allocate */
    task[0].t.task.allocation=ALLOC;
    tasks_free--;

    /* Insert task0 in runqueue */
    list_add(&(task[0].t.task.rq_list), &runqueue);
}

struct task_struct* current() {
	unsigned int esp;
	__asm__ __volatile__ (" movl %%esp, %0" :"=g" (esp));
	return (struct task_struct *)(esp&0xFFFFF000);
}

struct task_struct* list_head_to_task_struct(struct list_head *l) {
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

void scheduler() { /* Scheduling: Round Robin */
    union task_union *t;

    --life;
    current()->nbtics_cpu++;
    current()->remaining_life=life;

    if(!life) {
        if(list_is_last(runqueue.next, &runqueue)) {
            life=current()->quantum;
            current()->remaining_life=life;
            return;
        }
        list_del(&(current()->rq_list));
        list_add_tail(&(current()->rq_list), &runqueue);
        t=(union task_union *) list_head_to_task_struct(runqueue.next);
        life=t->task.quantum;
        t->task.nbtrans++;
        t->task.remaining_life=life;
        task_switch(t);
    }
}
