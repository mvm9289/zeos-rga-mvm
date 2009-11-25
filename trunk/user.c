#include <libc.h>

int __attribute__ ((__section__(".text.main")))
main(void) {
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /*__asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    /* Uncomment next line to call the initial routine for the test cases */
     runjp();


    /*char buffer[301];
    char buffer2[301];
    int aux;
    int fd;
    int fd2;
    int i;

    for(i = 0; i < 300; i++) {
        buffer[i]=(i%10)+'0';
    }
    buffer[i]='\0';
    switch(fork()) {
        case 0:
        fd = open("asdf", O_RDONLY | O_CREAT);
        if(fd == -1)  {
            write(1, "open1", 5);
            perror();
            while(1);
        }
        fd2 = open("asdf", O_WRONLY);
        if(fd2 == -1)  {
            write(1, "open2", 5);
            perror();
            while(1);
        }
        aux = write(fd2, buffer, strlen(buffer));
        if(aux == -1)  {
            write(1, "write1", 6);
            perror();
            while(1);
        }
        aux = read(fd, buffer2, strlen(buffer));
        if(aux == -1)  {
            write(1, "read1", 5);
            perror();
            while(1);
        }
        aux = write(1, buffer2, strlen(buffer2));
        if(aux == -1)  {
            write(1, "write2", 6);
            perror();
            while(1);
        }
        aux = close(fd2);
        if(aux == -1)  {
            write(1, "close1", 6);
            perror();
            while(1);
        }
        aux = close(fd);
        if(aux == -1)  {
            write(1, "close2", 6);
            perror();
            while(1);
        }
        fd = unlink("asdf");
        if(fd == -1)  {
            write(1, "unlink", 5);
            perror();
            while(1);
        }
        fd = open("asdf", O_RDONLY | O_CREAT);
        if(fd == -1)  {
            write(1, "open3", 5);
            perror();
            while(1);
        }
        aux = read(fd, buffer2, strlen(buffer));
        if(aux == -1)  {
            write(1,"read2", 5);
            perror();
            while(1);
        }else if(aux == 0) write(1,"0 Bytes",7);
        
        break;
    default:
        break;
    }*/
    while(1);
    return 0;
}
