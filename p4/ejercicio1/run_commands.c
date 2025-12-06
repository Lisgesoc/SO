#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>

#define MAX_LINE_LENGTH 4096 // Maximum length for a command line in console
#define MAX_CMD 100

pid_t launch_command(char** argv){
    /* To be completed */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (execvp(argv[0], argv) < 0) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }else{
    }
    return pid;   
}

char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}

void proces_file(const char* filename, int ordered){
    FILE *file=fopen(filename,"r");
    if(file==NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    int cont=0;
    int procID[MAX_CMD];

    char line[MAX_LINE_LENGTH];
    while(fgets(line,sizeof(line),file) && cont<MAX_CMD-1){
        size_t len =strlen(line);
        line[len-1]='\0';
        printf("Read line: %s\n", line);
        char **cmd_argv;
        int cmd_argc;
        cmd_argv=parse_command(line,&cmd_argc);
        pid_t pid = launch_command(cmd_argv);
        procID[cont]=pid;
        cont++;

        if(ordered){
            waitpid(pid, NULL, 0);
            printf("Process %d finished\n", pid);
        }

        for (int i = 0; cmd_argv[i] != NULL; i++) {
            free(cmd_argv[i]);
        }
        free(cmd_argv);
    }

    if(!ordered){
        for(int i=0;i<cont;i++){
            int pid=waitpid(0, NULL, 0);
            int idx;
            for(int j=0;j<cont;j++){
                if(procID[j]==pid){
                    idx=j;
                    break;
                }
            }
            printf("Process number %d with PID %d finished\n", idx+1, pid);
        }
    }

    fclose(file);     
}

int main(int argc, char *argv[]) {
    char **cmd_argv;
    int cmd_argc;
    int i;
    int exe_mode;
    int ordered=1;
    char *argumento;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    }


    int opt;
	while ((opt = getopt(argc, argv, "hx:s:b")) != -1)
	{
		switch (opt){
		case 'h':
            fprintf(stderr, "Usage: %s [-h] [-x \"command\"] [-s filename]\n", argv[0]);
        break;
        case 'x':
            printf("Option -x with argument: %s\n", optarg);
            exe_mode = 0;
            argumento=optarg;

        break;
        case 's':
            printf("Option -s with argument: %s\n", optarg);
            exe_mode = 1;
            argumento=optarg;
        break;
        case 'b':
            ordered = 0;
        break;
        default:
            return EXIT_FAILURE;
        }
    }


    switch(exe_mode){
        case 0:
            cmd_argv=parse_command(argumento,&cmd_argc);
            pid_t pid = launch_command(cmd_argv);
            waitpid(pid, NULL, 0);
            for (i = 0; cmd_argv[i] != NULL; i++)
                free(cmd_argv[i]);
            free(cmd_argv);
        break;
        case 1:
            proces_file(argumento,ordered);
        break;
        default:
            fprintf(stderr, "No execution mode selected. Use -x or -s.\n");
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}