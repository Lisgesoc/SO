#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
struct threadArgs{
	int id;
	char priority;
};

void *thread_usuario(void *arg)
{
	struct threadArgs *args=(struct threadArgs*) arg;
	int numHilo=args->id;
	char priority=args->priority;
	pthread_t id = pthread_self();
	free(arg);

	if(priority=='N'){
		sleep(3);
	}

	printf("Identificador del hilo: %lu\n", id);
	printf("Numero del hilo: %d\n", numHilo);
	printf("Prioridad del hilo: %c\n", priority);

	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	if(argc!=2){
		printf("Uso: %s <num_hilos>\n", argv[0]);
		exit(-1);
	}
	int num_hilos=atoi(argv[1]);
	struct threadArgs *args;
	pthread_t threads[num_hilos];
	for(int i=0;i< num_hilos;i++){
		args=malloc(sizeof(struct threadArgs));
		args->id=i;
		if(i%2==0){
			args->priority='P';
		}
		else{
			args->priority='N';
		}
		pthread_create(&threads[i], NULL, thread_usuario, (void*) args);
	}
	for(int i=0;i< num_hilos;i++){
		pthread_join(threads[i], NULL);
	}
	return 0;
}
