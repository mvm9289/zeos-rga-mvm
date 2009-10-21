/*
 * sys.c - Syscalls implementation
 */

#include <devices.h>
#include <utils.h>
#include <errno.h>
#include <sched.h>
#include <mm.h>
#include <mm_address.h>
#include <stats.h>

#define W_SIZE 256

int comprova_fd(int fd, int operacio) {
    if(fd != 1) return -EBADF;
    if(operacio != WRITE) return -EINVAL;

    return 0;
}

int sys_ni_syscall() {
    return -ENOSYS;
}

int sys_write(int fd,char *buffer, int size)
{
    char to_write[W_SIZE];
    int bytes=0;
    int err = comprova_fd(fd, WRITE);

    if(err != 0) return err;
    if(!access_ok(READ, buffer, size)) return -EFAULT;
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

int sys_getpid(void)
{
    return current()->Pid;
}


int sys_fork(void)
{
    int tsk;
    int i;
    int fr;

    if(phys_frames_free < NUM_PAG_DATA) return -EAGAIN; //PONER BIEN
    /* Alloc child task struct */
    if(tasks_free) {
        tsk=alloc_task_struct();
        tasks_free--;
    }
    else return -ENOMEM;

    /* Copy System Data: task_union */
    copy_data((void *) current(), (void *) &(task[tsk].t), KERNEL_STACK_SIZE*sizeof(unsigned long));

    /* Copy User Data *///REMIRAR SI PARA HACER LA COMPROVACION ANTES
    /*if(phys_frames_free < NUM_PAG_DATA) {
        dealloc_task_struct(tsk);
        return -EAGAIN;
    }*/
    
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
    task[tsk].t.task.quantum=current()->quantum; //HACE FALTA? YA LO COPIAS CON COPY DATA
    /* Child Nb Trans */
    task[tsk].t.task.nbtrans=0;
    /* Child Nb Tics Cpu */
    task[tsk].t.task.nbtics_cpu=0;
    /* Child sems_owner */
    for(i=0; i<NR_SEM; i++)
        task[tsk].t.task.sems_owner[i]=0;
    /* Child return value */
    task[tsk].t.stack[KERNEL_STACK_SIZE-10]=0;

    /* Insert child in RUNQUEUE */
    list_add_tail(&task[tsk].t.task.rq_list, &runqueue);

    return next_child_pid++;
}


int sys_nice(int quantum)
{
    int old;

    if(quantum <= 0) return -EINVAL;
    old = current()->quantum;
    current()->quantum = quantum;
    return old;
}

int sys_sem_init(int n_sem, unsigned int value) {
    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].allocation==ALLOC) return -EBUSY;

    sems[n_sem].allocation=ALLOC;
    sems[n_sem].count=value;
    INIT_LIST_HEAD(&sems[n_sem].blockqueue);

    current()->sems_owner[n_sem]=1;

    return 0;
}

int sys_sem_wait(int n_sem) {
    union task_union *new_entry;

    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].allocation==FREE) return -EINVAL;

    if(sems[n_sem].count <= 0) {
        if(current()->Pid==0) return -EPERM;

        list_del(&(current()->rq_list));
        list_add_tail(&(current()->rq_list), &sems[n_sem].blockqueue);
        sems[n_sem].count--;
        new_entry=RR_next_process();// ESTO ESTA EN VARIOS SITIOS, ENCAPSULAR??
        RR_update_vars(new_entry);
        task_switch(new_entry);
    }
    else sems[n_sem].count--;

    return 0;
}

int sys_sem_signal(int n_sem) {
    union task_union *t;

    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(sems[n_sem].allocation==FREE) return -EINVAL;

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

    if(sems[n_sem].allocation==FREE) return -EINVAL;
    if(n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
    if(current()->sems_owner[n_sem]==0) return -EPERM;

    while(sems[n_sem].count < 0) {
        t=(union task_union *) list_head_to_task_struct(sems[n_sem].blockqueue.next);
        t->stack[KERNEL_STACK_SIZE-10]=-1;

        list_del(sems[n_sem].blockqueue.next);
        list_add_tail(&(t->task.rq_list), &runqueue);
        
        sems[n_sem].count++;
    }

    sems[n_sem].allocation=FREE;
    current()->sems_owner[n_sem]=0;

    return 0;
}

void sys_exit(void) {
    int i;
    union task_union *new_entry;

    if(current()->Pid==0) return;

    /* Destroy Own Sems */
    for(i=0; i<NR_SEM; i++) {
        if(current()->sems_owner[i])
            sys_sem_destroy(i);//ESTO SE PUEDE??
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
    new_entry = RR_next_process();
    RR_update_vars(new_entry);
    task_switch(new_entry);
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
