#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/errno.h>

#include "syscallsConsultantJP.h"  


int main() {

    struct t_info stats;
    int pid;
    int res;
    int fd;
    int status = -1;


    printf("\n\nTESTS FOR SYSCALLS CONSULTANT MODULE\n\n");
    printf("My PID is %d.\n\n", getpid());
    printf("Press ENTER to start tests.");
    fflush(stdout);
    read(0, NULL, 1);
    printf("\nTests Runing...\n");

    printf("-Abrir el dispositivo: ");
    fd=open("/dev/consultant",O_RDONLY, 0777);
    if(fd<0){
        printf("Error abriendo el dispositivo\n");
        exit(0);
    }

    printf("OK\n");

    printf("-Abrir el dispositivo otra vez: ");
    res=open("/dev/consultant",O_RDONLY,0777);
    if(res<0) printf("Error abriendo el dispositivo------>OK\n");

    printf("-Activar monitorización: ");
    res=ioctl(fd,4,ALL);
    if(res<0) printf("Error activar monitorizacion\n");


    printf("Hacemos 1 Open incorrecto\n");
    open("hola.txt", O_RDWR, 0777);
    
    printf("Hacemos 1 Open correcto + 1 close correcto\n");
    res = open("newFile", O_RDWR | O_CREAT, 0777);
    close(res);

    printf("-Lectura estadísticas Open: ");
    read(fd,&stats,sizeof(struct t_info));
    printf("OK\nEstadísticas Open:\n");
    printf("OPEN:");
    printf("        Numero entrades: %d\n",stats.num_entrades);
    printf("        Sortides correctes: %d\n",stats.num_sortides_ok);
    printf("        Sortides incorrectes: %d\n",stats.num_sortides_error);
    printf("        Tiempo total: %d\n\n",stats.durada_total);

    printf("Changing syscall to Close: ");
    res = ioctl(fd,1,CLOSE_CALL);
    if(res < 0) printf("Error changing the syscall to monitorize");
    
    printf("Hacemos 1 close incorrecto\n");
    close(-1);

    printf("Lectura estadísticas Close: ");
    read(fd,&stats,sizeof(struct t_info));
    printf("OK\nEstadísticas close:\n");
    printf("CLOSE:");
    printf("        Numero entrades: %d\n",stats.num_entrades);
    printf("        Sortides correctes: %d\n",stats.num_sortides_ok);
    printf("        Sortides incorrectes: %d\n",stats.num_sortides_error);
    printf("        Tiempo total: %d\n\n",stats.durada_total);


    printf("Hacemos fork y cambiamos el proceso\n");
    pid = fork();
    if(pid == 0){
        pid = getpid();
        res = ioctl(fd,0,&pid);
        if(res < 0)printf("Error cambio de proceso\n");

        printf("Hacemos un close incorrecto\n");
        close(-1);

        printf("Lectura estadísticas Close: ");
        read(fd,&stats,sizeof(struct t_info));
        printf("OK\nEstadísticas close:\n");
        printf("Close:");
        printf("        Numero entrades: %d\n",stats.num_entrades);
        printf("        Sortides correctes: %d\n",stats.num_sortides_ok);   
        printf("        Sortides incorrectes: %d\n",stats.num_sortides_error);
        printf("        Tiempo total: %d\n\n",stats.durada_total);

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
    read(fd,&stats,sizeof(struct t_info));
    printf("OK\nEstadísticas write:\n");
    printf("write:");
    printf("        Numero entrades: %d\n",stats.num_entrades);
    printf("        Sortides correctes: %d\n",stats.num_sortides_ok);   
    printf("        Sortides incorrectes: %d\n",stats.num_sortides_error);
    printf("        Tiempo total: %d\n\n",stats.durada_total);


    printf("Reset estadísticas proceso actual: ");
    res=ioctl(fd,2,0);
    if(res<0) printf("Error de ioctl\n");
    else printf("OK\n");

    printf("Lectura estadísticas Write después del reset: ");
    read(fd,&stats,sizeof(struct t_info));
    printf("OK\nEstadísticas write:\n");
    printf("write:");
    printf("        Numero entrades: %d\n",stats.num_entrades);
    printf("        Sortides correctes: %d\n",stats.num_sortides_ok);   
    printf("        Sortides incorrectes: %d\n",stats.num_sortides_error);
    printf("        Tiempo total: %d\n\n",stats.durada_total);

    close(fd);

    printf("FIN JUEGO DE PRUEBAS");

    exit(0);
}


