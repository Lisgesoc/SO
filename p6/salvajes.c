#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>

#define NUMITER 3

int flag=0;
void *ptr=NULL;	
sem_t *sem_mutx=NULL;
sem_t *sem_cook=NULL;
sem_t *sem_filled=NULL;

int getServingFromPot(void){
	//TODO implementar programa
	/* Acceso a memoria compartida (ya protegida por mutex en savages()). */
	(*(int*)ptr)=(*(int*)ptr)-1;
	int c=(*(int*)ptr);
	return c;
}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{
	for (int i = 0; i < NUMITER; i++){
		/* Entrada a sección crítica. */
		sem_wait(sem_mutx);
		int c=(*(int*)ptr);
		while(c==0){
			/* Si no hay comida, liberamos mutex y avisamos al cocinero. */
			sem_post(sem_mutx);
			sem_post(sem_cook);
			/* Esperamos a que el cocinero llene el puchero. */
			sem_wait(sem_filled);
			sem_wait(sem_mutx);
			c=(*(int*)ptr);
		}
		getServingFromPot();
		sem_post(sem_mutx);
		eat();
	}
}

int main(int argc, char *argv[])
{
	int fd=-1;
	/* Abrir memoria compartida existente. */
	fd=shm_open("/capacidad", O_RDWR, 0666);
	if(fd==-1){
		perror("Error al abrir los recursos.\n Ejecute el programa cocinero antes de este \n");
		return 0;
	}


	/* Mapear memoria compartida. */
	ptr=mmap(NULL, sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(ptr==MAP_FAILED){
		perror("Error al abrir el mapa\n");
		flag=1;
	}


	/* Abrir semáforos existentes. */
	sem_mutx=sem_open("/semaforo",O_RDWR);
	sem_cook=sem_open("/semcook",O_RDWR);
	sem_filled=sem_open("/semfilled",O_RDWR);
	if(sem_mutx==SEM_FAILED || sem_cook==SEM_FAILED || sem_filled==SEM_FAILED){
		perror(" Error al abrir el semaforo\n");
		flag = 1;
	}

	if(flag==0){
		printf("Sistema preparado\n");
		savages();
		printf("Fin del bucle, cerrando...\n");
	}

	if(sem_mutx!=SEM_FAILED){
		sem_close(sem_mutx);
	}
	if(sem_cook!=SEM_FAILED){
		sem_close(sem_cook);
	}
	if(sem_filled!=SEM_FAILED){
		sem_close(sem_filled);
	}

	if(ptr!=MAP_FAILED){
		munmap(ptr,sizeof(int));
	}

	if(fd!=-1){
		close(fd);
	}

	return 0;
}
