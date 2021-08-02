#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>

int NumLeer=0;
sem_t Escritor;
pthread_mutex_t mutex;

void *FunEscritor(void *wid){
	char Cont, Mod[50];

	FILE* Archivo;
	Archivo=fopen("Archivo.txt","a+");

	if(Archivo==NULL)
		perror("Error al intentar abrir el archivo");
	sem_wait(&Escritor);
	printf("\nIngresa lo que desees escribir: ");
	fgets(Mod, sizeof(Mod), stdin);
	fprintf(Archivo, "%s", Mod);
	printf("El escritor No.%d modifico el archivo a\n", *(int *)wid);
	rewind(Archivo);
	while((Cont=fgetc(Archivo)) != EOF)
		printf("%c",Cont);

	fclose(Archivo);
	sem_post(&Escritor);
}

void *FunLector(void *rid){
	FILE* Archivo;
	char Cont;
	
	if((Archivo=fopen("Archivo.txt", "r")) == NULL)
        perror("Error al intentar abrir el archivo");

	pthread_mutex_lock(&mutex);
	NumLeer++;
	if(NumLeer == 1)
		sem_wait(&Escritor);

	pthread_mutex_unlock(&mutex);
	
	printf("El lector No.%d leyo el archivo como\n",*(int *)rid);
	while((Cont=fgetc(Archivo))!=EOF)
		printf("%c", Cont);
	pthread_mutex_lock(&mutex);
	NumLeer--;
	fclose(Archivo);
	if(NumLeer == 0)
		sem_post(&Escritor);
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char const *argv[]){
	FILE* Archivo;
	Archivo=fopen("Archivo.txt", "w+");
	fclose(Archivo);

	int NumLectores, NumEscritores, i;
	
	NumLectores=atoi(argv[1]);
	NumEscritores=atoi(argv[2]);
	
	pthread_t Lectores[NumLectores], Escritores[NumEscritores];	
	pthread_mutex_init(&mutex, NULL);
	sem_init(&Escritor, 0, 1);

	int Identificador[10]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	
	for(i=0; i<NumLectores; i++)
		pthread_create(&Lectores[i], NULL, (void *)FunLector, (void *)&Identificador[i]);

	for(i=0; i<NumEscritores; i++)
		pthread_create(&Escritores[i], NULL, (void *)FunEscritor, (void *)&Identificador[i]);

	for(i=0; i<NumLectores; i++)
		pthread_join(Lectores[i], NULL);	

	for(i=0; i<NumEscritores; i++)
		pthread_join(Escritores[i], NULL);

	pthread_mutex_destroy(&mutex);
	sem_destroy(&Escritor);
	return 0;
}
