#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SALIR 0

struct nodo{
    char *cadena;
    int argc;
    char **comandos;
    int operador;
    struct nodo *nodo_sig;
};
//Eduardo Castro
void Prompt();
void ComandoEntrada(char *Comand);
void CrearProceso(char* Argumento[]);
void ComandoSalida(char *Comand, int Red);
//Eduardo Cortés
void Debug(struct nodo *cola, int n);
void liberarMemoria(struct nodo *cola);
int analizadorParser(struct nodo **cola);
void Tuberias(struct nodo *cola, int pipeNum);
int BuscarToken(char *cadena, char *token, char **ap);
void CrearProcesoConPipe(struct nodo *cola, int *pipes, int pipeNum, int n);

int main(int argc, char *argv[]){
    struct nodo *cola;

    int bandera = 1;
    int pipeNum = 0;

	int STDout=dup(1); //Duplica la salida estandar
	int STDin=dup(0); //Duplica la entrada estandar

    while(bandera != SALIR){
        int pid, status;

		close(1); // Cierro la salida estandar
	    dup(STDout); // Duplico la salida estandar
		close(0); // Cierro la entrada estandar
	    dup(STDin); // Duplico la entrada estandar

        Prompt();
        if(pipeNum = analizadorParser(&cola)){
            if(pipeNum != -1){
                if(cola->nodo_sig == NULL){
                    CrearProceso(cola->comandos);
                }else{
                    Tuberias(cola, pipeNum);
                }
                liberarMemoria(cola);
            }
        }
        bandera = pipeNum;
    }

	return 0;
}

int analizadorParser(struct nodo **cabeza){
    char buffer[256], *aux, dir[256];
    int i = 0, adj = 0;
    struct nodo *cola;

    if(fscanf(stdin, "%[^\n]%*c", buffer) && *buffer != EOF){ //Leer del teclado, comprobar que el usuario no solo presione Enter.
        if(strcmp(buffer, "exit") != 0 && strcmp(buffer, "EXIT") != 0){ //Si el usuario introdujo la palabra "exit o EXIT", sale del programa.

            *cabeza = (struct nodo*)malloc(sizeof(struct nodo)); //asignar memoria para el primer nodo de la cola.
            (*cabeza)->nodo_sig = NULL;

            cola = *cabeza; //copiar el apuntador a la cabeza de la cola a una variable auxiliar.
            /*======================
            Primera parte - Tuberías
            ======================*/
            aux = strtok(buffer, "|"); //Separar la cadena introducida donde se encuentre el operador '|'.

            while(1){
                cola->cadena = malloc(256*sizeof(char*)); //asignar memoria para la cadena de la tubería.
                strcpy(cola->cadena, aux); //Copiar el comando.
                aux = strtok(NULL, "|"); //Continuar separando la cadena introducida por el operador '|'.
                if(aux != NULL){
                    cola->nodo_sig = (struct nodo*)malloc(sizeof(struct nodo)); //asignar memoria para el siguiente nodo.
                    cola = cola->nodo_sig; //Movemos el apuntador al siguiente nodo de la cola.
                    cola->nodo_sig = NULL;
                }else{
                    break; //Ya no hay más tuberías.
                }
            }
            //printf("\nPRIMERA PARTE... DONE.\n");

            /*=========================
            Segunda parte - Redirección
            =========================*/
            cola = *cabeza;
            while(cola != NULL){ //Recorremos todas las tuberías y buscamos operadores de direccionamiento.
                strcpy(dir, cola->cadena);
                if(BuscarToken(cola->cadena, "<", &aux) != -1){
                    ComandoEntrada(aux);
                    adj = BuscarToken(dir, ">", &aux);
                    if(adj != -1)
                        ComandoSalida(aux, adj);
                }else{
                    adj = BuscarToken(cola->cadena, ">", &aux);
                    if(adj != -1)
                        ComandoSalida(aux, adj);
                }
                cola = cola->nodo_sig; //Movemos el apuntador al siguiente nodo de la cola.
            }
            //printf("\nSEGUNDA PARTE... DONE.\n");

            /*========================
            Tercera parte - Argumentos
            ========================*/
            cola = *cabeza;
            while(cola != NULL){ //Recorremos todas las tuberías y separamos por argumentos.
                i = 0; //Inicializamos la variable iteradora.
                cola->comandos = malloc(256*sizeof(char**)); //asignar memoria para el arreglo de cadenas.
                aux = strtok(cola->cadena, " "); //Separar la cadena del comando en espacios.
                while(aux != NULL){
                    (cola->comandos)[i] = malloc(256*sizeof(char*)); //Asignar memoria para la cadena del argumento.
                    strcpy((cola->comandos)[i++], aux); //Copiar argumento al arreglo de cadenas.
                    aux = strtok(NULL, " "); //Continuar separando la cadena introducida en espacios.
                }
                cola->argc = i; //Guardamos el número de argumentos.
                cola = cola->nodo_sig; //Movemos el apuntador al siguiente nodo de la cola.
            }
            //printf("\nTERCERA PARTE... DONE.\n");

            if(0 == (*cabeza)->argc){ //El usuario introdujo únicamente espacios en blanco.
                i = 0;
            }else{
                cola = *cabeza;
                i = 0; //Se usará para contar el número de tuberías.
                while(cola != NULL){
                    (cola->comandos)[cola->argc] = NULL;
                    cola = cola->nodo_sig; //Movemos el apuntador al siguiente nodo de la cola.
                    i++;
                }
            }
        }else{
            return 0; //El usuario introdujo la palabra "exit o EXIT".
        }
    }else{ //El usuario no introdujo datos.
        i = -1;
        getchar(); //Limpiar stdin.
    }
    return i; //Regresar el número de tuberías.
}

