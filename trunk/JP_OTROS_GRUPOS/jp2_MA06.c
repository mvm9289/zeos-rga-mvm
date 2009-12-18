#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sched.h>
#include <malloc.h>
#include "jp2_MA06.h"

char names_syscalls[5][5] = { 'O','P','E','N','\0',
			'C','L','O','S','E',
			'W','R','I','T','E',
			'L','S','E','E','K',
			'C','L','O','N','E'};

void funcion_clone(){}

void hijo1(){
  int fd;
  void **pila_hijo;
  printf("Fill 1: 1 open, 2 write, 1 lseek, 1 close, 1 clone; totes correctes\n");
  fd = open("hijo1.txt", O_RDWR|O_CREAT);
  write(fd, "hola", 4);
  lseek(fd, -2, SEEK_CUR);
  pila_hijo = malloc(1000);
  clone((void *)funcion_clone,pila_hijo, CLONE_VM|CLONE_FILES, NULL);
  while(1);
}

void hijo2(){
  int fd;
  void **pila_hijo;
  printf("Fill 2: 1 open, 3 write, 1 lseek, 1 close, 1 clone\n");
  printf("2 write i 1 close correctes, totes les altres incorrectes\n");
  fd = open("hijo2.txt", O_RDWR);
  write(fd, "hola", 4);
  lseek(fd, -2, SEEK_CUR);
  pila_hijo = malloc(1000);
  clone((void *)funcion_clone, pila_hijo, CLONE_SIGHAND, NULL);
  while(1);
}

void hijo3(){
  int fd;
  void **pila_hijo;
  printf("Fill 3: 1 open, 3 write, 1 lseek, 1 close, 1 clone\n");
  printf("2 write, 1 close i 1 clone correctes, totes les altres incorrectes\n");
  fd = open("hijo2.txt", O_RDWR);
  write(fd, "hola", 4);
  lseek(fd, -2, SEEK_CUR);
  pila_hijo = malloc(1000);
  clone((void *)funcion_clone,pila_hijo, CLONE_VM|CLONE_FILES, NULL);
  while(1);
}

void hijo4(){
  printf("Fill 4: 1 write i 1 close correctes\n");
  while(1);
}

void hijo5(){
  printf("Fill 5: 1 write i 1 close correctes\n");
  while(1);
}

void hijo6(){
  int fd;
  void **pila_hijo;
  printf("Fill 6: 1 open, 2 write, 1 lseek, 2 close, 1 clone; totes correctes\n");
  fd = open("hijo6.txt", O_RDWR|O_CREAT);
  write(fd, "hola", 4);
  lseek(fd, -2, SEEK_CUR);
  pila_hijo = malloc(1000);
  clone((void *)funcion_clone,pila_hijo, CLONE_VM|CLONE_FILES, NULL);
  close(fd);
  while(1);
}

void hijo7(){
  int fd;
  void **pila_hijo;
  printf("Fill 7: 1 open, 2 write, 1 lseek, 2 close, 1 clone; totes correctes\n");
  fd = open("hijo7.txt", O_RDWR|O_CREAT);
  write(fd, "hola", 4);
  lseek(fd, -2, SEEK_CUR);
  pila_hijo = malloc(1000);
  clone((void *)funcion_clone,pila_hijo, CLONE_VM|CLONE_FILES, NULL);
  close(fd);
  while(1);
}


void inserta_moduls(){
  int pid;
  int status;
  char param[10];

  pid = fork();
  if(pid == 0){
    sprintf(param, "pid=%d", getppid());
    printf("Insertem el modul syscallsMonitor\n");
    execlp("insmod", "insmod", "syscallsMonitor.ko", param, '\0');
  }
  wait(&status);

  pid = fork();
  if(pid == 0){
    printf("Creem el dispositiu\n");
    execlp("mknod", "mknod", "/dev/consultant", "c", "256", "0", '\0');
  }
  wait(&status);

  pid = fork();
  if(pid == 0){
    printf("Insertem el modul syscallsConsultant\n");
    execlp("insmod", "insmod", "syscallsConsultant.ko", '\0');
  }
  wait(&status);
}

void pinta_info(struct t_info *info){
  printf("num_entrades: %d\n", info->num_entrades);
  printf("num_sortides_ok: %d\n", info->num_sortides_ok);
  printf("num_sortides_error: %d\n", info->num_sortides_error);
  printf("durada_total: %lld\n", info->durada_total);
}

void pinta_totes_estadistiques(int fd){
  int i;
  char buf[sizeof(struct t_info)];
  struct t_info* info;

  for(i=OPEN; i<=CLONE; i++){
    write(1, &names_syscalls[i][0], 5);
    printf("\n");
    ioctl(fd, 1, i);
    if(read(fd, buf, sizeof(struct t_info)) < 0) printf("Error, crida no monitoritzada. Comportament correcte\n");
    else{
  	info = (struct t_info *)buf;
	pinta_info(info);
    }
  }
}


