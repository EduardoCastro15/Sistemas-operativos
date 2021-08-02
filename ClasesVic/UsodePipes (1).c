//uso de pipes
#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(){
	int numero; //numero a analizar
  int pid; //pid del proceso hijo
	int pipedes[2]; //Descriptor de la tubería

	if(pipe(pipedes)==-1){//cierra el programa en caso de que no se haya creado el pipe
		perror("Error al crear el pipe\n");
		exit(0);
	}

	pid=fork();
	
  if(pid==0){//proceso hijo
    
    read(pipedes[0],&numero,sizeof(numero));
    printf("%d-------------------------\n", getpid());
    close(pipedes[1]); 
    if(numero%2==0)
      printf("El número ingresado: %d, es un numero par\n",numero );
    else
      printf("El número ingresado: %d, es impar\n",numero );
    
    printf("Ya terminé padre\n");

	}
	else{//proceso padre
    printf("%d-------------------------\n", getpid());
    close(pipedes[0]);
    puts("ingresa un número entero: \n");
    scanf("%d",&numero);
    write(pipedes[1],&numero,sizeof(numero));
	}	
return 0;
}