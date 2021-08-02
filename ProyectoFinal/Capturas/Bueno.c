#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MSJ_ERROR "ERROR DE SINTAXIS...\n"
#define MAX 40

void Prompt(){
	int hostname_size=sysconf(_SC_HOST_NAME_MAX); //Funcion que arroja el tamaño del hostname
	char* whoami=getenv("USER"); //Funcion que arroja el nombre del usuario actual
	char* hostname=(char*)malloc(hostname_size); //Reserva memoria dinamica para el nombre del equipo
	char cwd[100]; //Guarda la ruta de acceso

	gethostname(hostname, hostname_size); //Funcion que retorna el hostname y lo almacena
	getcwd(cwd, sizeof(cwd)); //Funcion que retorna la ruta del archivo y lo almacena
	printf("%s", whoami); //Imprime el usuario
	printf("@%s:", hostname); //Imprime el nombre del equipo
	printf("~%s$ ",cwd); //Imprime la ruta del archivo
}

void ComandoEntrada(char Comand[]){
	int fd; //Descriptor de fichero
	char* ComandPtr=Comand; //Puntero a la cadena de comandos

	fd=open(ComandPtr, O_RDONLY); //Asigno a la salida el fichero
	close(0); //Cierro la entrada estándar
	dup(fd); //Duplicamos el descriptor de fichero
}

void ComandoSalida(char Comand[], int Red){
	char* ComandPtr=Comand; //Puntero a la cadena de comandos
	int Existe=open(ComandPtr, O_RDONLY); //Verifica si el archivo existe o no

	close(1); //Cierro la salida estándar
	if(Red==0) open(ComandPtr, O_CREAT | O_WRONLY, 0777); //Creamos el fichero en el que se va almacenar la salida del comando
	else{ //Si el usuario desea que la salida se escriba hasta el final de un archivo que ya existente
		if(Existe==0) open(ComandPtr, O_APPEND | O_WRONLY, 0777); //Abrimos el fichero y guardamos la informacion hasta el final (El fichero si existe)
		else open(ComandPtr, O_CREAT | O_APPEND | O_WRONLY, 0777); //Abrimos el fichero y guardamos la informacion hasta el final (El fichero no existe)
	}
}

void Tuberias(char* Comando1[], char* Comando2[]){
	int fd[2]; //Descriptor de fichero
	int Estado=0; //Bandera de estado
	pid_t Hijo=fork(); //Creacion de un proceso hijo el cual ejecutara los comandos
	if(Hijo==-1) perror("ERROR EN LA CREACION DEL PROCESO..."); //Comprobamos si el hijo se creó bien
	else if(Hijo==0){ //Proceso hijo
		pipe(&fd[0]); //Función pipe encargada de crear la tubería
   		if(fork()!=0){ //Se ejecuta el primer comando ingresado
      		close(fd[0]); //Cerramos el lado de lectura de la tuberia
      		close(1); //Cerramos la salida estandar
      		dup(fd[1]); //STDin = Extremo de entrada del pipe
      		close(fd[1]); //Cerramos el descriptor de fichero que sobra tras el dup(fd[1])
      		execvp(Comando1[0],Comando1); //Ejecutamos el primer comando
   		}
   		else{ //Se ejecuta el segundo comando ingresado
      		close(fd[1]); //Cerramos el lado de escritura de la tuberia
      		close(0); //Cerramos la entrada estandar
      		dup(fd[0]); //STDout = Extremo de salida del pipe
      		close(fd[0]); //Cerramos el descriptor de fichero que sobra tras el dup(fd[0])
     		execvp(Comando2[0],Comando2); //Ejecutamos el segundo comando
    	}
	}
	else Hijo=wait(&Estado); //Proceso padre esperando al hijo
}

void CrearProceso(char* Argumento[]){
	int Estado=0; //Badera de estado
	pid_t Hijo=fork(); //Creacion de un proceso hijo el cual ejecutara los comandos
	if(Hijo==-1) perror("ERROR EN LA CREACION DEL PROCESO..."); //Comprobamos si el hijo se creó bien
	else if(Hijo==0){ //Proceso hijo
		execvp(Argumento[0], Argumento); //Ejecutamos el comando
		perror("MiniShell"); //Error al no poder ejecutar el comando
		exit(Estado); //Finaliza ejecucion del interprete
	}
	else Hijo=wait(&Estado);
}

