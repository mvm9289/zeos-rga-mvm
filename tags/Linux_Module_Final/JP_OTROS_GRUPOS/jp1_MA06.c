#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <malloc.h>

void funcion_clone(){}

int main(){
  int pid;
  int fd;
  void **pila_hijo;
  int status;
  char param[10];

  printf("Inici del joc de proves\n");
  printf("PID procés actual: %d\n", getpid());

  pid = fork();
  if(pid == 0){
    sprintf(param, "pid=%d", getppid());
    printf("Insertem el modul syscallsMonitor\n");
    execlp("insmod", "insmod", "syscallsMonitor.ko", param, '\0');
    _exit(-1);
  }
  wait(&status);
  if(status < 0){
    printf("Error a l'insertar el modul\n");
    _exit(0);
  }
  printf("Modul carregat correctament\nPrem enter per continuar\n");
  read(0, param, 1);

  printf("Provem la crida open:\n  1)Open amb exit:");
  fd = open("prova.txt", O_RDWR|O_CREAT);
  if(fd < 0) printf(" Error al realitzar l'open\n");
  else printf(" Correcte\n");
  printf("  2)Open incorrecte:");
  status = open("prova1.txt", O_WRONLY);
  if(status < 0) printf(" Correcte\n");
  else printf(" Error: aquesta crida hauria de ser incorrecte\n");

  printf("Provem la crida write:\n  1)Write amb exit:");
  status = write(fd, "Provant write\n", 14);
  if(status < 0) printf(" Error al realitzar el write\n");
  else printf(" Correcte\n");
  printf("  2)Write incorrecte:");
  status = write(-1, "hola\n", 5);
  if(status < 0) printf(" Correcte\n");
  else printf(" Error: aquesta crida hauria de ser incorrecte\n");

  printf("Provem la crida lseek:\n  1)Lseek amb exit:");
  status = lseek(fd, -2, SEEK_CUR);
  if(status < 0) printf(" Error al realitzar el lseek\n");
  else printf(" Correcte\n");
  printf("  2)Lseek incorrecte:");
  status = lseek(-1, 3, SEEK_CUR);
  if(status < 0) printf(" Correcte\n");
  else printf(" Error: aquesta crida hauria de ser incorrecte\n");

  printf("Provem la crida close:\n  1)Close amb exit:");
  status = close(fd);
  if(status < 0) printf(" Error al realitzar el close\n");
  else printf(" Correcte\n");
  printf("  2)Close incorrecte:");
  status = close(-1);
  if(status < 0) printf(" Correcte\n");
  else printf(" Error: aquesta crida hauria de ser incorrecte\n");

  printf("Provem la crida clone:\n  1)Clone amb exit:");
  pila_hijo = malloc(1000);
  status = clone((void *)funcion_clone,pila_hijo, CLONE_VM|CLONE_FILES, NULL);
  if(status < 0) printf(" Error al realitzar el clone\n");
  else printf(" Correcte\n");
  printf("  2)Clone incorrecte:");
  status = clone((void *)funcion_clone, pila_hijo, CLONE_SIGHAND, NULL);
  if(status < 0) printf(" Correcte\n");
  else printf(" Error: aquesta crida hauria de ser incorrecte\n");

  printf("Resum de resultats:\n");
  printf("sys_call\tnum_entrades\tcorrectes\tincorrectes\n");
  printf("OPEN\t\t2\t\t1\t\t1\n");
  printf("WRITE\t\t29\t\t28\t\t1\n");
  printf("LSEEK\t\t2\t\t1\t\t1\n");
  printf("CLOSE\t\t2\t\t1\t\t1\n");
  printf("CLONE\t\t2\t\t1\t\t1\n");

  wait(&status);

  unlink("prova.txt");

  printf("PROCES BLOQUEJAT!\n");
  printf("Per consultar les estadistiques:\n");
  printf("En un nou terminal, executi 'dmesg' abans de finalitzar el joc de proves\n");
  printf("Per finalitzar el joc de proves, premi ctrl + c\n");
  while(1);
}