void Prompt(){ //INICIO DE LA FUNCIÓN Prompt
	int hostname_size=sysconf(_SC_HOST_NAME_MAX); //Funcion que arroja el tamaño del hostname
	char* whoami=getenv("USER"); //Funcion que arroja el nombre del usuario
	char* hostname; //Guarda el nombre del usuario
	char cwd[100]; //Guarda la direccion del archivo
	hostname=(char*)malloc(hostname_size); //Reserva memoria de forma dinamica
	gethostname(hostname, hostname_size); //Funcion que retorna el hostname y lo almacena
	getcwd(cwd, sizeof(cwd)); //Funcion que retorna la ruta del archivo y lo almacena
	printf("%s", whoami); //Imprime el usuario
	printf("@%s:", hostname); //Imprime el hostname
	printf("~%s$ ",cwd); //Imprime la ruta del archivo
    free(hostname);
}

void ComandoEntrada(char *ComandPtr){
	int fd; //Descriptor de fichero

	fd=open(ComandPtr, O_RDONLY); //Asigno a la salida el fichero
	close(0); //Cierro la entrada est�ndar
	dup(fd); //Duplicamos el descriptor de fichero
}

void ComandoSalida(char *ComandPtr, int Red){
	int Existe=open(ComandPtr, O_RDONLY); //Verifica si el archivo existe o no

	close(1); //Cierro la salida est�ndar
	if(Red==0){
        open(ComandPtr, O_CREAT | O_WRONLY | O_TRUNC, 0777); //Creamos el fichero en el que se va almacenar la salida del comando
	}else{ //Si el usuario desea que la salida se escriba hasta el final de un archivo que ya existente
		if(Existe==0){
            open(ComandPtr, O_APPEND | O_WRONLY, 0777); //Abrimos el fichero y guardamos la informacion hasta el final (El fichero si existe)
        }else{
            open(ComandPtr, O_CREAT | O_APPEND | O_WRONLY, 0777); //Abrimos el fichero y guardamos la informacion hasta el final (El fichero no existe)
        }
	}
}

void CrearProceso(char* Argumento[]){
    int status; //Bandera de estado
	int pid;
    pid=fork(); //Creacion de un proceso hijo el cual ejecutara los comandos
	if(pid == -1){
        perror("ERROR EN LA CREACION DEL PROCESO..."); //Comprobamos si el hijo se creó bien
	}else if(pid==0){ //Proceso hijo
		execvp(Argumento[0], Argumento); //Ejecutamos el comando
		perror("MiniShell"); //Error al no poder ejecutar el comando
		exit(status); //Finaliza ejecucion del interprete
	}else{
        pid=wait(&status);
    }
}

