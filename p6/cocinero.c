#define _POSIX_C_SOURCE 200809L//Para poder usar la estruct sigaction 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>
#define M 10

int finish = 0;
void *ptr=NULL;
sem_t *sem_mutx=NULL;
sem_t *sem_cook=NULL;
sem_t *sem_filled=NULL;

void putServingsInPot(int servings){
	//TODO implementar programa
	/* Sección crítica: Acceso exclusivo a la memoria compartida usando semáforo mutex. */
	sem_wait(sem_mutx);
	(*(int *)ptr)=servings;
	printf("El cocinero rellena el puchero\n");
	sem_post(sem_mutx);

}

void cook(void)
{
	while(!finish) {
		/* Espera a que el puchero esté vacío (sem_cook). */
		sem_wait(sem_cook);

		if(finish==1) break;

		putServingsInPot(M);

		/* Señaliza que el puchero está lleno (sem_filled). */
		sem_post(sem_filled);
	}
}

void handler(int signo)
{
	finish = 1;
}

int main(int argc, char *argv[])
{
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;

	/* Configuración de manejo de señales para terminar limpiamente. */
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);

	int fd=-1;
	/* shm_open: Crea objeto de memoria compartida POSIX. */
	fd=shm_open("/capacidad", O_CREAT|O_RDWR, 0666);
	if(fd==-1){
		perror("DEBUG: Error al abrir la region de mem compartida\n");
		finish = 1;
	}
	/* ftruncate: Define el tamaño del objeto de memoria compartida. */
	if(ftruncate(fd,sizeof(int))==-1){
		perror("DEBUG: Error al reservar la memoria compartidan\n");
		finish = 1;
	}


	/* mmap: Mapea el objeto de memoria compartida al espacio de direcciones. */
	ptr=mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(ptr==MAP_FAILED){
		perror("DEBUG: Error al mapear el archivo a un puntero\n");
	/* sem_open: Crea semáforos con nombre para sincronización. */
		finish = 1;
	}


	sem_mutx=sem_open("/semaforo",O_CREAT|O_RDWR,0666,1);
	sem_cook=sem_open("/semcook",O_CREAT|O_RDWR,0666,0);
	sem_filled=sem_open("/semfilled",O_CREAT|O_RDWR,0666,0);
	if(sem_mutx==SEM_FAILED || sem_cook==SEM_FAILED || sem_filled==SEM_FAILED){
		perror("DEBUG: Error al crcear el semaforo\n");
		finish = 1;
	}

	printf("Sistema preparado\n");

	cook();

	printf("Fin del bucle, cerrando...\n");
/* Limpieza de recursos: cerrar y eliminar semáforos y memoria compartida. */
	
	if(sem_mutx!=SEM_FAILED){
		sem_close(sem_mutx);
		sem_unlink("/semaforo");
	}	
	if(sem_cook!=SEM_FAILED){
		sem_close(sem_cook);
		sem_unlink("/semcook");
	}	
	if(sem_filled!=SEM_FAILED){
		sem_close(sem_filled);
		sem_unlink("/semfilled");
	}
	printf("DEBUG: Semaforo cerrado\n");

	if(ptr!=MAP_FAILED){
		munmap(ptr,sizeof(int));
	}
	if(fd!=-1){
		close(fd);
	}


	shm_unlink("/capacidad");
	printf("DEBUG: Mem compartida cerrada\n");
	
	return 0;
}
