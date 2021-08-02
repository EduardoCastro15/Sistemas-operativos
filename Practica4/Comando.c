#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	if(argc<2){
		printf("Ingrese un elemento\n");
	}else{
		char *Argumentos[argc];
		for(int i=1; i<=argc; i++){
			Argumentos[i-1]=argv[i];
		}
		Argumentos[argc]=0;
		execvp(Argumentos[0], Argumentos);
	}
	
}
