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
	pthread_mutex_lock(&mutNrmIds);
	while(id!=norm[nrmIdx]){
		pthread_cond_wait(&nrmIDCond,&mutNrmIds);
	}
	pthread_mutex_unlock(&mutNrmIds);

	pthread_mutex_lock(&mutAccess);
	while (aforo==0 || vipNum>0){
		pthread_cond_wait(&nrmCond,&mutAccess);
	}
	//printf("Aforo %d\n",aforo);
	
	pthread_mutex_lock(&mutNrmIds);
	nrmIdx++;
	pthread_cond_broadcast(&nrmIDCond);
	aforo--;
	printf("Client %2d (not vip) enter\n",id);
	pthread_mutex_unlock(&mutAccess);
	pthread_mutex_unlock(&mutNrmIds);
}

void enter_vip_client(int id)
{
	//Bloqueo mutex de acceso y pongo un vip en cola
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
	aforo--;
	vipNum--;
	if(vipNum==0 && aforo>0){
		//Despierta a norm si al salir el de la vip fila no quedan mas vip
		pthread_cond_signal(&nrmCond);
	}
	pthread_mutex_unlock(&mutAccess);

	printf("Client %2d (  vip  ) enter\n",id);
	
	pthread_mutex_lock(&mutVipIds);
	//Aumenta el indice de la fila de vips para que le toque al siguiente vip
	vipIdx++;
	pthread_cond_broadcast(&vipIDCond);
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
	aforo=N;
	vipNum=0;
	nrmIdx=0;
	vipIdx=0;
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
	FILE *file =fopen(argv[1],"r");
	int M;
	fscanf(file,"%d",&M);

	int totVip=0,totNrm=0;
	struct client *c;
	for(int i=0;i<M;i++){
		int v;
		fscanf(file,"%d",&v);
		c=malloc(sizeof(struct client));
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
		pthread_create(&tid[i],NULL,client,(void *)c);
	}
	fclose(file);
	for(int i=0;i<M;i++){
		pthread_join(tid[i],NULL);
	}

	pthread_mutex_destroy(&mutAccess);
	pthread_mutex_destroy(&mutNrmIds);
	pthread_mutex_destroy(&mutVipIds);
	pthread_cond_destroy(&vipCond);
	pthread_cond_destroy(&nrmCond);
	pthread_cond_destroy(&vipIDCond);
	pthread_cond_destroy(&nrmIDCond);

	return 0;
}
