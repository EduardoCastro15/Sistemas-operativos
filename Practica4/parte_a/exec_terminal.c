#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){

	printf("\n-------------------------------------------------------------------------------");
	printf("\nPrograma que invoca cualquier comando del interprete de comandos de la terminal");
	printf("\n-------------------------------------------------------------------------------\n\n");
	if(argc < 2){
		printf("Uso: ./exec_terminal [comando] [argumentos]");
	}else{
		execvp(argv[1], (argv+1));

		/* El código siguiente no se ejecuta a menos que falle exec */
		perror("\nError al llamar a exec\n");
		printf("\nADIOS\n");
	}

	return 0;
}
