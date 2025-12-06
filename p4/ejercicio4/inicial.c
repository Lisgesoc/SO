#define _POSIX_C_SOURCE 200809L//Para poder usar la estruct sigaction 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

int fd_glob;

void handler(){
    write(fd_glob,"00000",5);
}

int main(void)
{
    int fd1,fd2,i,pos;
    char c;
    char buffer[6];
    int id=getpid();

    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    write(fd1, "00000", 5);
    //signal(SIGUSR1,handler);

    struct sigaction sa;
    memset(&sa,0,sizeof(sa));// necesita la libreria string
    sa.sa_handler=handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART; //para que funcione bien waitpid
    sigaction(SIGUSR1, &sa, NULL);

    for (i=1; i < 10; i++) {
        pos = lseek(fd1, 0, SEEK_CUR);
        if (fork() == 0) {
            /* Child */
            sprintf(buffer, "%d", i*11111);
            lseek(fd1, 5+(i-1)*10, SEEK_SET);
            write(fd1, buffer, 5);
            close(fd1);
            kill(id,SIGUSR1);
            exit(0);
        } else {
            fd_glob=fd1;
            lseek(fd_glob,i*10,SEEK_SET);
            /* Parent */
            lseek(fd1, 5, SEEK_CUR);
        }
    }

	//wait for all childs to finish
    while (wait(NULL) != -1);

    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}
