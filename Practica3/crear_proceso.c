#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
	int pid, n=atoi(argv[1]), i;
	for(i=0; i<n; i++){
		pid=fork();
		if(pid==0){
			printf("\n\nNo.Hijo= %d\nIdProceso= %d\nIdPadreProceso= %d", i+1, getpid(), getppid());
			exit(0);
		}else{
			printf("\nIdPadre= %d", getpid());
			wait(NULL);
		}
	}
}