#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/syscallsmon.h"  


int main() {

    char buffer[256];
    struct t_stats stats;
    int pid;
    int res0;
    int res1;
    int fdnull;
    int fd0;
    int fd1;
    int status;


    printf("\n\nTESTS FOR SYSCALLS CONSULTANT MODULE\n\n");
    printf("Press ENTER to start tests.");
    fflush(stdout);
    read(0, NULL, 1);
    printf("\nTests Running...\n");

    fdnull = open("/dev/null", O_WRONLY, 0777);

    printf("\nTEST 1: Open the device.");
    fd0 = open("/dev/consultant", O_RDONLY, 0777);
    if(fd0 < 0){
        printf("----->Result BAD\n");
        printf("ERROR: Can not open the device.");
        exit(0);
    }
    printf("----->Result OK\n");

    printf("\nTEST 2: Open the device again checking that's not permitted.");
    fd1 = open("/dev/consultant", O_RDONLY, 0777);
    if(fd1 < 0) printf("----->Result OK\n");
    else {
        printf("----->Result BAD\n");
        printf("ERROR: Can open the device.\n\n");
        exit(0);
    }

    printf("\nTEST 3: Disable monitorization.");
    res0 = ioctl(fd0, DEACTIVATE_MONITOR, ALL);
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not disable monitorization.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 4: Reset statistics of all processes.");
    res0 = ioctl(fd0, RESET_ALL, 0);
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not reset statistics of all processes.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 5: Enable OPEN monitorization and select it.");
    res0 = ioctl(fd0, ACTIVATE_MONITOR, OPEN_CALL);
    res1 = ioctl(fd0, SWITCH_SYSCALL, OPEN_CALL);
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not activate the monitorization.\n\n");
        exit(0);
    }
    else if(res1 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not switch the syscall.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 6: A right OPEN call.");
    fd1 = open("newfile", O_RDWR | O_CREAT, 0777);
    read(fd0, &stats, sizeof(struct t_stats));
    if(fd1 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not create a file.\n\n");
        exit(0);
    }
    else if(stats.total_calls != 1 || stats.ok_calls != 1 || stats.error_calls != 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Stats do not match the results.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 7: Enable CLOSE monitorization and select it.");
    res0 = ioctl(fd0, ACTIVATE_MONITOR, CLOSE_CALL);
    res1 = ioctl(fd0, SWITCH_SYSCALL, CLOSE_CALL);
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not activate the monitorization.\n\n");
        exit(0);
    }
    else if(res1 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not switch the syscall.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 8: A right CLOSE call.");
    res0 = close(fd1);
    read(fd0, &stats, sizeof(struct t_stats));
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not close the fd.\n\n");
        exit(0);
    }
    else if(stats.total_calls != 1 || stats.ok_calls != 1 || stats.error_calls != 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Stats do not match the results.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    unlink("newfile");

    printf("\nTEST 9: Reset statistics of current process.");
    res0 = ioctl(fd0, RESET, 0);
    read(fd0, &stats, sizeof(struct t_stats));
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not reset the statistics of current process\n\n");
        exit(0);
    }
    else if(stats.total_calls != 0 || stats.ok_calls != 0 || stats.error_calls != 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Stats do not match the results.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 10: Enable WRITE monitorization and select it.");
    res0 = ioctl(fd0, ACTIVATE_MONITOR, WRITE_CALL);
    res1 = ioctl(fd0, SWITCH_SYSCALL, WRITE_CALL);
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not activate the monitorization.\n\n");
        exit(0);
    }
    else if(res1 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not switch the syscall.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 11: Three right WRITE calls.");
    write(fdnull, "WRITE", 5);
    write(fdnull, "WRITE", 5);
    write(fdnull, "WRITE", 5);
    read(fd0, &stats, sizeof(struct t_stats));
    if(stats.total_calls != 5 || stats.ok_calls != 5 || stats.error_calls != 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Stats do not match the results.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nTEST 12: Create a child process that do three wrong OPEN calls.");
    fflush(stdout);
    pid = fork();
    if(pid == 0) {
        pid = getpid();
        fd1 = open("newfile", O_RDWR, 0777);
        fd1 = open("newfile", O_RDWR, 0777);
        fd1 = open("newfile", O_RDWR, 0777);
        res0 = ioctl(fd0, SWITCH_PROCESS, &pid);
        res1 = ioctl(fd0, SWITCH_SYSCALL, OPEN_CALL);
        read(fd0, &stats, sizeof(struct t_stats));
        if(res0 < 0) {
            printf("----->Result BAD\n");
            printf("ERROR: Can not switch the process.\n\n");
            exit(0);
        }
        else if(res1 < 0) {
            printf("----->Result BAD\n");
            printf("ERROR: Can not switch the syscall.\n\n");
            exit(0);
        }
        if(stats.total_calls != 3 || stats.ok_calls != 0 || stats.error_calls != 3) {
            printf("----->Result BAD\n");
            printf("ERROR: Stats do not match the results.\n\n");
        }
        else printf("----->Result OK\n");
        exit(0);
    }
    wait(&status);

    printf("\nTEST 13: Change the process monitorization to the fisrt process.");
    res0 = ioctl(fd0, SWITCH_PROCESS, NULL);
    if(res0 < 0) {
        printf("----->Result BAD\n");
        printf("ERROR: Can not switch the process.\n\n");
        exit(0);
    }
    else printf("----->Result OK\n");

    printf("\nALL TESTS PASSED\n");

    close(fd0);
    exit(0);
    // ahora iba a hacer un fork haciendo unos writes, reseteando todo y volviendo a mostrar los writes del hijo, muriendo, sacando al padre del wait y mostrando de nuevo los writes para ver que se han reseteado todos... después ya no se, no había pensado tanto xDD
}


