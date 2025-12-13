#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   
#include <unistd.h>  
#include <sys/stat.h>

/*
void copy(int fdo, int fdd){
	char buffer[512];
	int bytes_read;
	int total_bytes_read=0;
	struct stat st;
	fstat(fdo, &st);
	int total_bytes=st.st_size;
	while(total_bytes_read < total_bytes){	
		if(total_bytes - bytes_read < sizeof(buffer)){
			bytes_read=read(fdo, buffer, sizeof(buffer));
			if(bytes_read < 0){
				perror("Error reading source file");
				close(fdo);
				close(fdd);
				exit(EXIT_FAILURE);
			}
			write(fdd, buffer, bytes_read);
			total_bytes_read += bytes_read;
		}else{
			bytes_read=read(fdo, buffer, total_bytes - total_bytes_read);
			if(bytes_read < 0){
				perror("Error reading source file");
				close(fdo);
				close(fdd);
				exit(EXIT_FAILURE);
			}
			write(fdd, buffer, bytes_read);
			total_bytes_read += bytes_read;
			break;
		}
	}
}
*/


void copy(int fdo, int fdd){
	char buffer[512]; // Buffer de tamaño fijo para la copia
	int bytes_read;
	
	/* Bucle de copia: leemos del origen y escribimos en el destino.
	 * sizeof(buffer) indica cuántos bytes intentamos leer en cada iteración.
	 */
	while(bytes_read<sizeof(buffer)){	
		/* read lee del descriptor de archivo 'fdo'.
		 * Devuelve el número de bytes leídos, 0 si es fin de archivo (EOF), o -1 si hay error.
		 */
		bytes_read=read(fdo, buffer, sizeof(buffer));
		if(bytes_read < 0){
			perror("Error reading source file"); // perror imprime el error asociado a errno
			close(fdo);
			close(fdd);
			exit(EXIT_FAILURE);
		}else if(bytes_read==0){
			break; // Fin de archivo alcanzado
		}
		/* write escribe en el descriptor de archivo 'fdd'.
		 * Escribimos exactamente el número de bytes que leímos ('bytes_read').
		 */
		write(fdd, buffer, bytes_read);
	}
}


int main(int argc, char *argv[]){

	if(argc != 3){
		fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
		exit(EXIT_FAILURE);
	}	

	/* Abrimos el archivo origen en modo solo lectura (O_RDONLY). */
	int fdi = open(argv[1], O_RDONLY);
	if(fdi < 0){
		perror("Error opening source file");
		exit(EXIT_FAILURE);
	}
	
	/* Abrimos el archivo destino. Flags:
	 * O_WRONLY: Solo escritura.
	 * O_CREAT: Crear el archivo si no existe.
	 * O_TRUNC: Truncar el archivo a longitud 0 si ya existe.
	 * 0644: Permisos del archivo (rw-r--r--) si se crea.
	 */
	int fdo = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fdo < 0){
		perror("Error opening/creating destination file");
		close(fdi);
		exit(EXIT_FAILURE);
	}

	copy(fdi, fdo);

	printf("File copied successfully from %s to %s\n", argv[1], argv[2]);
	
	/* Cerramos descriptores de archivo */
	close(fdi);
	close(fdo);
	return 0;
}
