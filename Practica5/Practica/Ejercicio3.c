#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int A, B, Final=0;

void *HiloA(void *vargp){
    Final+=A;
    return NULL; 
}

void *HiloB(void *vargp){
    Final-=B;
    return NULL; 
} 

int main(int argc, char const *argv[]){
    printf("\nA:");
    scanf("%d",&A);
    printf("\nB:");
    scanf("%d",&B);

    pthread_t ThreadA, ThreadB;

    pthread_create(&ThreadB, NULL, HiloB, NULL); 
    pthread_create(&ThreadA, NULL, HiloA, NULL);
    
    pthread_join(ThreadA,NULL);
    pthread_join(ThreadB,NULL);

    printf("\nEl valor de la variable global es %d - %d = %d\n", A, B, Final);
    return 0;
}
