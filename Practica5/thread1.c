#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <pthread.h> 

char mensaje1[256],mensaje2[256];

void *fmensaje1(void *vargp) 
{ 
    sleep(1); 
    printf("Hilo %d: %s\n",*(int *)vargp,mensaje1); 
    return NULL; 
} 
void *fmensaje2(void *vargp) 
{ 
    sleep(1); 
    printf("Hilo %d: %s\n",*(int *)vargp,mensaje2); 
    return NULL; 
} 
   
int main() 
{ 
	printf("\nIntroduce el primer mensaje:");
	fgets(mensaje1,sizeof(mensaje1),stdin);
	printf("\nIntroduce el segundo mensaje:");
	fgets(mensaje2,sizeof(mensaje2),stdin);
	
   pthread_t thread1,thread2; 
   int id[2];
   id[0]=1;
   id[1]=2;
    
    pthread_create(&thread2, NULL,fmensaje2,&id[1]);
    pthread_create(&thread1, NULL,fmensaje1,&id[0]); 
     
    pthread_join(thread2,NULL);
    pthread_join(thread1,NULL); 
  	printf("\nFin\n");   
  
    return 0;
}
