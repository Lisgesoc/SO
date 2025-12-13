#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
	char c;
	int len=0;

	/* Bucle para calcular la longitud de la cadena leyendo byte a byte.
	 * En Sistemas Operativos es importante entender cómo se accede al archivo.
	 * fread realiza una lectura bufferizada desde el flujo 'file'.
	 */
	do{
		// Leemos 1 byte (sizeof(char)) del archivo y lo guardamos en la variable 'c'.
		// fread devuelve el número de elementos leídos correctamente. Si es 0, hemos llegado al final del archivo o hubo error.
		if((fread(&c, sizeof(char), 1, file))==0){
			return NULL; // Fin de archivo o error
		}		
		len++; // Incrementamos la longitud
	} while (c!='\0'); // Continuamos hasta encontrar el terminador nulo de la cadena
	
	/* fseek nos permite mover el puntero de posición del archivo.
	 * Aquí retrocedemos 'len' bytes desde la posición actual (SEEK_CUR)
	 * para volver al inicio de la cadena que acabamos de medir.
	 */
	fseek(file,-len,SEEK_CUR);

	/* Asignación dinámica de memoria.
	 * Solicitamos al sistema operativo 'len' bytes en el heap para almacenar la cadena.
	 */
	char *str=malloc(len);
	if(str!=NULL){
		// Leemos la cadena completa ahora que tenemos memoria reservada.
		// Leemos 'len' elementos de tamaño 1 byte.
		fread(str, sizeof(char), len, file);
		return str; // Devolvemos el puntero a la memoria reservada
	}

	return NULL; // Fallo en malloc
}

int main(int argc, char *argv[])
{
	// Verificación de argumentos de la línea de comandos.
	// argc: número de argumentos. argv: array de cadenas con los argumentos.
	if(argc < 2){
		// errx imprime un mensaje de error y termina el proceso con el código EXIT_FAILURE.
		errx(EXIT_FAILURE, "Usage: %s <inFileName>", argv[0]);
		return 0;
	}

	/* Apertura del archivo.
	 * fopen solicita al SO abrir el archivo en modo lectura ("r").
	 * Devuelve un puntero a FILE que representa el flujo.
	 */
	FILE *file=fopen(argv[1], "r");
	if(file == NULL){
		// Si fopen falla (ej. archivo no existe, permisos), devuelve NULL.
		errx(EXIT_FAILURE, "Error opening file");
		return 0;
	}

	char *str;
	// Primera llamada para leer la primera cadena del archivo
	str=loadstr(file);

	/* Bucle principal: procesamos mientras loadstr devuelva una cadena válida. */
	while(str!=NULL){
		// Imprimimos la cadena leída en la salida estándar (stdout)
		printf("%s\n", str);
		
		/* Gestión de memoria: IMPORTANTE.
		 * Como loadstr reservó memoria con malloc, es responsabilidad del llamador liberarla.
		 * free libera el bloque de memoria apuntado por 'str' para que el SO pueda reutilizarlo.
		 */
		free(str);
		
		// Leemos la siguiente cadena
		str=loadstr(file);
	}

	/* Cierre del archivo.
	 * Es buena práctica cerrar los descriptores de archivo para liberar recursos del sistema.
	 */
	fclose(file);

	return 0;
}
