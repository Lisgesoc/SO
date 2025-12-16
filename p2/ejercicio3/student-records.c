#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"
#include <getopt.h>


/*
* Assume lines in the text file are no larger than 100 chars
*/
#define MAXLEN_LINE_FILE 100


/* 
 * Función auxiliar para procesar una línea de texto.
 * Conceptos SO: Gestión de memoria dinámica (malloc/free), manipulación de cadenas.
 */
void registraLinea(FILE *file, student_t *student){
	char *token[4];
	// Reservamos memoria para leer la línea completa.
	char *line=malloc(MAXLEN_LINE_FILE*sizeof(char));
	char *cpy=line; // Guardamos el puntero original para poder hacer free() después, ya que strsep mueve 'line'.
	
	// fgets lee del stream 'file' hasta encontrar un salto de línea o alcanzar MAXLEN_LINE_FILE.
	fgets(line, MAXLEN_LINE_FILE, file);
	
	// Parseamos la línea separando por el delimitador ':'
	for(int j=0;j<4;j++){
		token[j]=strsep(&line,":");
	}

	// Asignamos memoria dinámica para los campos de longitud variable (strings).
	// Es importante sumar +1 para el carácter terminador nulo '\0'.
	student->first_name=malloc(strlen(token[2])+1);
	student->last_name=malloc(strlen(token[3])+1);

	// Convertimos y copiamos los datos a la estructura.
	student->student_id=atoi(token[0]);
	strcpy(student->NIF, token[1]);
	strcpy(student->first_name,token[2]);
	strcpy(student->last_name,token[3]);

	// Liberamos la memoria temporal de la línea leída.
	free(cpy);
}

/*
 * Libera la memoria interna de un estudiante.
 * Conceptos SO: Prevención de fugas de memoria (memory leaks).
 */
void free_student(student_t* student){
	free(student->first_name);
	free(student->last_name);
}

/*
 * Libera todo el array de estudiantes.
 * Primero libera el contenido de cada estructura y finalmente el array contenedor.
 */
void free_student_array(student_t* student_array, int nr_entries){
	for(int i=0; i<nr_entries; i++){
		free_student(&student_array[i]);
	}
	free(student_array);
}

/*
 * Lee el fichero de texto completo.
 * Conceptos SO: E/S formateada (fscanf).
 */
student_t* read_student_text_file(FILE* students, int* nr_entries){
	// Leemos el número de entradas (primera línea del fichero).
	fscanf(students , "%d\n", nr_entries);
	
	// Reservamos memoria contigua para el array de estructuras.
	student_t* student_array=malloc((*nr_entries)*sizeof(student_t));
	
	for(int i=0; i<*nr_entries; i++){
		registraLinea(students, &student_array[i]);
	}
	return student_array;
}

/*
 * Lee el fichero binario completo.
 * Conceptos SO: E/S binaria (fread), punteros a ficheros.
 */
student_t* read_student_binary_file(FILE* students, int* nr_entries){
	// fread lee bloques de bytes crudos. Aquí leemos un entero (4 bytes típicamente).
	fread(nr_entries, sizeof(int), 1, students);
	printf("Number of entries: %d\n", *nr_entries);
	
	student_t* student_array=malloc((*nr_entries)*sizeof(student_t));
	
	for(int i=0; i<*nr_entries; i++){
		// Leemos datos de tamaño fijo directamente a la memoria de la estructura.
		fread(&student_array[i].student_id, sizeof(int), 1, students);
		fread(&student_array[i].NIF, sizeof(student_array[i].NIF), 1, students);

		//first_name: Lectura carácter a carácter para cadenas de longitud variable en binario.
		char buffer_first_name[MAXLEN_LINE_FILE];
		int index_fn=0;
		char ch_fn;
		do{
			fread(&ch_fn, sizeof(char), 1, students);
			buffer_first_name[index_fn]=ch_fn;
			index_fn++;
		}while(ch_fn!='\0' && index_fn<MAXLEN_LINE_FILE);
		
		// Una vez tenemos la cadena completa, reservamos la memoria exacta necesaria.
		student_array[i].first_name=malloc(strlen(buffer_first_name)+1);
		strcpy(student_array[i].first_name, buffer_first_name);

		//last_name: Mismo proceso que first_name.
		char buffer_last_name[MAXLEN_LINE_FILE];
		int index_ln=0;
		char ch_ln;
		do{
			fread(&ch_ln, sizeof(char), 1, students);
			buffer_last_name[index_ln]=ch_ln;
			index_ln++;
		}while(ch_ln!='\0' && index_ln<MAXLEN_LINE_FILE);
		
		student_array[i].last_name=malloc(strlen(buffer_last_name)+1);
		strcpy(student_array[i].last_name, buffer_last_name);
	}
	return student_array;
}