void Comando(char Comand[]){
	int i=0; //Iterador
	int j=0; //Iterador
	int k=0; //Iterador
	int Red=0; //Bandera que indica si se va a redireccionar la salida de un comando hasta el final del fichero >>
	int Ejecutar=0; //Bandera de ejecucion
	int Tuberia=0; //Determina si ejecutamos un proceso único o dos procesos coenctados mediante tuberia
	char Arg1[MAX][MAX]; //Almacena un argumento en caso de que no haya tuberías
	char Arg2[MAX][MAX]; //Almacena 2 argumentos si el usuario introduce tuberías
	char Salida[MAX]; //Almacena el nombre del fichero donde el usuario quiera volcar la información del comando
	char Entrada[MAX]; //Almacena el nombre del fichero de donde el usuario quiera volcar la información para el comando
	char* Comando1[MAX]; //Almacena los comandos para que el execvp los ejecute
	char* Comando2[MAX]; //Almacena los comandos para que el execvp los ejecute

	Comando1[0] = NULL; //La primera posicion del arreglo es NULL
	Comando2[0] = NULL; //La primera posicion del arreglo es NULL

	while((Comand[i]!='\0') && (Comand[i]!='|') && (Comand[i]!='>')){ //Recorremos el arreglo hasta que nos topemos con un caracter nulo, o un mayor que, o un OR
		for(j=0; (Comand[i]!=' ') && (Comand[i]!='\0') && (Comand[i]!='|') && (Comand[i]!='>') && (Comand[i]!='<'); j++){ //Recorremos el arreglo hasta topar con un espacio, o un caracter nulo, o un MAYOR QUE, o un MENOR QUE, o un OR
			Arg1[k][j]=Comand[i]; //Almacenamos cada posicion del arreglo en una coordenada de la matriz
			i++; //Avanzamos de posicion en el arreglo de comandos
		}
		if(Comand[i]==' ') i++; //En caso de encontrarse con un espacio incrementamos el contador en 1, para evitar un ciclo infinito
		Arg1[k][j]='\0'; //Agregamos un caracter nulo como fin de la cadena
		Comando1[k]=Arg1[k]; //Y finalmente una vez creada la cadena, se la pasamos al puntero argumento que será el que se ejecute con la función execvp
		k++; //Avanzamos una fila
		if(Comand[i]=='<'){ //Nos permite saber cuando se desea un redireccionamiento de entrada
			i++; //Avanzamos de posicion en el arreglo
			if(Comand[i]!=' ') Ejecutar=1; //Si nos encontramos con algo que no sea un espacio cambiamos la bandera de ejecucion
			else{ //En caso de encontrarnos con un espacio
				i++; //Avanzamos de posicion en el arreglo de comandos
				for(j=0; (Comand[i]!=' ') && (Comand[i]!='\0') && (Comand[i]!='|') && (Comand[i]!='>'); j++){ //Recorremos el arreglo hasta topar con un espacio, o un caracter nulo, o un MAYOR QUE, o un OR
					Entrada[j] = Comand[i]; //Estamos haciendo una copia del comando/archivo de entrada
					i++; //Avanzamos de posicion en el arreglo de comandos
				}
				Entrada[j]='\0'; //Agregamos un caracter nulo como fin de la cadena
				if(Comand[i]!='\0') i++; //En caso de encontrarnos con un caracter nulo significa que un ampersand fue sustituido en esta posicion
				ComandoEntrada(Entrada); //Mandamos a llamar a la funcion ComandoEntrada pasando como argumento el comando/archivo
			}
		}
	}
	Comando1[k]=NULL; //Asignamos NULL al último comando a ejecutar para el EXECV
	if (Comand[i]=='|') {//Si encuentra una tubería, separará el segundo comando de la misma forma que se hizo con el primero
		k=0; //Reiniciamos iterador
		i++; //Avanzamos de posicion en el arreglo de comandos
		if(Comand[i]!=' ') Ejecutar=1; //Si nos encontramos con algo que no sea un espacio cambiamos la bandera de ejecucion
		else{ //En caso de encontrarnos con un espacio
			i++; //Avanzamos de posicion en el arreglo de comandos
			Tuberia=1; //Cambiamos la bandera de tuberia
			while((Comand[i]!='\0') && (Comand[i]!='>')){ //Recorremos el arreglo hasta que nos topemos con un caracter nulo o un mayor que	 
				for(j=0; (Comand[i]!=' ') && (Comand[i]!='\0') && (Comand[i]!='>'); j++){ //Recorremos el arreglo hasta que nos topemos con un espacio, o con un caracter nulo o un mayor que	
					Arg2[k][j]=Comand[i]; //Almacenamos cada posicion del arreglo en una coordenada de la matriz
					i++; //Avanzamos de posicion en el arreglo de comandos
				}
				if(Comand[i]==' ') i++; //En caso de encontrarse con un espacio incrementamos el contador en 1, para evitar un arreglo infinito
				Arg2[k][j]='\0'; //Agregamos un caracter nulo como fin de la cadena
				Comando2[k]=Arg2[k]; //Y finalmente una vez creada la cadena, se la pasamos al puntero argumento que será el que se ejecute con la función execvp
				k++; //Avanzamos una fila
			}
			Comando2[k]=NULL; //Asignamos NULL al último comando a ejecutar para el EXECV
		}
	}
	if(Comand[i]=='>'){ //Si encontramos un > cortará la cadena que será el fichero que quiere utilizar para la salida
		i++; //Avanzamos de posicion en el arreglo de comandos
		if((Comand[i]==' ') || (Comand[i]=='>')){ //Verificamos si el usuario desea redireccionar la salida hasta el final de un archivo existente
			if(Comand[i]=='>'){ //El usuario lo desea almacenar en un fichero, hasta el final
				Red=1; //Indica si el usuario desea redireccionar la salida
				i+=2; //Avanzamos de posicion en el arreglo de comandos
				for(j=0; Comand[i]!='\0';j++){ //Recorremos el arreglo hasta toparnos con algun caracter nulo
					Salida[j]=Comand[i]; //Estamos haciendo una copia del comando/archivo de salida
					i++; //Avanzamos de posicion en el arreglo de comandos
				}
				Salida[j]='\0'; //Agregamos un caracter nulo como fin de la cadena
				ComandoSalida(Salida, Red);	//Mandamos a llamar a la funcion ComandoSalida pasando como argumento el comando/archivo
			}
			else{
				i++; //Avanzamos de posicion en el arreglo de comandos
				for(j=0; Comand[i]!='\0';j++){ //Recorremos el arreglo hasta toparnos con algun caracter nulo
					Salida[j]=Comand[i]; //Estamos haciendo una copia del comando/archivo de salida
					i++; //Avanzamos de posicion en el arreglo de comandos
				}
				Salida[j]='\0'; //Agregamos un caracter nulo como fin de la cadena
				ComandoSalida(Salida, Red);	//Mandamos a llamar a la funcion ComandoSalida pasando como argumento el comando/archivo
			}
		}
		else Ejecutar=1; //Existe un error de sintaxis
	}
	if(Ejecutar==0){ //Checamos la bandera de errores, si es 0, entonces, ejecuta los comandos
		if (Tuberia==0) CrearProceso(Comando1); //En caso de que la bandera de Tuberia se encuentre apagada, entonces, mandamos a llamar a la funcion CrearProceso
		else Tuberias(Comando1,Comando2); //En caso contrario, mandamos a llamar a la funcion Tuberias
	}
	else printf(MSJ_ERROR); //En caso contrario, significa que encontro algun error de sintaxis durante el analisi del arreglo
}

int main(int argc, char* argv[]){
	char Comand[MAX]; //Comando ingresado por el usuario
	int i; //Iterador
	int Ejecutar=1; //Bandera de ejecucion
	int STDout=dup(1); //Duplica la salida estandar
	int STDin=dup(0); //Duplica la entrada estandar
	while(Ejecutar){ //Ciclo que permite ejectuar comandos hasta que el ususario ingrese exit
		close(1); // Cierro la salida estandar
	    dup(STDout); // Duplico la salida estandar
		close(0); // Cierro la entrada estandar
	    dup(STDin); // Duplico la entrada estandar
		Prompt(); // Imprimimos el prompt dinamico
		scanf("\n%[^\n]",Comand); // Escaneamos el comando hasta que el usuario pulsa enter
		if(strcmp(Comand, "exit")==0) break; //Si el usuario ingresa la cadena exit, el ciclo se rompe y sale
		else Comando(Comand); //Llamada a funcion Comando
	}
	exit(0); //Finaliza ejecucion del interprete
	return(0);
}
