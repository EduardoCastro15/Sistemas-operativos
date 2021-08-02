#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
	int pid, n1=atoi(argv[1]), n2=atoi(argv[2]), i;
	for(i=0; i<4; i++){
		pid=fork();
		if(pid==0){
			printf("\n\nNo.Hijo= %d\nIdProceso= %d\nIdPadreProceso= %d\n\n", i+1, getpid(), getppid());
			switch(i){
				case 0:
					printf("Suma= %d", n1+n2);
					exit(0);
				case 1:
					printf("Resta= %d", n1-n2);
					exit(0);
					break;
				case 2:
					printf("Multiplicacion= %d", n1*n2);
					exit(0);
					break;
				case 3:
					if(n2 != 0){
						printf("Division= %d", n1/n2);
						exit(0);
					}else{
						printf("Imposible dividir entre 0");
						exit(1);
					}
					break;
			}
			exit(0);
		}else wait(NULL);
	}
}