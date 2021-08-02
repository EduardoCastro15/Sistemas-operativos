#include <stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include <sys/time.h>

char **gargv;//Global argv
int suma[1000]={0};
int j=0;

void* mifuncion(void* arg)
{
	int i,n,n1,n2,parte,sumahilo=0;
	n=atoi(gargv[3]);
	n1=atoi(gargv[1]);
	n2=atoi(gargv[2]);
	parte=(n2/n);
	if(n2%n!=0&&j==1)
	{
		parte=parte+(n2-(parte*n));		
	}	
	
	for(i=0;i<parte;i++)
		sumahilo=sumahilo+n1;
	suma[j]=sumahilo;
	j++;
}



int main(int argc, char  **argv)
{
	struct timeval startprograma, endprograma;
	gettimeofday(&startprograma, NULL);
	gargv=argv;
	int i,n,n1,n2,sumatotal=0;
	struct timeval start, end;

	n=atoi(gargv[3]);
	n1=atoi(gargv[1]);
	n2=atoi(gargv[2]);
	pthread_t hilos[n];//Declarando hilos
	
	 gettimeofday(&start, NULL);
	for(i=0;i<n;i++)//Creando hilos
		pthread_create(&hilos[i],NULL,mifuncion,NULL);

	for(i=0;i<n;i++) //Joining hilos i.e. esperando a que terminen
		{
			pthread_join(hilos[i],NULL);
			 gettimeofday(&end, NULL);
			printf("El hilo[%d] tardo %ld microsegundos\n",i+1,((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));
		}
	for(i=0;i<n;i++)
		sumatotal=sumatotal+suma[i];

	printf("La suma es: %d\n",sumatotal);
	printf("El programa tardo %ld microsegundos\n",-1*((endprograma.tv_sec * 1000000 + endprograma.tv_usec)
		  - (startprograma.tv_sec * 1000000 + startprograma.tv_usec)));


	gettimeofday(&endprograma, NULL);
	return 0;
	
}