#define _POSIX_C_SOURCE 200809L//Para poder usar la estruct sigaction 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
pid_t pid;

/* Manejador de señal (handler).
 * Se ejecuta cuando llega la señal SIGALRM.
 * Mata al proceso hijo usando kill y su PID.
 */
void handler(int a){
    kill(pid,SIGKILL);
}

/*programa que temporiza la ejecución de un proceso hijo */


int main(int argc, char **argv)
{
	if(argc < 2){
		fprintf(stderr, "Uso: %s <command>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* Creamos un proceso hijo. */
	pid = fork();
	argv++; // Avanzamos argv para apuntar al comando a ejecutar (saltamos el nombre del programa actual)
	
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        /* El hijo ejecuta el comando pasado como argumento. */
        if (execvp(argv[0], argv) < 0) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    int st;
    
    //signal(SIGALRM, handler);

    /* Configuración del manejador de señales con sigaction (más robusto que signal).
     * Queremos capturar SIGALRM.
     */
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));// necesita la libreria string
    sa.sa_handler=handler; // Función a ejecutar cuando llegue la señal
    sigemptyset(&sa.sa_mask); // No bloqueamos señales adicionales durante la ejecución del handler
    sa.sa_flags=SA_RESTART; //para que funcione bien waitpid (reinicia llamadas al sistema interrumpidas)
    sigaction(SIGALRM, &sa, NULL);

    /* Programamos una alarma para dentro de 5 segundos.
     * El sistema enviará SIGALRM al proceso cuando pase el tiempo.
     */
    alarm(5);   
    
    /* Esperamos a que el hijo termine.
     * Si salta la alarma antes, el handler se ejecuta (matando al hijo) y waitpid podría ser interrumpido
     * (pero SA_RESTART lo reinicia) o retornar el estado del hijo muerto.
     */
    if(waitpid(pid, &st, 0)==-1){
        perror("waitpid");
    }else if(WIFEXITED(st)){
        /* WIFEXITED: verdadero si el hijo terminó normalmente (exit o return). */
        printf("El proceso termino de manera natural\n");
    }else if(WIFSIGNALED(st)){
        /* WIFSIGNALED: verdadero si el hijo terminó por una señal no capturada. */
        printf("El proceso termino por una señal\n");
    }

    
	return 0;
}
