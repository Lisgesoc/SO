#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/* Manejador de señal que imprime un mensaje al recibirla. */
void recibir_mensaje(int num){ printf("Señal recibida\n");}
/* Función ejecutada por el proceso B (receptor).
 * Configura el manejador de señales y espera en un bucle infinito.
 */

void procB(){

    int id=getpid();
    printf("B id: %d\n",id);

    /* Establecemos el manejador para SIGUSR1.
     * Cuando llegue SIGUSR1, se ejecutará recibir_mensaje.
     */
    signal(SIGUSR1, recibir_mensaje);

    /* Bucle infinito esperando señales. */
    while(1) { sleep(1); }
}

/* Función ejecutada por el proceso A (emisor).
 * Envía señales SIGUSR1 al proceso B periódicamente.
 */
void procA(int Bid){
    sleep(1);
    int id=getpid();
    printf("A id: %d\n",id);

    /* Enviamos señales SIGUSR1 al proceso B (identificado por Bid).
     * kill envía una señal a un proceso o grupo de procesos.
     */
    kill(Bid, SIGUSR1);
    sleep(4);
    kill(Bid, SIGUSR1);
    sleep(2);
    kill(Bid, SIGUSR1);
}
#define numP 2
int main(int argc, char **argv)
{
    int childID[numP];
    for(int i=0;i<numP;i++){
        /* Creamos procesos hijos. */
        childID[i]=fork();

        if(childID[i]<0){ 
            perror("fork");
            exit(EXIT_FAILURE);
        }else if(childID[i]==0){
            /* Código del hijo */
            if(i==0){
                /* El primer hijo ejecuta procB (el receptor). */
                procB();exit(0);
            }else{
                /* El segundo hijo ejecuta procA (el emisor), pasándole el PID del primer hijo.
                 * Nota: childID[0] aquí es válido porque el padre lo rellenó en la iteración anterior
                 * y el fork duplica la memoria, PERO hay una condición de carrera potencial:
                 * Si i=1 se ejecuta antes de que i=0 termine en el padre, childID[0] podría no estar listo?
                 * No, porque es secuencial en el padre. childID[0] ya tiene valor cuando i=1.
                 * PERO, el hijo i=1 hereda la memoria del padre tal cual estaba en el momento del fork.
                 * Así que sí, childID[0] es visible para el segundo hijo.
                 */
                procA(childID[0]);exit(0);
            }
        }
    }
    /* El padre espera a que terminen todos los hijos. */
    for(int i=0;i<numP;i++){
        waitpid(childID[i],NULL,0);
    }
    return 0;
}