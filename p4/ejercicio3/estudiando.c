#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void recibir_mensaje(int num){ printf("Señal recibida\n");}

void procB(){

    int id=getpid();
    printf("B id: %d\n",id);

    signal(SIGUSR1, recibir_mensaje);

    while(1) { sleep(1); }
}

void procA(int Bid){
    sleep(1);
    int id=getpid();
    printf("A id: %d\n",id);

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
        childID[i]=fork();

        if(childID[i]<0){ 
            perror("fork");
            exit(EXIT_FAILURE);
        }else if(childID[i]==0){
            if(i==0){
                procB();exit(0);
            }else{
                procA(childID[0]);exit(0);
            }
        }
    }
    for(int i=0;i<numP;i++){
        waitpid(childID[i],NULL,0);
    }
    return 0;
}