int main(){
  int fd;
  int fd2;
  int pids[7];
  struct t_info* info;
  char buf[sizeof(struct t_info)];

  //insertem els moduls
  inserta_moduls();


  printf("Inici del joc de proves\n");
  
  fd = open("/dev/consultant", O_RDONLY);
  if(fd < 0){
    printf("\nError a l'obrir el dispositiu\n\n");
    printf("\nFi joc de proves\n\n");
    printf("Per finalitzar el joc de proves, premi ctrl + c\n");
    while(1);
  }

  pids[0] = fork();
  if(pids[0] == 0){
    close(fd);
    hijo1();
  }

  sleep(4);

  printf("TEST 1: Monitoritzem fill 1 i consultem totes les seves estadistiques:\n");
  ioctl(fd, 0, &pids[0]);

  pinta_totes_estadistiques(fd);

  printf("Fem reset de les estadistiques del fill 1\n");
  ioctl(fd, 2);

  pinta_totes_estadistiques(fd);

  printf("\nFI TEST 1\n\n");

  printf("TEST 2: Creem 2 fills i els monitoritzem:\n");
  pids[1] = fork();
  if(pids[1] == 0){
    close(fd);
    hijo2();
  }
  pids[2] = fork();
  if(pids[2] == 0){
    close(fd);
    hijo3();
  }

  sleep(4);

  printf("\nFILL 2:\n");
  ioctl(fd, 0, &pids[1]);
  pinta_totes_estadistiques(fd);

  printf("\nFILL 3:\n");
  ioctl(fd, 0, &pids[2]);
  pinta_totes_estadistiques(fd);

  printf("\nFem reset de tots els processos que s'estan analitzant\n");
  ioctl(fd, 3);

  printf("\nFILL 2:\n");
  ioctl(fd, 0, &pids[1]);
  pinta_totes_estadistiques(fd);

  printf("\nFILL 3:\n");
  ioctl(fd, 0, &pids[2]);
  pinta_totes_estadistiques(fd);

  printf("\nFI TEST 2\n\n");

  printf("TEST 3: Desmonitoritzem la crida write i close:\n");

  ioctl(fd, 5, WRITE);
  ioctl(fd, 5, CLOSE);

  pids[3] = fork();
  if(pids[3] == 0){
    close(fd);
    hijo4();
  }
  sleep(4);

  ioctl(fd, 0,&pids[3]);
  ioctl(fd, 1, WRITE);
  if(read(fd, buf, sizeof(struct t_info)) < 0) printf("Error, crida no monitoritzada. Comportament correcte\n");
  else{
  	info = (struct t_info *)buf;
	pinta_info(info);
  }

  printf("\nFI TEST 3\n\n");

  printf("TEST 4: Tornem a monitoritzar la crida write i close:\n");

  ioctl(fd, 4, WRITE);
  ioctl(fd, 4, CLOSE);

  pids[4] = fork();
  if(pids[4] == 0){
    close(fd);
    hijo5();
  }
  sleep(4);

  ioctl(fd, 0,&pids[4]);
  ioctl(fd, 1, WRITE);
  read(fd, buf, sizeof(struct t_info));
  info = (struct t_info *)buf;
  pinta_info(info);

  printf("\nFI TEST 4\n\n");

  printf("TEST 5: Desmonitoritzem totes les crides:\n");
  ioctl(fd, 5, TOTES);
 
  pids[5] = fork();
  if(pids[5] == 0){
    close(fd);
    hijo6();
  }
  sleep(4);

  ioctl(fd, 0,&pids[5]);
  pinta_totes_estadistiques(fd);

  printf("\nFI TEST 5\n\n");

  printf("TEST 6: Tornem a monitoritzar totes les crides:\n");
  ioctl(fd, 4, TOTES);
 
  pids[6] = fork();
  if(pids[6] == 0){
    close(fd);
    hijo7();
  }
  sleep(4);

  ioctl(fd, 0,&pids[6]);
  pinta_totes_estadistiques(fd);

  printf("\nFI TEST 6\n\n");

  printf("TEST 7: Fem un ioctl per canviar de proces amb arg2 = NULL:\n");
  ioctl(fd, 0, NULL);

  pinta_totes_estadistiques(fd);

  printf("\nFI TEST 7\n\n");

  printf("TEST 8: Intentem tornar a obrir el dispositiu:\n");

  fd2 = open("/dev/consultant", O_RDONLY);
  if(fd2 < 0) printf("El canal no s'ha pogut obrir. Comportament correcte.\n");

  printf("\nFI TEST 8\n\n");

  close(fd);

  printf("\nFi joc de proves\n\n");

  printf("Per finalitzar el joc de proves, premi ctrl + c\n");

  unlink("hijo1.txt");
  unlink("hijo6.txt");
  unlink("hijo7.txt");

  while(1);
}
