#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * INFO SOBRE ENLACES (LINKS)
 * --------------------------
 * Comandos utilizados para crear enlaces (ver p3/prepara_ficheros.sh):
 *   ln -s "fichero1" "enlaceS"  -> Crea un enlace simbólico (soft link) llamado "enlaceS" que apunta a "fichero1".
 *   ln "fichero2" "enlaceH"     -> Crea un enlace físico (hard link) llamado "enlaceH" que apunta al mismo inodo que "fichero2".
 *
 * Diferencias clave:
 *   - Hard Link: Apunta al mismo inodo. Si borras el original, el enlace sigue funcionando (el contenido persiste hasta que el contador de enlaces del inodo llega a 0).
 *                No se pueden hacer hard links a directorios (normalmente) ni entre distintos sistemas de ficheros.
 *   - Soft Link: Es un fichero especial que contiene la ruta al fichero destino. Si borras el original, el enlace queda roto.
 *                Puede apuntar a directorios y cruzar sistemas de ficheros.
 *
 * System calls relacionadas:
 *   - link(oldpath, newpath): Crea un hard link.
 *   - symlink(target, linkpath): Crea un soft link.
 *   - unlink(pathname): Borra un nombre del sistema de ficheros (decrementa contador de enlaces).
 *   - lstat(pathname, statbuf): Como stat, pero si es un enlace simbólico, devuelve info del enlace mismo, no del destino.
 *   - readlink(pathname, buf, bufsiz): Lee el contenido de un enlace simbólico (la ruta a la que apunta).
 */

//CON FOPEN Y FREAD Y FWRITE
/*
int main(int argc, char* argv[]) {
	FILE* file=NULL;
	//int c,ret;
	char *buffer;

	buffer= malloc(1);
	if (buffer==NULL){
		err(4,"Memory allocation failed");
	}

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	// Open file 
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	// Read file byte by byte 
	while (fread(buffer, 1,1, file) != 0) {
		// Print byte to stdout 
		//ret=putc((unsigned char) c, stdout);
		fwrite(buffer,1,1,stdout);
		


	}
	free(buffer);
	fclose(file);
	return 0;
}

*/
//CON OPEN Y READ
int main(int argc, char* argv[]) {
	char *buffer;
	int fd, bytes_read=0;
	struct stat *st;

	/* Reservamos memoria para la estructura stat. */
	st= malloc(sizeof(struct stat));
	if (st==NULL){
		err(5,"Memory allocation failed");
	}
	/* Reservamos memoria para el buffer de lectura (1 byte). */
	buffer= malloc(1);

	if (buffer==NULL){
		err(4,"Memory allocation failed");
	}

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	/* Abrimos el archivo en modo solo lectura. */
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		err(2,"The input file %s could not be opened",argv[1]);
	
	/* Obtenemos información del archivo (aunque no se usa explícitamente después). */
	fstat(fd, st);
	
	/* Read file byte by byte */
	while (1) {
		/* Leemos 1 byte del archivo.
		 * read devuelve el número de bytes leídos.
		 */
		if(read(fd,buffer, 1)<=0) break;

		/* Imprimimos el carácter leído. */
		printf("%c", buffer[0]);
		bytes_read+= sizeof(buffer);
	}
	/* Liberamos recursos. */
	free(buffer);
	close(fd);
	return 0;
}