void CrearProcesoConPipe(struct nodo *cola, int *pipes, int pipeNum, int n){
    if(cola->nodo_sig != NULL){
        if (fork() == 0){           
            dup2(pipes[(n*2)], 0); // reemplazar el stdin con el read end de la tubería anterior.
            dup2(pipes[1 + ((n+1)*2)], 1);  // reemplazar el stdout con el write end de la tubería anterior.

            /* se cierran las tuberías */
            for(int i = 0; i < (pipeNum*2); i++){
                close(pipes[i]);
            }

            execvp(*cola->comandos, cola->comandos);
        }else{
            CrearProcesoConPipe(cola->nodo_sig, pipes, pipeNum, (n+1));
        }
    }else{ //Última tubería
        if (fork() == 0){
            dup2(pipes[(n*2)], 0); //reemplazamos el stdin con el input read de la tubería anterior

            /* se cierran las tuberías */
            for(int i = 0; i < (pipeNum*2); i++){
                close(pipes[i]);
            }

            execvp(*cola->comandos, cola->comandos);
        }
    }
}

void Tuberias(struct nodo *cola, int pipeNum){
    int status; //Banderas de estado
	int pid;
    int *pipes;

    pipes = malloc(pipeNum*2*sizeof(char*));
    
    /* Inicializar tuberías */
    for(int i = 0; i < (pipeNum*2); i++){
        pipe(pipes + (i*2)); // Crear las tuberías.
    }

    // Creamos el fork que ejecutará el primer comando.
    if (fork() == 0){
        dup2(pipes[1], 1); //reemplazamos el stdout con write de la primera tubería.

        /* se cierran las tuberías */
        for(int i = 0; i < (pipeNum*2); i++){
            close(pipes[i]);
        }

        execvp(*cola->comandos, cola->comandos); //Ejecutar el primer comando.
    }else{
        CrearProcesoConPipe(cola->nodo_sig, pipes, pipeNum, 0);
    }

    /* Solo el proceso padre llega aquí; espera a que los hijos terminen */
    for(int i = 0; i < (pipeNum*2); i++){
        close(pipes[i]);
    }
    for (int i = 0; i < (pipeNum); i++)
        wait(&status);

    free(pipes);
}

int BuscarToken(char *cadena, char *token, char **ap){
    char *aux, dir[256], dir2[256], out[256];
    int i = 0; //Inicializamos la variable iteradora.
    int adj = -1;
    while(cadena[i++] != '\0'){
        if(cadena[i] == token[0]){
            if(cadena[i+1] == token[0])
                adj = 1; // adjuntar al final del archivo.
            else
                adj = 0;
            aux = strtok(cadena, token); //Separar la cadena del comando por el caracter '<'.
            while(aux != NULL){
                strcpy(dir, aux);
                aux = strtok(NULL, token); //Continuar separando la cadena introducida en espacios.
            }
            aux = dir;
            while(aux[0] == ' ')
                aux++;
            strcpy(dir2, aux);
            i = 0;
            while(dir2[i] != '\0'){
                if(dir2[i] == ' '){
                    dir2[i] = '\0';
                    aux = dir2;
                    break;
                }
                i++;
            }
            strcpy(out, aux);
            *ap = out;
            break;
        }
    }
    return adj;
}

void liberarMemoria(struct nodo *cola){
    struct nodo *aux;
    while(cola != NULL){
        for(int i = 0; i < cola->argc; i++)
            free((cola->comandos)[i]);
        free(cola->comandos);
        free(cola->cadena);
        aux = cola;
        cola = cola->nodo_sig; //Movemos el apuntador al siguiente nodo de la cola.
        free(aux);
    }
}

void Debug(struct nodo *cola, int n){
    int pipeI = 0;
    printf("\nPipe Num: %d", n);
    while(cola != NULL){
        printf("\nCMD[%d]: ", pipeI++);
        for(int i = 0; i < cola->argc; i++){
            printf("\n\tArg[%d]: [%s]", i, (cola->comandos)[i]);
        }
        cola = cola->nodo_sig; //Movemos el apuntador al siguiente nodo de la cola.
    }
    printf("\n\n");
}