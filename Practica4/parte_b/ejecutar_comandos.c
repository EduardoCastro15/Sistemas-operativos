#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SALIR -1

int leerTeclado(char ***comandos, char *nombre_programa);
void liberarMemoria(int argc, char **argv);

int main(int argc, char *argv[]){
    int argc_interno = 0;

    while(argc_interno != SALIR){
        int pid, status;
        char **comandos;
        printf("\nIntroduce comandos > ");
        argc_interno = leerTeclado(&comandos, argv[0]);
        if(argc_interno > 0){
            pid = fork();
            if(pid == -1){
        		perror("\n\nError en la llamada a fork.");
        		exit(-1);
            }else if(pid == 0){
                execv("parte_a", comandos);
                exit(0);
            }else{
                pid = wait(&status);
                if(WIFEXITED(status)){
                    printf("\nMi hijo con PID: %i terminó con el estado %i\n", pid, WEXITSTATUS(status));
                    printf("---------------------------------------------\n");
                }
            }
            liberarMemoria(argc_interno, comandos);
        }
    }

	exit(0);
}

int leerTeclado(char ***comandos, char *nombre_programa){
    char buffer[256], *aux;
    int i = 0;

    if(fscanf(stdin, "%[^\n]%*c", buffer) && *buffer != EOF){ //Leer del teclado, comprobar que el usuario no solo presione Enter.
        if(strcmp(buffer, "exit") != 0 && strcmp(buffer, "EXIT")){ //Si el usuario introdujo la palabra "exit o EXIT", sale del programa.

            *comandos = malloc(256*sizeof(char**)); //asignar memoria para el arreglo de cadenas.
            aux = strtok(buffer, " "); //Separar la cadena introducida en espacios.

            /*  Se agrega un comando extra al inicio.
                Esto es debido a que el programa de la 'parte a' requiere que se ignore el primer
                argumento, ya que este argumento corresponde con el nombre del programa
                ("ejecutar comandos" (padre) o "parte_a" (hijo) en este caso). */
            if(aux != NULL){
                (*comandos)[i] = malloc(256*sizeof(char*)); //asignar memoria para la cadena del argumento.
                strcpy((*comandos)[i++], nombre_programa); //copiar nombre del programa al arreglo de cadenas.
            }

            while(aux != NULL){
                (*comandos)[i] = malloc(256*sizeof(char*)); //asignar memoria para la cadena del argumento.
                strcpy((*comandos)[i++], aux); //copiar argumento al arreglo de cadenas.
                aux = strtok(NULL, " "); //Continuar separando la cadena introducida en espacios.
            }

            if(0 == i){ //El usuario introdujo únicamente espacios en blanco.
                free(*comandos); //Liberar memoria.
            }else{
                (*comandos)[i] = NULL;
            }

        }else{
            i = SALIR; //El usuario introdujo la palabra "exit o EXIT".
        }
    }else{ //El usuario no introdujo datos.
        getchar(); //Limpiar stdin.
    }

    return i; //Regresar el número de argumentos.
}

void liberarMemoria(int argc, char **argv){
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
}
