#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <err.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

#define N 15

int aforo;
pthread_mutex_t mutAccess;
int vipNum;
pthread_cond_t vipCond;
pthread_cond_t nrmCond;

#define MAX 50

int vipIdx;
pthread_mutex_t mutVipIds;
pthread_cond_t vipIDCond;

int nrmIdx;
pthread_mutex_t mutNrmIds;
pthread_cond_t nrmIDCond;

int norm[MAX+1];
int vip[MAX+1];

struct client{
	int isvip;
	int id;
};

void enter_normal_client(int id)
{
	/* Bloqueo del mutex para proteger el acceso a la variable compartida 'nrmIdx' y 'norm'.
	 * En programación concurrente, es crucial proteger las secciones críticas.
	 */
	pthread_mutex_lock(&mutNrmIds);
	
	/* Espera condicional: mientras no sea el turno de este cliente (id != norm[nrmIdx]),
	 * el hilo se bloquea en la variable de condición 'nrmIDCond'.
	 * pthread_cond_wait libera el mutex atómicamente y pone el hilo a dormir.
	 * Al despertar, vuelve a adquirir el mutex antes de retornar.
	 */
	while(id!=norm[nrmIdx]){
		pthread_cond_wait(&nrmIDCond,&mutNrmIds);
	}
	pthread_mutex_unlock(&mutNrmIds); // Liberamos mutex tras verificar turno

	/* Bloqueo del mutex de acceso a la discoteca (aforo). */
	pthread_mutex_lock(&mutAccess);
	
	/* Espera si la discoteca está llena (aforo == 0) o si hay VIPs esperando (vipNum > 0).
	 * Los clientes normales tienen menor prioridad que los VIPs.
	 */
	while (aforo==0 || vipNum>0){
		pthread_cond_wait(&nrmCond,&mutAccess);
	}
	//printf("Aforo %d\n",aforo);
	
	/* Sección crítica: actualizar estado de la cola y aforo. */
	pthread_mutex_lock(&mutNrmIds);
	nrmIdx++; // Avanzamos el índice de clientes normales atendidos
	pthread_cond_broadcast(&nrmIDCond); // Despertamos a todos los hilos esperando turno para que verifiquen
	aforo--; // Decrementamos el aforo disponible
	printf("Client %2d (not vip) enter\n",id);
	
	pthread_mutex_unlock(&mutAccess); // Liberamos mutex de acceso
	pthread_mutex_unlock(&mutNrmIds); // Liberamos mutex de IDs
}

void enter_vip_client(int id)
{
	//Bloqueo mutex de acceso y pongo un vip en cola
	/* Incrementamos el contador de VIPs esperando.
	 * Esto bloquea la entrada de clientes normales en su condición de espera.
	 */
	pthread_mutex_lock(&mutAccess);
	vipNum++;
	pthread_mutex_unlock(&mutAccess);

	//Bloqueo mutex de fila vip para comprobar si le toca 
	//sino espera a que el vip idx le despierte
	//si es su turno no suelta el mutex hasta el final del acceso
	pthread_mutex_lock(&mutVipIds);
	while(id!=vip[vipIdx]){
		pthread_cond_wait(&vipIDCond,&mutVipIds);
	}
	pthread_mutex_unlock(&mutVipIds);

	//Bloquea el mutex de acceso y comprueba el aforo
	//espera a que le despierte la funcion exit
	pthread_mutex_lock(&mutAccess);
	while(aforo<=0){
		pthread_cond_wait(&vipCond,&mutAccess);
	}
	//printf("Aforo %d\n",aforo);
	aforo--; // Ocupa plaza
	vipNum--; // Deja de esperar (entra)
	
	/* Si ya no quedan VIPs esperando y hay sitio, despertamos a los normales.
	 * pthread_cond_signal despierta al menos a un hilo esperando en nrmCond.
	 */
	if(vipNum==0 && aforo>0){
		//Despierta a norm si al salir el de la vip fila no quedan mas vip
		pthread_cond_signal(&nrmCond);
	}
	pthread_mutex_unlock(&mutAccess);

	printf("Client %2d (  vip  ) enter\n",id);
	
	pthread_mutex_lock(&mutVipIds);
	//Aumenta el indice de la fila de vips para que le toque al siguiente vip
	vipIdx++;
	pthread_cond_broadcast(&vipIDCond); // Avisamos al siguiente VIP
	pthread_mutex_unlock(&mutVipIds);
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	//Tomamos el mutex de acceso para aumentar el aforo disp 
	//dispertamos a las colas (nrm o vip)
	pthread_mutex_lock(&mutAccess);
	aforo++;
	if(vipNum==0){
		pthread_cond_signal(&nrmCond);
	}else{
		pthread_cond_signal(&vipCond);
	}
	pthread_mutex_unlock(&mutAccess);
	printf("Client %2d (%s) exiting disco\n",id, VIPSTR(isvip));
}

