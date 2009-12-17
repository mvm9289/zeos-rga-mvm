#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/errno.h>

#include "../include/syscallsmon.h"  


int main() {

    char buffer[256];
    struct t_stats stats;
    int pid;
    int res;
    int res2;
    int fd;
    int status = -1;


    printf("\n\nTESTS FOR SYSCALLS CONSULTANT MODULE\n\n");
    printf("My PID is %d.\n\n", getpid());
    printf("Press ENTER to start tests.");
    fflush(stdout);
    read(0, NULL, 1);
    printf("\nTests Runing...\n");

    printf("Opening the device: ");
    fd=open("/dev/consultant",O_RDONLY, 0777);
    if(fd<0){
        printf("---> Error abriendo el dispositivo\n");
        exit(0);
    }

    printf("---> OK\n");

    printf("-Abrir el dispositivo otra vez: ");
    res=open("/dev/consultant",O_RDONLY,0777);
    if(res<0) printf("Error abriendo el dispositivo------>OK\n");

    printf("Enable monitorization: ");
    res=ioctl(fd,4,ALL);
    if(res<0) printf("---> Error activar monitorizacion\n");
    else printf("---> OK\n");


    printf("A right OPEN call + a right CLOSE call:\n");
    res = open("newFile", O_RDWR | O_CREAT, 0777);
    if(res > 0) printf("OK ---> OPEN\n");
    res2 = close(res);
    if(res2 > 0) printf("OK ---> CLOSE\n");

    printf("Reading OPEN statistics: ");
    read(fd,&stats,sizeof(struct t_stats));
    printf("----> OK\nCall statistics:\n");
    printf("OPEN:");
    printf("        Num calls: %d\n",stats.total_calls);
    printf("        Ok calls: %d\n",stats.ok_calls);
    printf("        Bad calls: %d\n",stats.error_calls);
    printf("        Total time: %d\n",stats.total_time);

    printf("Disable WRITE call monitorization: ");
    res=ioctl(fd,5,WRITE_CALL);
    if(res<0) printf("---> Error %d\n", res);    
    else printf("---> OK\n");

    printf("Reset statistics of current process: ");
    res=ioctl(fd,2,0);
    if(res<0) printf("---> ioctl error\n");
    else printf("---> OK\n");

    printf("Reading OPEN statistics again checking the reset: ");
    read(fd,&stats,sizeof(struct t_stats));
    printf("----> Read OK\nCall statistics:\n");
    printf("OPEN:");
    printf("        Num calls: %d\n",stats.total_calls);
    printf("        Ok calls: %d\n",stats.ok_calls);
    printf("        Bad calls: %d\n",stats.error_calls);
    printf("        Total time: %d\n",stats.total_time);

    printf("Changing syscall to WRITE (enabling and desabling monitorization to avoid printfs): ");
    res = ioctl(fd,1,WRITE_CALL);
    if(res < 0) printf("Error changing the syscall to monitorize");    

    printf("Three right WRITE calls: ");

    printf("Enable WRITE call monitorization: ---> OK");
    res=ioctl(fd,4,WRITE_CALL);

    write(1,"Writing...\n",11);
    write(1,"Writing...\n",11);
    write(1,"Writing...\n",11);

    res=ioctl(fd,5,WRITE_CALL);
    printf("Disable WRITE call monitorization: ");
    if(res<0) printf("---> Error %d\n", res);    
    else printf("---> OK\n");
   
    printf("Reading WRITE statistics: ");
    read(fd,&stats,sizeof(struct t_stats));
    printf("----> OK\nCall statistics:\n");
    printf("WRITE:");
    printf("        Num calls: %d\n",stats.total_calls);
    printf("        Ok calls: %d\n",stats.ok_calls);
    printf("        Bad calls: %d\n",stats.error_calls);
    printf("        Total time: %d\n",stats.total_time);
    

    pid = fork();
    if(pid == 0) {

        printf("Changing the process to a new process: ");
        pid = getpid();
        res = ioctl(fd,0,&pid);
        if(res < 0) printf("---> Error\n");
        else {    
            printf("Three right WRITE calls:\n");
            printf("Enable WRITE call monitorization: ---> OK");
            res=ioctl(fd,4,WRITE_CALL);

            write(1,"Writing...\n",11);
            write(1,"Writing...\n",11);
            write(1,"Writing...\n",11);

            res=ioctl(fd,5,WRITE_CALL);
            printf("Disable WRITE call monitorization: ");
            if(res<0) printf("---> Error %d\n", res);    
            else printf("---> OK\n");

            printf("Reading WRITE statistics of the new process: ");
            read(fd,&stats,sizeof(struct t_stats));
            printf("----> OK\nCall statistics:\n");
            printf("WRITE:");
            printf("        Num calls: %d\n",stats.total_calls);
            printf("        Ok calls: %d\n",stats.ok_calls);
            printf("        Bad calls: %d\n",stats.error_calls);
            printf("        Total time: %d\n",stats.total_time);

            printf("Reset all process statistics: ");
            res=ioctl(fd,3,0);
            if(res<0) printf("---> ioctl error\n");
            else printf("---> OK\n");

            printf("Reading WRITE statistics of the new process again checking the general reset: ");
            read(fd,&stats,sizeof(struct t_stats));
            printf("----> OK\nCall statistics:\n");
            printf("WRITE:");
            printf("        Num calls: %d\n",stats.total_calls);
            printf("        Ok calls: %d\n",stats.ok_calls);
            printf("        Bad calls: %d\n",stats.error_calls);
            printf("        Total time: %d\n",stats.total_time);

            printf("Son process dead.\n");
            exit(0);
        } 
    } 
    
    wait(&status);

    printf("Changing the process to the first process: ");
    res = ioctl(fd,0,NULL);
    if(res < 0) printf("---> Error\n");
    else {
        printf("Reading WRITE statistics of the first process again checking the general reset: ");
        read(fd,&stats,sizeof(struct t_stats));
        printf("----> OK\nCall statistics:\n");
        printf("WRITE:");
        printf("        Num calls: %d\n",stats.total_calls);
        printf("        Ok calls: %d\n",stats.ok_calls);
        printf("        Bad calls: %d\n",stats.error_calls);
        printf("        Total time: %d\n",stats.total_time);
    }         

            
        


    // ahora iba a hacer un fork haciendo unos writes, reseteando todo y volviendo a mostrar los writes del hijo, muriendo, sacando al padre del wait y mostrando de nuevo los writes para ver que se han reseteado todos... después ya no se, no había pensado tanto xDD

    printf("Changing syscall to Close: ");
    res = ioctl(fd,1,CLOSE_CALL);
    if(res < 0) printf("Error changing the syscall to monitorize");
    
    printf("Hacemos 1 close incorrecto\n");
    close(-1);

    printf("Lectura estadísticas Close: ");
    read(fd,&stats,sizeof(struct t_stats));
    printf("OK\nEstadísticas close:\n");
    printf("CLOSE:");
    printf("        Numero entrades: %d\n",stats.total_calls);
    printf("        Sortides correctes: %d\n",stats.ok_calls);
    printf("        Sortides incorrectes: %d\n",stats.error_calls);
    printf("        Tiempo total: %d\n\n",stats.total_time);


    printf("Hacemos fork y cambiamos el proceso\n");
    pid = fork();
    if(pid == 0){
        pid = getpid();
        res = ioctl(fd,0,&pid);
        if(res < 0)printf("Error cambio de proceso\n");

        printf("Hacemos un close incorrecto\n");
        close(-1);

        printf("Lectura estadísticas Close: ");
        read(fd,&stats,sizeof(struct t_stats));
        printf("OK\nEstadísticas close:\n");
        printf("Close:");
        printf("        Numero entrades: %d\n",stats.total_calls);
        printf("        Sortides correctes: %d\n",stats.ok_calls);   
        printf("        Sortides incorrectes: %d\n",stats.error_calls);
        printf("        Tiempo total: %d\n\n",stats.total_time);

        exit(0);
    }
    wait(&status);
    printf("Volvemos a cambiar el proceso al inicial: ");
    res=ioctl(fd,0,&pid);
    if(res>=0) printf("OK\n");
    else printf("Error restaurando el proceso padre\n");
    
    printf("Changing syscall to Write: ");
    res = ioctl(fd,1,WRITE_CALL);
    if(res < 0) printf("Error changing the syscall to monitorize");
    
    printf("Hacemos 3 writes.\n");
    write(1,"write:\n",7);
    write(1,"write:\n",7);
    write(1,"write:\n",7);

    printf("Lectura estadísticas Write: ");
    read(fd,&stats,sizeof(struct t_stats));
    printf("OK\nEstadísticas write:\n");
    printf("write:");
    printf("        Numero entrades: %d\n",stats.total_calls);
    printf("        Sortides correctes: %d\n",stats.ok_calls);   
    printf("        Sortides incorrectes: %d\n",stats.error_calls);
    printf("        Tiempo total: %d\n\n",stats.total_time);


    printf("Reset estadísticas proceso actual: ");
    res=ioctl(fd,2,0);
    if(res<0) printf("Error de ioctl\n");
    else printf("OK\n");

    printf("Lectura estadísticas Write después del reset: ");
    read(fd,&stats,sizeof(struct t_stats));
    printf("OK\nEstadísticas write:\n");
    printf("write:");
    printf("        Numero entrades: %d\n",stats.total_calls);
    printf("        Sortides correctes: %d\n",stats.ok_calls);   
    printf("        Sortides incorrectes: %d\n",stats.error_calls);
    printf("        Tiempo total: %d\n\n",stats.total_time);

    close(fd);

    printf("FIN JUEGO DE PRUEBAS");

    exit(0);
}


