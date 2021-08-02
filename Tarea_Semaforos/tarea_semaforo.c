#include<stdlib.h>
#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>

#define NumHilos 10

int P=0;
sem_t S;
sem_t Lim;

void* Thread(void *arg){
	int i;
	sem_wait(&S);   //down(s)
	P++;
	printf("\nAcabo de llegar al metro, todavia no estamos completos\n");
	if(P<NumHilos){
		sem_post(&S);   //up(s)
		sem_wait(&Lim);   //down(barrera)
	}else{
		for(i=0; i<NumHilos; i++)
			sem_post(&Lim);  //up(barrera)
		P=0;
		sem_post(&S);  //up(s)
	}
	pthread_exit(0);
}

int main(){
	int i, Val, A[NumHilos];
	pthread_t Hilos[NumHilos];
	sem_init(&S, 0, 1);
	sem_init(&Lim, 0, 0);
	for(i=0; i<NumHilos; i++){
		A[i]=i;
		Val=pthread_create(&Hilos[i], NULL, Thread, NULL);
		if(Val){
			perror("\nERROR\n");
			exit(-1);
		}
	}
	for(i=0; i<NumHilos; i++)
		pthread_join(Hilos[i], NULL);

	printf("\nYa estamos todos juntos, vamos a la escuela\n");
	return 0;
}