int print_text_file(char *path){

	// fopen abre el fichero. "r" indica modo lectura de texto.
	FILE *file=fopen(path, "r");
	if(file==NULL){
		// perror imprime el error del sistema asociado a la última llamada fallida (errno).
		perror("Error opening file");
		return EXIT_FAILURE;
	}

	int numStudents;
	student_t *students;
	students=read_student_text_file(file, &numStudents);

	printf("Number of students: %d\n", numStudents);



	for(int i=0; i<numStudents; i++){
		
		printf("\n--- Student %d ---\n", i+1);
		printf("Student ID: %d\n", students[i].student_id);
		printf("NIF: %s\n", students[i].NIF);
		printf("First Name: %s\n", students[i].first_name);
		printf("Last Name: %s\n", students[i].last_name);

	}

	free_student_array(students, numStudents);

	// Es fundamental cerrar el fichero para liberar el descriptor de fichero del SO.
	fclose(file);
	return 0;
}

int print_binary_file(char *path)
{
	/* To be completed  (part C) */

	// "rb" indica modo lectura binaria. Es crucial en Windows/WSL para evitar traducciones de saltos de línea.
	FILE*file=fopen(path, "rb");
	if(file==NULL){
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	int numStudents;
	student_t *students;
	students=read_student_binary_file(file, &numStudents);
	for(int i=0; i<numStudents; i++){
		
		printf("\n--- Student %d ---\n", i+1);
		printf("Student ID: %d\n", students[i].student_id);
		printf("NIF: %s\n", students[i].NIF);
		printf("First Name: %s\n", students[i].first_name);
		printf("Last Name: %s\n", students[i].last_name);

	}

	free_student_array(students, numStudents);

	fclose(file);

	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part B) */
	FILE *inFile=fopen(input_file, "r");
	if(inFile==NULL){
		perror("Error opening input file");
		return EXIT_FAILURE;
	}
	// "wb" abre para escritura binaria, truncando el fichero si existe o creándolo si no.
	FILE *outFile=fopen(output_file, "wb");
	if(outFile==NULL){
		perror("Error opening output file");
		return EXIT_FAILURE;
	}

	int numStudents;
	student_t *students;
	// Cargamos los datos en memoria desde texto primero.
	students=read_student_text_file(inFile, &numStudents);
	
	// Escribimos el número total de estudiantes al inicio del fichero binario.
	fwrite(&numStudents, sizeof(int), 1, outFile);

	student_t student;


	for(int i=0; i<numStudents; i++){

		// Escribimos los campos. fwrite vuelca el contenido de la memoria directamente al disco.
		fwrite(&students[i].student_id, sizeof(int), 1, outFile);
		fwrite(&students[i].NIF, sizeof(students[i].NIF), 1, outFile);
		// Para las cadenas, escribimos longitud + 1 para incluir el '\0'.
		fwrite(students[i].first_name, strlen(students[i].first_name)+1, 1, outFile);
		fwrite(students[i].last_name, strlen(students[i].last_name)+1, 1, outFile);

	}

	free_student_array(students, numStudents);

	fclose(inFile);
	fclose(outFile);

	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;
	/* 
	 * Parse command-line options (incomplete code!) 
	 * Conceptos SO: Argumentos del main (argc, argv), función getopt.
	 * getopt parsea las opciones precedidas por guion (-).
	 * "hi:po:b" significa:
	 *  - h, p, b: opciones simples (flags).
	 *  - i:, o:: opciones que requieren un argumento (indicado por los dos puntos).
	 */
	while ((opt = getopt(argc, argv, "hi:po:b")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file| -o <output_file> ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			printf("DEBUG: input file: %s\n", optarg);
			// strdup duplica la cadena optarg (que apunta al argumento actual) en nueva memoria.
			options.input_file = strdup(optarg);
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;	
		case 'o':
			options.action = WRITE_BINARY_ACT;
			options.output_file = strdup(optarg);
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (options.input_file == NULL)
	{
		// fprintf a stderr se usa para mensajes de error, separándolos de la salida estándar.
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}





















/*
================================================================================
                            INDICE DE REFERENCIA RAPIDA
================================================================================

1. GESTION DE PROCESOS
----------------------
- fork()          : Crear proceso hijo.
                    -> p4/ejercicio1/run_commands.c (func: launch_command)
                    -> p4/ejercicio3/signalsyfork.c (main)
- execvp()        : Ejecutar programa (reemplaza imagen).
                    -> p4/ejercicio1/run_commands.c (func: launch_command)
- waitpid()       : Esperar cambio de estado de hijo.
                    -> p4/ejercicio1/run_commands.c (func: proces_file)
                    -> p4/ejercicio3/signalsyfork.c (main)
- exit()          : Terminar proceso.
                    -> Usado en casi todos los ejemplos.

2. SEÑALES (SIGNALS)
--------------------
- sigaction()     : Configurar manejador de señales (recomendado).
                    -> p4/ejercicio3/signalsyfork.c (main)
                    -> p4/ejercicio4/inicial.c (main)
- signal()        : Configurar manejador (simple/antiguo).
                    -> p4/ejercicio3/estudiando.c (func: procB)
- kill()          : Enviar señal a proceso.
                    -> p4/ejercicio3/signalsyfork.c (func: handler)
                    -> p4/ejercicio3/estudiando.c (func: procA)
- alarm()         : Temporizador para señal SIGALRM.
                    -> p4/ejercicio3/signalsyfork.c (main)

3. HILOS (THREADS) - POSIX
--------------------------
- pthread_create(): Crear nuevo hilo.
                    -> p4/ejercicio2/hilos.c (main)
                    -> p5/disco.c (main)
- pthread_join()  : Esperar terminación de hilo.
                    -> p4/ejercicio2/hilos.c (main)
                    -> p5/disco.c (main)
- pthread_exit()  : Terminar hilo actual.
                    -> p4/ejercicio2/hilos.c (func: thread_usuario)
- pthread_self()  : Obtener ID del hilo actual.
                    -> p4/ejercicio2/hilos.c (func: thread_usuario)

4. SINCRONIZACION (MUTEX & COND VARS)
-------------------------------------
- pthread_mutex_init/lock/unlock/destroy : Exclusión mutua.
                    -> p5/disco.c (main, enter_normal_client, etc.)
- pthread_cond_init/wait/signal/broadcast: Variables de condición.
                    -> p5/disco.c (enter_normal_client, enter_vip_client)

5. SISTEMA DE FICHEROS (SYSTEM CALLS)
-------------------------------------
- open()          : Abrir archivo (flags: O_RDONLY, O_CREAT, O_TRUNC...).
                    -> p2/ejercicio1/show_file.c
                    -> p3/ejercicio1/copy.c
                    -> p4/ejercicio4/inicial.c
- read()          : Leer bytes de descriptor.
                    -> p2/ejercicio1/show_file.c
                    -> p3/ejercicio1/copy.c
- write()         : Escribir bytes a descriptor.
                    -> p3/ejercicio1/copy.c
                    -> p4/ejercicio4/inicial.c (en handler)
- lseek()         : Mover puntero de archivo.
                    -> p3/ejercicio3/mostrar.c
                    -> p4/ejercicio4/inicial.c
- lstat() / stat(): Obtener metadatos de archivo.
                    -> p3/ejercicio4/espacio.c

6. DIRECTORIOS
--------------
- opendir()       : Abrir flujo de directorio.
                    -> p3/ejercicio4/espacio.c
- readdir()       : Leer entrada de directorio.
                    -> p3/ejercicio4/espacio.c
- closedir()      : Cerrar flujo.
                    -> p3/ejercicio4/espacio.c
- S_ISDIR()       : Macro para comprobar si es directorio.
                    -> p3/ejercicio4/espacio.c

7. PARSEO DE ARGUMENTOS
-----------------------
- getopt()        : Procesar opciones de línea de comandos (-a, -b arg).
                    -> p1/ejercicio3/getopt.c
                    -> p2/ejercicio3/student-records.c
                    -> p4/ejercicio1/run_commands.c

8. OTROS
--------
- malloc/free     : Memoria dinámica.
                    -> p2/ejercicio3/student-records.c
                    -> p4/ejercicio1/run_commands.c
- Estructuras     : Definición y uso.
                    -> p2/ejercicio3/student-records.c (struct student)
                    -> p5/disco.c (struct client)

9. CONCURRENCIA (POSIX)
-----------------------
- shm_open()      : Memoria compartida.
                    -> p6/cocinero.c
                    -> p6/salvajes.c
- mmap()          : Mapeo de memoria.
                    -> p6/cocinero.c
                    -> p6/salvajes.c
- sem_open()      : Semáforos con nombre.
                    -> p6/cocinero.c
                    -> p6/salvajes.c
- sem_wait/post() : Operaciones P/V.
                    -> p6/cocinero.c
                    -> p6/salvajes.c

10. ENLACES
-----------
- ln (comando)    : Creación de enlaces.
                    -> p3/prepara_ficheros.sh
- lstat()         : Info de enlaces simbólicos.
                    -> p3/ejercicio2/copy2.c
                    -> p3/ejercicio4/espacio.c
- readlink()      : Leer destino de enlace.
                    -> p3/ejercicio2/copy2.c
- symlink()       : Crear enlace simbólico.
                    -> p3/ejercicio2/copy2.c
*/
