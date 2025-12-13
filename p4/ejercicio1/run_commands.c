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
    /* fork crea un nuevo proceso duplicando el actual.
     * Devuelve:
     * - < 0: Error.
     * - 0: Estamos en el proceso hijo.
     * - > 0: Estamos en el proceso padre (devuelve el PID del hijo).
     */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        /* execvp reemplaza la imagen del proceso actual con un nuevo programa.
         * argv[0] es el nombre del comando.
         * argv es el array de argumentos (incluyendo el nombre).
         * Si tiene éxito, execvp NO retorna.
         */
        if (execvp(argv[0], argv) < 0) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }else{
        // Parent process
        // El padre continúa ejecución y retorna el PID del hijo lanzado.
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
    /* malloc reserva memoria dinámica para el array de punteros a cadenas. */
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    /* ... lógica de parseo ... */

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            /* realloc redimensiona el bloque de memoria preservando el contenido. */
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        
        /* ... */

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
    /* Abrimos el archivo de comandos en modo lectura. */
    FILE *file=fopen(filename,"r");
    if(file==NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    int cont=0;
    int procID[MAX_CMD]; // Array para almacenar los PIDs de los procesos lanzados

    char line[MAX_LINE_LENGTH];
    /* Leemos el archivo línea a línea.
     * fgets lee hasta encontrar un salto de línea o llenar el buffer.
     */
    while(fgets(line,sizeof(line),file) && cont<MAX_CMD-1){
        size_t len =strlen(line);
        /* Eliminamos el salto de línea final si existe. */
        line[len-1]='\0';
        printf("Read line: %s\n", line);
        char **cmd_argv;
        int cmd_argc;
        
        /* Parseamos la línea para obtener los argumentos del comando. */
        cmd_argv=parse_command(line,&cmd_argc);
        
        /* Lanzamos el comando en un nuevo proceso (fork + exec). */
        pid_t pid = launch_command(cmd_argv);
        procID[cont]=pid;
        cont++;

        /* Si la ejecución es ordenada (-b NO especificado), esperamos a que termine
         * el proceso actual antes de lanzar el siguiente.
         */
        if(ordered){
            waitpid(pid, NULL, 0);
            printf("Process %d finished\n", pid);
        }

        /* Liberamos la memoria reservada por parse_command. */
        for (int i = 0; cmd_argv[i] != NULL; i++) {
            free(cmd_argv[i]);
        }
        free(cmd_argv);
    }

    /* Si la ejecución es concurrente (no ordenada), esperamos a todos los hijos al final. */
    if(!ordered){
        for(int i=0;i<cont;i++){
            /* waitpid(0, ...) espera a cualquier proceso hijo del grupo. */
            int pid=waitpid(0, NULL, 0);
            int idx;
            /* Buscamos qué comando correspondía a este PID para imprimir el mensaje. */
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
    int ordered=1; // Por defecto, ejecución secuencial (ordenada)
    char *argumento;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    }


    int opt;
    /* Parseo de argumentos:
     * -x: Ejecutar un solo comando.
     * -s: Ejecutar script (lista de comandos en archivo).
     * -b: Ejecución en background (concurrente/no ordenada).
     */
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
        case 0: // Modo comando único (-x)
            cmd_argv=parse_command(argumento,&cmd_argc);
            pid_t pid = launch_command(cmd_argv);
            waitpid(pid, NULL, 0); // Esperamos a que termine
            for (i = 0; cmd_argv[i] != NULL; i++)
                free(cmd_argv[i]);
            free(cmd_argv);
        break;
        case 1: // Modo script (-s)
            proces_file(argumento,ordered);
        break;
        default:
            fprintf(stderr, "No execution mode selected. Use -x or -s.\n");
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}