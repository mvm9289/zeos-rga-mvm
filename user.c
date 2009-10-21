#include <libc.h>

int __attribute__ ((__section__(".text.main")))
main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /*__asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    /* Uncomment next line to call the initial routine for the test cases */
     runjp(); 
    /*char buffer[128];
    int p1;
    struct stats stt;

    if(sem_init(0, 0)==-1) {
        write(1, "\n", 1);
        perror();
        while(1);
    }
    else write(1, "\nSemI OK!", 9);
    if(sem_init(1, 0)==-1) {
        write(1, "\n", 1);
        perror();
        while(1);
    }
    else write(1, "\nSemI OK!", 9);
    if(sem_init(2, 0)==-1) {
        write(1, "\n", 1);
        perror();
        while(1);
    }
    else write(1, "\nSemI OK!", 9);
    p1=fork();
    switch(p1) {
        case -1:
            write(1, "\nError", 6);
            break;
        case 0:
            p1=fork();
            switch(p1) {
                case -1:
                    write(1, "\nError", 6);
                    break;
                case 0:
while(1) {
    if(sem_wait(0)==0) write(1, "\nSemW OK!", 9);
                    itoa(getpid(), buffer);
                    write(1, "\nMy PID is ", 11);
                    write(1, buffer, strlen(buffer));
                    if(get_stats(getpid(), &stt)==-1) {
                        write(1, "\n", 1);
                        perror();
                    }
                    else {
                        itoa(stt.total_tics, buffer);
                        write(1, "\nTotal Tics: ", 13);
                        write(1, buffer, strlen(buffer));
                        itoa(stt.total_trans, buffer);
                        write(1, "\nTotal Trans: ", 14);
                        write(1, buffer, strlen(buffer));
                        itoa(stt.remaining_tics, buffer);
                        write(1, "\nRemaining Tics: ", 17);
                        write(1, buffer, strlen(buffer));                        
                    }
    if(sem_signal(1)==0) write(1, "\nSemS OK!", 9);
                    write(1, "\n", 1);
}
                    break;
                default:
while(1) {
    if(sem_wait(1)==0) write(1, "\nSemW OK!", 9);
                    itoa(getpid(), buffer);
                    write(1, "\nMy PID is ", 11);
                    write(1, buffer, strlen(buffer));
                    itoa(p1, buffer);
                    write(1, "\nI am the parent of ", 20);
                    write(1, buffer, strlen(buffer));
                    itoa(nice(240), buffer);
                    write(1, "\nMy old quantum is ", 19);
                    write(1, buffer, strlen(buffer));
                    write(1, "\nI do exit()", 12);
    if(sem_signal(2)==0) write(1, "\nSemS OK!", 9);
                    write(1, "\n", 1);
                    exit();
}
                break;
            }
            break;
        default:
while(1) {
            itoa(getpid(), buffer);
            write(1, "\nMy PID is ", 11);
            write(1, buffer, strlen(buffer));
            itoa(p1, buffer);
            write(1, "\nI am the parent of ", 20);
            write(1, buffer, strlen(buffer));
            if(nice(-5)==-1) {
                write(1, "\n", 1);
                perror();
            }
    if(sem_signal(0)==0) write(1, "\nSemS OK!", 9);
            write(1, "\n", 1);
    if(sem_wait(2)==0) write(1, "\nSemW OK!", 9);
}
            break;
    }*/

    while(1);
    return 0;
}
