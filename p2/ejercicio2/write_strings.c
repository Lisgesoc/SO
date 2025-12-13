#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[]){

	/* Validación de argumentos.
	 * Se espera al menos el nombre del archivo de salida y una cadena.
	 */
	if(argc < 3){
		errx(EXIT_FAILURE, "Usage: %s <outFileName> <string1> <string2> ... <stringN>", argv[0]);
		return 0;
	}

	/* Apertura del archivo en modo escritura ("w").
	 * Si el archivo no existe, se crea. Si existe, se trunca (se borra su contenido).
	 */
	FILE *file=fopen(argv[1], "w");
	if(file == NULL){
		errx(EXIT_FAILURE, "Error opening file");
		return 0;
	}

	/* Iteramos sobre los argumentos restantes (las cadenas a escribir). */
	for(int i = 2; i < argc; i++){
		/* Escribimos la cadena en el archivo.
		 * fwrite escribe 'strlen(argv[i])' elementos de tamaño 'sizeof(char)'.
		 * argv[i] es el puntero a la cadena actual.
		 */
		fwrite(argv[i], sizeof(char), strlen(argv[i]), file);
		
		/* Escribimos el carácter nulo terminador '\0'.
		 * Esto es importante para separar las cadenas en el archivo binario
		 * y que luego puedan ser leídas correctamente (por ejemplo, por read_strings.c).
		 */
		fwrite("\0", sizeof(char), 1, file);
	}

	/* Cerramos el archivo para asegurar que los datos se vuelcan al disco. */
	fclose(file);

	return 0;
}
