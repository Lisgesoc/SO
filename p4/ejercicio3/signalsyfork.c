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
	pid = fork();
	argv++;
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (execvp(argv[0], argv) < 0) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    int st;
    
    //signal(SIGALRM, handler);

    struct sigaction sa;
    memset(&sa,0,sizeof(sa));// necesita la libreria string
    sa.sa_handler=handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART; //para que funcione bien waitpid
    sigaction(SIGALRM, &sa, NULL);

    alarm(5);   
    if(waitpid(pid, &st, 0)==-1){
        perror("waitpid");
    }else if(WIFEXITED(st)){
        printf("El proceso termino de manera natural\n");
    }else if(WIFSIGNALED(st)){
        printf("El proceso termino por una señal\n");
    }

    
	return 0;
}
