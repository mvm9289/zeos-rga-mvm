#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include "../include/syscallsmon.h"
#include <signal.h>


int main(){
	write(1,"Iniciem els Jocs de proves\n",28);
	int pid = getpid();

	int canal = open("/dev/consultant", O_RDONLY);
	if(canal < 0)printf("Error\n");
	
	else {
		if(ioctl(canal, RESET) < 0) printf("Error\n");

		//Proves open
		//2 correctes i un incorrecte
		int fd0 = open("hola.txt", O_RDWR | O_CREAT);
		int fd1 = open("hola.txt", O_RDONLY);
		int fd2 = open("adeu.txt", O_RDONLY);

		//Proves close
		//1 correcte i un incorrecte
		close(fd2);
		close(fd1);

		//Proves write
		//1 correcte i 2 incorrectes
		write(fd0, "Hola\n", 5);
		write(fd1, "Jo fallo", 8);
		write(fd2, "Jo fallo", 8);

		//Proves lseek
		//1 correcte i 2 incorrectes
		lseek(fd0, 1, SEEK_END);
		lseek(fd1, 1, SEEK_END);
		lseek(fd2, 1, SEEK_END);

		//Proves clone
		//1 correcte

			/*Sincronitzacio mitjançant named_pipes*/
			mknod("pipa",S_IFIFO|0666,0);

		printf("Fem un fill\n");
		int f = fork();
		struct t_stats s;
		if(f == 0){
			int open_de_mes_incorrecte = open("",O_RDONLY);
			open_de_mes_incorrecte = open("",O_RDONLY);
			open_de_mes_incorrecte = open("",O_RDONLY);
			pid = getpid();

			
			printf("El fill ha fet tres opens i es bloquejara ara\n");
			int sincronitzacio = open("pipa", O_RDONLY);
			printf("Execucio del fill\n");
			if(ioctl(canal, SWITCH_PROCESS, &pid) < 0)printf("Error\n");
			if(ioctl(canal, SWITCH_SYSCALL, OPEN_CALL) < 0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques de l'open del fill:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);

			printf("Fem un open del fill\n");
			open_de_mes_incorrecte = open("",O_RDONLY);
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques de l'open del fill:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);
			printf("Fi de l'execucio del fill\n");

			return 0;
		}
		else{
			int status;
			int open_de_mes_incorrecte = open("",O_RDONLY);

			if(ioctl(canal, SWITCH_PROCESS, &pid) < 0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques de l'open del pare:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);
			
			printf("Des del pare consultem les stats del fill:\n");
			if(ioctl(canal, SWITCH_PROCESS, &f) < 0)printf("Error\n");
			if(ioctl(canal, SWITCH_SYSCALL, CLOSE_CALL) < 0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del close del fill:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);

			if(ioctl(canal, SWITCH_SYSCALL, OPEN_CALL) < 0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del open del fill:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);

			printf("Canviem al pare posant la direccio del PID a NULL\n");
			int* i = NULL;

			if(ioctl(canal, SWITCH_PROCESS, i) < 0)printf("Error\n");
			else printf("Canvi correcte!\n");
			if(ioctl(canal, SWITCH_SYSCALL, CLONE_CALL)<0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del clone del pare:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);

			i = (int *)2;	//Adreça invalida
			printf("Intentem canviar posant la direccio del PID incorrecta\n");
			if(ioctl(canal, SWITCH_PROCESS, i) < 0)printf("Retorna ERROR\n");

			printf("RESET DE TOTES LES ESTADISTIQUES\n");
			if(ioctl(canal, RESET_ALL)<0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del clone del pare:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);
			printf("Desbloquejem el fill i esperem la seva mort\n");
			int sincronitzacio;
			sincronitzacio = open("pipa", O_WRONLY);
	
			wait(&status);
			printf("Des del pare: fem 5 opens correctes\n");
			int open_de_mes_correcte = open("hola.txt",O_RDONLY);
			open_de_mes_correcte = open("hola.txt",O_RDONLY);
			open_de_mes_correcte = open("hola.txt",O_RDONLY);
			open_de_mes_correcte = open("hola.txt",O_RDONLY);
			open_de_mes_correcte = open("hola.txt",O_RDONLY);
			
			
			if(ioctl(canal, SWITCH_PROCESS, NULL)<0)printf("Error\n");
			if(ioctl(canal, SWITCH_SYSCALL, OPEN_CALL)<0)printf("Error\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del open del pare:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);
			
			printf("DESACTIVANT I REINICIANT ESTADISTIQUES\n");

			pid = getpid();
			if(ioctl(canal, RESET)<0)printf("Error\n");
			if(ioctl(canal, DEACTIVATE_MONITOR,OPEN_CALL)<0)printf("Error\n");
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			open_de_mes_incorrecte = open("hola1.txt",O_RDONLY);
			printf("ACTIVANT ESTADISTIQUES\n");
			if(ioctl(canal, ACTIVATE_MONITOR, ALL)<0)printf("Error\n");

			open_de_mes_incorrecte = open("hola.txt",O_RDONLY);
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del open del pare:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);
			
		}
		unlink("pipa");
		printf("Fem un nou fill\n");
		pid=fork();
		if (pid==0) {
			while(1);
		}
		else {
			if(ioctl(canal, SWITCH_PROCESS, &pid)<0)printf("Error\n");
			
			printf("Provem que els stats d'un nou proces estan inicialitzades\n");
			read(canal, &s, sizeof(struct t_stats));
			printf("Estadistiques del open del fill:\n");
			printf("Crides fetes %d\n", s.total_calls);
			printf("Crides correctes %d\n", s.ok_calls);
			printf("Crides incorrectes %d\n", s.error_calls);
			//printf("Temps %d\n", s.total_time);
			kill(pid, SIGKILL);
			waitpid(pid, NULL);
			printf("El fill ha mort\n");

			printf("Provem que intentar llegir els stast d'un proces mort doni error\n");
			if(read(canal, &s, sizeof(struct t_stats)) < 0)printf("Retorna ERROR\n");
			else printf("No retorna error\n");

			printf("Provem de canviar a analitzar un proces amb un pid mort\n");

			if(ioctl(canal, SWITCH_PROCESS, &pid)<0)printf("Retorna ERROR\n");
			else printf("No retorna error\n");
			printf("Fi dels jocs de proves\n");
			printf("BON NADAL!\n");

		}
	}
	
    unlink("hola.txt");


	//while(1);

	return 0;
	
}
