#include <stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <unistd.h>  

int a,b,var=0;


void *funcion_a(void *vargp) 
{ 
    sleep(1); 
    printf("\nHilo A incremento la variable global en %d unidades",a); 
    var=var+a;
    return NULL; 
} 
void *funcion_b(void *vargp) 
{ 
    sleep(1); 
    printf("\nHilo B decremento la variable global en %d unidades",b); 
    var=var-b;
    return NULL; 
} 


int main(int argc, char const *argv[])
{
	printf("\nIntroduce A unidades a incrementar:");
	scanf("%d",&a);
	printf("\nIntroduce B unidades a disminuir:");
	scanf("%d",&b);
	printf("\nLa variable global inicialmente vale %d",var);


	pthread_t hiloa,hilob;

	pthread_create(&hilob, NULL,funcion_b,NULL); 
	pthread_create(&hiloa, NULL,funcion_a,NULL);
    

    pthread_join(hiloa,NULL);
    pthread_join(hilob,NULL);

    printf("\nLa variable global finalmente vale %d\n",var);
	return 0;
}