void *client(void *arg)
{
	//Copia del puntero y de los parametros para poder liberear la mem
	struct client *c =arg;
	int isvip=c->isvip;
	int id=c->id;
	free(c);

	printf("Cliente %d (%s) en cola\n",id, VIPSTR(isvip));

	//Secuencia de procesado del hilo
	if(isvip){
		enter_vip_client(id);
	}else{
		enter_normal_client(id);
	}
	dance(id,isvip);
	disco_exit(id,isvip);
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t tid[MAX*2];
	
	/* Inicialización de variables globales de control. */
	aforo=N;
	vipNum=0;
	nrmIdx=0;
	vipIdx=0;
	
	/* Inicialización de mutexes y variables de condición.
	 * Es fundamental inicializarlos antes de crear cualquier hilo que los use.
	 */
	pthread_mutex_init(&mutAccess,NULL);
	pthread_mutex_init(&mutVipIds,NULL);
	pthread_mutex_init(&mutNrmIds,NULL);
	pthread_cond_init(&vipCond,NULL);
	pthread_cond_init(&nrmCond,NULL);
	pthread_cond_init(&vipIDCond,NULL);
	pthread_cond_init(&nrmIDCond,NULL);
	
	if(argc<2){
		errx(EXIT_FAILURE,"Usege: %s <file>",argv[0]);
		return 0;
	}
	
	/* Lectura del archivo de entrada con la secuencia de clientes. */
	FILE *file =fopen(argv[1],"r");
	int M;
	fscanf(file,"%d",&M); // Número total de clientes

	int totVip=0,totNrm=0;
	struct client *c;
	
	/* Bucle de creación de hilos (clientes). */
	for(int i=0;i<M;i++){
		int v;
		fscanf(file,"%d",&v); // Leemos tipo de cliente (1=VIP, 0=Normal)
		
		/* Reservamos memoria para los argumentos del hilo.
		 * Cada hilo necesita su propia estructura para evitar condiciones de carrera.
		 */
		c=malloc(sizeof(struct client));
		
		/* Asignamos IDs y guardamos en los arrays de orden (vip[] o norm[]).
		 * Protegemos el acceso a estos arrays con mutexes.
		 */
		if(v==1){
			pthread_mutex_lock(&mutVipIds);
			vip[totVip]=i;
			pthread_mutex_unlock(&mutVipIds);
			totVip++;
		}else{
			pthread_mutex_lock(&mutNrmIds);
			norm[totNrm]=i;
			pthread_mutex_unlock(&mutNrmIds);
			totNrm++;
		}

		c->id=i;
		c->isvip=v;
		/* Creamos el hilo cliente. */
		pthread_create(&tid[i],NULL,client,(void *)c);
	}
	fclose(file);
	
	/* Esperamos a que terminen todos los hilos. */
	for(int i=0;i<M;i++){
		pthread_join(tid[i],NULL);
	}

	/* Destruimos los recursos de sincronización al finalizar. */
	pthread_mutex_destroy(&mutAccess);
	pthread_mutex_destroy(&mutNrmIds);
	pthread_mutex_destroy(&mutVipIds);
	pthread_cond_destroy(&vipCond);
	pthread_cond_destroy(&nrmCond);
	pthread_cond_destroy(&vipIDCond);
	pthread_cond_destroy(&nrmIDCond);

	return 0;
}
