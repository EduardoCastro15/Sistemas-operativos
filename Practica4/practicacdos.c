#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
int pid=1;

int main(int argc,char *argv[]){
	int status;
	int i=0;
	char cadena[50];
	char *final[50];
	char delimitador[] = " ";
	char *varprog="./a";
	system("clear");
	do{
		i=0;
		final[i]=(char *)varprog;
		i++;
		//Inicio del programa a ejecutar
		printf("Ingrese un comando> ");
		fflush(stdin);
    		gets(cadena);
    		char *token = strtok(cadena, delimitador);
    		if(token != NULL){
        	while(token != NULL){
            	// Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
            	final[i]=(char *)token;
            	token = strtok(NULL, delimitador);
            	i++;
    		//Delimitacion de los argumentos
    		final[i]=0;
		if(pid!=0){
			if(pid==-1){
				perror("\nError en el uso de fork()");
				exit(pid);
			}else{
				pid=fork();
		  		wait(&status);
			}
		}else{
			break;
		}
		if(pid==0){
			printf("%d\n",execvp("./a",final));
			exit(0);
		}
	}while(strcmp(cadena,"exit")!=0);
	printf("Ejecucion de programa principal ha terminado.\n");
	return 0;
}
