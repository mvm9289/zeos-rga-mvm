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


void sys_exit(void)
{
}

int sys_fork(void)
{
    int tsk;
    int i;
    int fr;

    /* Alloc child task struct */
    if(tasks_free) {
        tsk=alloc_task_struct();
        tasks_free--;
    }
    else return -ENOMEM;

    /* Copy System Data: task_union */
    copy_data((void *) current(), (void *) &(task[tsk].t), KERNEL_STACK_SIZE*sizeof(unsigned long));

    /* Copy User Data */
    if(phys_frames_free < NUM_PAG_DATA) {
        dealloc_task_struct(tsk);
        return -EAGAIN;
    }
    
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
    task[tsk].t.task.quantum=STD_QUANTUM;
    /* Child Nb Tics Cpu */
    task[tsk].t.task.nbtics_cpu=0;
    /* Child return value */
    task[tsk].t.stack[KERNEL_STACK_SIZE-10]=0;

    /* Insert child in RUNQUEUE */
    list_add_tail(&task[tsk].t.task.rq_list, &runqueue);

    return next_child_pid++;
}


int sys_nice(int quantum)
{
    int old;

    if(quantum <= 0) return -EPERM;
    old = current()->quantum;
    current()->quantum = quantum;
    return old;
}

int sys_getstats(int pid, struct stats *st) {
    struct task_struct *tsk;
    struct stats stt;

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

