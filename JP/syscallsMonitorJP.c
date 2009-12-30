#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PRINTFS 44

void do_nothing() {}

int main() {
    char buffer[256];
    void **stack;

    int good_opens = 0;
    int bad_opens = 0;
    int good_closes = 0;
    int bad_closes = 0;
    int good_writes = 0;
    int bad_writes = 0;
    int good_lseeks = 0;
    int bad_lseeks = 0;
    int good_clones = 0;
    int bad_clones = 0;

    int fd0;
    int fd1;
    int fd2;

    int i;
    unsigned int bad_result = 0;

    printf("\n\nTESTS FOR SYSCALLS MONITOR MODULE\n\n");
    printf("My PID is %d.\n\n", getpid());
    printf("Press ENTER to start tests.");
    fflush(stdout);
    read(0, NULL, 1);
    printf("\nTests Running...\n");

    printf("\nTEST 1: Open a non exist file with result OK.");
    fd1 = open("newfile", O_RDWR | O_CREAT | O_EXCL, 0777);
    if (fd1 == -1) {
        printf("----->Execution BAD\n");
        perror(buffer);
        printf(buffer);
        good_writes++;
        bad_opens++;
    }
    else {
        printf("----->Execution OK\n");
        good_opens++;
    }
    
    printf("\nTEST 2: Open an already exist file with result OK.");
    fd1 = open("newfile", O_RDWR, 0777);
    fd0 = fd1;
    if (fd1 == -1) {
        printf("----->Execution BAD\n");
        perror(buffer);
        printf(buffer);
        good_writes++;
    }
    else {
        printf("----->Execution OK\n");
        good_opens++;
    }

    printf("\nTEST 3: Open an already exist file with result BAD.");
    fd1 = open("newfile", O_RDWR | O_CREAT | O_EXCL, 0777);
    if (fd1 == -1) {
        printf("----->Execution OK\n");
        bad_opens++;
    }
    else {
        printf("----->Execution BAD\n");
        good_opens++;
    }

    printf("\nTEST 4: Open a non exist file with result BAD.");
    fd1 = open("newfile2", O_RDWR, 0777);
    if (fd1 == -1) {
        printf("----->Execution OK\n");
        bad_opens++;
    }
    else {
        printf("----->Execution BAD\n");
        good_opens++;
    }

    for (i = 0; i < 5; i++) {
        fd1 = dup(2);
        fd2 = close(fd1);
        if (fd2 == -1) {
            perror(buffer);
            printf(buffer);
            bad_closes++;
            good_writes += 5;
            bad_result = 1;
        }
        else good_closes++;
    }
    printf("\nTEST 5: Close an exist fd with result OK. (x5)");
    if (bad_result) printf("----->Execution BAD\n");
    else printf("----->Execution OK\n");
    bad_result = 0;

    fd1 = -1;
    for (i = 0; i < 5; i++) {
        fd2 = close(fd1);
        if (fd2 == -1) {
            bad_closes++;
            bad_result = 1;
        }
        else good_closes++;
    }
    printf("\nTEST 6: Close a non exist fd with result BAD.(x5)");
    if (bad_result) printf("----->Execution OK\n");
    else printf("----->Execution BAD\n");
    bad_result = 0;

    for (i = 0; i < 5; i++) {
        fd1 = write(-1, "\nTHIS SHOULD NOT SHOW\n", 20);
        if (fd1 == -1) {
            bad_writes++;
            bad_result = 1;
        }
        else good_writes++;
    }
    printf("\nTEST 7: Write a non exist fd with result BAD.(x5)");
    if (bad_result) printf("----->Execution OK\n");
    else printf("----->Execution BAD\n");
    bad_result = 0;

    for (i = 0; i < 5; i++) {
        fd1 = lseek(fd0, 0, SEEK_END);
        if (fd1 == -1) {
            perror(buffer);
            printf(buffer);
            bad_lseeks++;
            good_writes += 5;
            bad_result = 1;
        }
        else good_lseeks++;
    }
    printf("\nTEST 8: Lseek an exist fd with result OK.(x5)");
    if (bad_result) printf("----->Execution BAD\n");
    else printf("----->Execution OK\n");
    bad_result = 0;

    for (i = 0; i < 5; i++) {
        fd1 = lseek(-1, 0, SEEK_END);
        if (fd1 == -1) {
            bad_lseeks++;
            bad_result = 1;
        }
        else good_lseeks++;
    }
    printf("\nTEST 9: Lseek a non exist fd with result BAD.(x5)");
    if (bad_result) printf("----->Execution OK\n");
    else printf("----->Execution BAD\n");
    bad_result = 0;

    for (i = 0; i < 5; i++) {
        fd1 = clone((void *)do_nothing, stack, CLONE_VM | CLONE_FILES, NULL);
        if (fd1 == -1) {
            perror(buffer);
            printf(buffer);
            bad_clones++;
            good_writes += 5;
            bad_result = 1;
        }
        else good_clones++;
    }
    printf("\nTEST 10: Clone with result OK.(x5)");
    if (bad_result) printf("----->Execution BAD\n");
    else printf("----->Execution OK\n");
    bad_result = 0;

    for (i = 0; i < 5; i++) {
        fd1 = clone((void *)do_nothing, stack, CLONE_SIGHAND, NULL);
        if (fd1 == -1) {
            bad_clones++;
            bad_result = 1;
        }
        else good_clones++;
    }
    printf("\nTEST 11: Clone with result BAD.(x5)");
    if (bad_result) printf("----->Execution OK\n");
    else printf("----->Execution BAD\n");
    bad_result = 0;

    unlink("newfile");

    printf("\n\nEXPECTED RESULTS OF MODULE:\n");
    printf("\nOPEN:\n");
    printf("Total calls: %d\n", good_opens+bad_opens);
    printf("Good calls: %d\n", good_opens);
    printf("Bad calls: %d\n", bad_opens);
    printf("\nCLOSE:\n");
    printf("Total calls: %d\n", good_closes+bad_closes);
    printf("Good calls: %d\n", good_closes);
    printf("Bad calls: %d\n", bad_closes);
    printf("\nWRITE:\n");
    printf("Total calls: %d\n", good_writes+bad_writes+PRINTFS);
    printf("Good calls: %d\n", good_writes+PRINTFS);
    printf("Bad calls: %d\n", bad_writes);
    printf("\nLSEEK:\n");
    printf("Total calls: %d\n", good_lseeks+bad_lseeks);
    printf("Good calls: %d\n", good_lseeks);
    printf("Bad calls: %d\n", bad_lseeks);
    printf("\nCLONE:\n");
    printf("Total calls: %d\n", good_clones+bad_clones);
    printf("Good calls: %d\n", good_clones);
    printf("Bad calls: %d\n", bad_clones);

    while(1);
}
