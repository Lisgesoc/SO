#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include "defs.h"
#include <bits/getopt_core.h>


char * progname;

/**
 * @brief  Print usage of the tool
 **/
void usage() {
	printf("Usage: %s [ options ] title \n",progname);
	printf("\n\toptions:\n");
	printf("\t\t-h: display this help message\n");
	printf("\t\t-e: print even numbers instead of odd  (default)\n");
	printf("\t\t-l lenght: lenght of the sequence to be printed\n");
	printf("\t\ttitle: name of the sequence to be printed\n");
}

/**
 * @brief  Prints a sequence of odd or even numbers in stdout
 *
 * @param lenght
 * @param type
 * @param title
 **/
void display_numbers ( int lenght, parity_t type, char * title) {
	
	int i,n;
	int first = (type == ODD )? 1 : 2;
	
	printf("Title: %s\n",title);
	for (i=0,n=first; i<lenght; i++,n+=2) {
		printf("%d ",n);
	}
	printf("\n");
}



int main(int argc, char *argv[])
{
	int  opt;
	struct options options;

	progname = argv[0];

	/* Inicialización de valores por defecto para las opciones. */
	options.par_mode = ODD;
	options.lenght= 10;
	options.title = NULL;

	/* Parseo de opciones de línea de comandos con getopt.
	 * "hel:" indica que buscamos las opciones -h, -e, y -l.
	 * Los dos puntos ':' después de 'l' indican que -l requiere un argumento.
	 */
	while((opt = getopt(argc, argv, "hel: ")) != -1) {
		switch(opt) {
		case 'h':
			usage();
			exit(0);
		case 'e':
			options.par_mode = EVEN;
			break;
		case 'l':
			/* optarg contiene el argumento de la opción actual (si lo requiere).
			 * optind es el índice del siguiente elemento a procesar en argv.
			 */
			if (optind >= argc) {
				// Nota: getopt maneja internamente la falta de argumentos si se configura,
				// pero aquí se hace una comprobación manual adicional o redundante.
				fprintf(stderr, "Option -l requires an argument\n");
				usage();
				exit(EXIT_FAILURE);
			}	
			options.lenght = atoi(optarg); // Convertimos el argumento a entero
			break;
		default:
			break;
		}
	}

	/* Se espera un argumento extra (el título) que no es una opción (no empieza por -).
	 * optind apunta al primer argumento que no es una opción tras el bucle getopt.
	 */
	if (optind >= argc) {
		fprintf(stderr, "Invalid title\n");
		usage();
		exit(EXIT_FAILURE);
	}
	
	/* Asignamos el título.
	 * En este caso, el código original tomaba argv[argc-1], asumiendo que es el último.
	 * Lo correcto tras getopt suele ser usar argv[optind].
	 */
	options.title = argv[argc-1];
	/* Fill options.title with the corresponding element of argv */
	// options.title = argv[xxxx];

    /* Llamada a la función principal con las opciones parseadas. */
	display_numbers (options.lenght, options.par_mode, options.title);
	return 0;
}
