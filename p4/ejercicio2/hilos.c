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
	
	/* Obtenemos el identificador del hilo actual (pthread_t). */
	pthread_t id = pthread_self();
	
	/* Liberamos la memoria de los argumentos, ya que fue reservada con malloc en el main.
	 * Cada hilo es responsable de liberar su propia estructura de argumentos en este diseño.
	 */
	free(arg);

	if(priority=='N'){
		sleep(3); // Simula trabajo o espera
	}

	printf("Identificador del hilo: %lu\n", id);
	printf("Numero del hilo: %d\n", numHilo);
	printf("Prioridad del hilo: %c\n", priority);

	/* Termina el hilo. */
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
	pthread_t threads[num_hilos]; // Array para guardar los identificadores de los hilos
	
	for(int i=0;i< num_hilos;i++){
		/* Reservamos memoria para los argumentos de cada hilo.
		 * Es necesario usar memoria dinámica para evitar condiciones de carrera
		 * si pasáramos la dirección de una variable local que cambia en el bucle.
		 */
		args=malloc(sizeof(struct threadArgs));
		args->id=i;
		if(i%2==0){
			args->priority='P';
		}
		else{
			args->priority='N';
		}
		
		/* Creación del hilo.
		 * &threads[i]: puntero donde se guardará el ID del hilo creado.
		 * NULL: atributos por defecto.
		 * thread_usuario: función que ejecutará el hilo.
		 * (void*) args: argumentos pasados a la función.
		 */
		pthread_create(&threads[i], NULL, thread_usuario, (void*) args);
	}
	
	/* Esperamos a que terminen todos los hilos.
	 * pthread_join bloquea al hilo llamante (main) hasta que el hilo especificado termine.
	 */
	for(int i=0;i< num_hilos;i++){
		pthread_join(threads[i], NULL);
	}
	return 0;
}
