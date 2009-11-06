/*
 * sys.c - Syscalls implementation
 */
#include <sys.h>
#include <devices.h>
#include <utils.h>
#include <errno.h>
#include <sched.h>
#include <mm.h>
#include <mm_address.h>
#include <stats.h>

int comprova_fd(int fd, int operacio) {
    if(fd != 1) return -EBADF;
    if(operacio != WRITE) return -EINVAL;

    return 0;
}

int sys_ni_syscall() {
    return -ENOSYS;
}

int sys_write(int fd, char *buffer, int size) {
    char to_write[W_SIZE];
    int bytes=0;
    int err = comprova_fd(fd, WRITE);

    if(err != 0) return err;
    if(!access_ok(READ, buffer, size)) return -EFAULT;
    if(size < 0) return -EINVAL;

    while (size > W_SIZE) {
        copy_from_user(buffer+bytes, to_write, W_SIZE);
        bytes += file_operations[current()->channel_table[fd].log_device->ops_indx].sys_write_dev(0, to_write, W_SIZE);
        size -= W_SIZE;
    }
    copy_from_user(buffer+bytes, to_write, size);
    bytes += file_operations[current()->channel_table[fd].log_device->ops_indx].sys_write_dev(0, to_write, size);

    return bytes;
}

int sys_getpid(void) {
    return current()->Pid;
}

int sys_fork(void) {
    int tsk;
    int i;
    int fr;

    /* Sufficient Space? */
    if(!tasks_free) return -ENOMEM;
    if(phys_frames_free < NUM_PAG_DATA) return -EAGAIN;

    /* Alloc child task struct */
    tsk=alloc_task_struct();
    tasks_free--;

    /* Copy System Data: task_union */
    copy_data((void *) current(), (void *) &(task[tsk].t), KERNEL_STACK_SIZE*sizeof(unsigned long));

    /* Copy User Data */
    phys_frames_free-=NUM_PAG_DATA;
    for(i=0; i<NUM_PAG_DATA; i++) {
        /* Frame allocation */
        fr=alloc_frame();

        /* Save new frame on child task_struct */
        task[tsk].t.task.phys_frames[i]=fr;

        /* Copy Data Page */
        set_ss_pag(PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA, fr); // Set AUX page
        set_cr3(); // Flush TLB
        copy_data((void *) ((PAG_LOG_INIT_DATA_P0+i)<<12), (void *) ((PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA)<<12), PAGE_SIZE);
    }
    del_ss_pag(PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA); // Delete AUX page
    set_cr3(); // Flush TLB

    /* Initialize child task_struct */

    /* New PID for child process */
    task[tsk].t.task.Pid=next_child_pid;
    /* Child PPid */
    task[tsk].t.task.PPid=current()->Pid;
    /* Child Quantum */
    task[tsk].t.task.quantum=current()->quantum;
    /* Child Nb Trans */
    task[tsk].t.task.nbtrans=0;
    /* Child Nb Tics Cpu */
    task[tsk].t.task.nbtics_cpu=0;
    /* Child Remaining Tics */
    task[tsk].t.task.remaining_life=0;
    /* Child sems_owner */
    for(i=0; i<NR_SEM; i++)
        task[tsk].t.task.sems_owner[i]=NOT_OWNER;
    /* Child return value */
    task[tsk].t.stack[KERNEL_STACK_SIZE-10]=0;
    /* Child CT */
    for(i=0; i<CTABLE_SIZE; i++) {
        if(!task[tsk].t.task.channel_table[i].free)
            OFT[task[tsk].t.task.channel_table[i].OFT_indx].num_refs++;
    }

    /* Insert child in RUNQUEUE */
    list_add_tail(&task[tsk].t.task.rq_list, &runqueue);

    return next_child_pid++;
}

int sys_nice(int quantum) {
    int old;

    if(quantum <= 0) return -EINVAL;

    old = current()->quantum;
    current()->quantum = quantum;

    return old;
}

int sys_sem_init(int n_sem, unsigned int value) {
    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].initialization==INIT) return -EBUSY;

    sems[n_sem].initialization=INIT;
    sems[n_sem].count=value;
    INIT_LIST_HEAD(&sems[n_sem].blockqueue);

    current()->sems_owner[n_sem]=OWNER;

    return 0;
}

int sys_sem_wait(int n_sem) {
    union task_union *t;

    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].initialization==NOT_INIT) return -EINVAL;
    if(current()->Pid==0) return -EPERM;

    sems[n_sem].count--;
    if(sems[n_sem].count < 0) {
        list_del(&(current()->rq_list));
        list_add_tail(&(current()->rq_list), &sems[n_sem].blockqueue);

        t=(union task_union *) list_head_to_task_struct(runqueue.next);
        life=t->task.quantum;
        t->task.nbtrans++;
        t->task.remaining_life=life;
        task_switch(t);
    }

    return 0;
}

int sys_sem_signal(int n_sem) {
    union task_union *t;

    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].initialization==NOT_INIT) return -EINVAL;

    sems[n_sem].count++;
    if(sems[n_sem].count <= 0) {
        t=(union task_union *) list_head_to_task_struct(sems[n_sem].blockqueue.next);
        t->stack[KERNEL_STACK_SIZE-10]=0;

        list_del(sems[n_sem].blockqueue.next);
        list_add_tail(&(t->task.rq_list), &runqueue);
    }

    return 0;
}

int sys_sem_destroy(int n_sem) {
    union task_union *t;

    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].initialization==NOT_INIT) return -EINVAL;
    if(current()->sems_owner[n_sem]==NOT_OWNER) return -EPERM;

    while(sems[n_sem].count < 0) {
        t=(union task_union *) list_head_to_task_struct(sems[n_sem].blockqueue.next);
        t->stack[KERNEL_STACK_SIZE-10]=-1;

        list_del(sems[n_sem].blockqueue.next);
        list_add_tail(&(t->task.rq_list), &runqueue);
        
        sems[n_sem].count++;
    }

    sems[n_sem].initialization=NOT_INIT;
    current()->sems_owner[n_sem]=NOT_OWNER;

    return 0;
}

void sys_exit(void) {
    int i;
    union task_union *t;

    if(current()->Pid==0) return;

    /* Destroy Own Sems */
    for(i=0; i<NR_SEM; i++) {
        if(current()->sems_owner[i])
            sys_sem_destroy(i);
    }

    /* Free Phys Frames of Current Process */
    for(i=0; i < NUM_PAG_DATA; ++i) {
        free_frame(current()->phys_frames[i]);
        phys_frames_free++;
    }

    /* Free task_struct of Current Process */
    current()->allocation = FREE;
    tasks_free++;
    
    /* Select next process to entry on CPU */
    list_del(&(current()->rq_list));
    t=(union task_union *) list_head_to_task_struct(runqueue.next);
    life=t->task.quantum;
    t->task.nbtrans++;
    t->task.remaining_life=life;
    task_switch(t);
}

int sys_getstats(int pid, struct stats *st) {
    struct task_struct *tsk;
    struct stats stt;

    if(pid < 0) return -EINVAL;
    if(!access_ok(WRITE, st, sizeof(struct stats)))
        return -EFAULT;

    tsk=search_task(pid);
    if(!tsk) return -ESRCH;

    stt.total_tics=tsk->nbtics_cpu;
    stt.total_trans=tsk->nbtrans;
    stt.remaining_tics=tsk->remaining_life;

    copy_to_user((void *) &stt, (void *)st, sizeof(struct stats));

    return 0;
}
