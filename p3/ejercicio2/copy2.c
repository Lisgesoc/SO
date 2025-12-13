#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>  
#include <fcntl.h>   

void copy(int fdo, int fdd)
{
	char buffer[512];
	int bytes_read;
	while(bytes_read<sizeof(buffer)){	
		bytes_read=read(fdo, buffer, sizeof(buffer));
		if(bytes_read < 0){
			perror("Error reading source file");
			close(fdo);
			close(fdd);
			exit(EXIT_FAILURE);
		}else if(bytes_read==0){
			break;
		}
		write(fdd, buffer, bytes_read);
	}
}

void copy_regular(char *orig, char *dest)
{
	/* Copia de archivos regulares.
	 * Abrimos origen para lectura y destino para escritura (creando/truncando).
	 */
	int fdi = open(orig, O_RDONLY);
	if(fdi < 0){
		perror("Error opening source file");
		exit(EXIT_FAILURE);
	}
	/* Permisos 0644: S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH */
	int fdo = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if(fdo < 0){
		perror("Error opening/creating destination file");
		close(fdi);
		exit(EXIT_FAILURE);
	}

	copy(fdi, fdo); // Función auxiliar de copia (read/write loop)

	close(fdi);
	close(fdo);
}

void copy_link(char *orig, char *dest)
{
	struct stat st;
	/* Obtenemos información del enlace simbólico origen para saber su tamaño. */
	lstat(orig, &st);	
	
	/* Reservamos memoria para almacenar la ruta a la que apunta el enlace.
	 * +1 para el carácter nulo terminador.
	 */
	char *buffer = malloc(st.st_size + 1);
	if(buffer == NULL){
		perror("Memory allocation error");
		exit(EXIT_FAILURE);	
	}
	
	/* readlink lee el contenido del enlace simbólico (la ruta apuntada).
	 * No añade el carácter nulo al final, por eso lo hacemos manualmente.
	 */
	int len = readlink(orig, buffer, st.st_size + 1);
	if(len < 0){
		perror("Error reading symbolic link");
		free(buffer);
		exit(EXIT_FAILURE);
	}
	buffer[len] = '\0';
	
	/* symlink crea un nuevo enlace simbólico en 'dest' que apunta a 'buffer'. */
	if(symlink(buffer, dest) < 0){
		perror("Error creating symbolic link");
		free(buffer);
		exit(EXIT_FAILURE);
	}	
	free(buffer);
}

int main(int argc, char *argv[]){

	if(argc != 3){
		fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *source = argv[1];
	char *destination = argv[2];	
	struct stat st;
	
	/* lstat es crucial aquí para distinguir si el origen es un enlace simbólico.
	 * stat seguiría el enlace y nos daría info del archivo apuntado.
	 */
	if(lstat(source, &st) < 0){
		perror("lstat error");
		exit(EXIT_FAILURE);
	}
	
	/* Despachamos según el tipo de archivo. */
	if(S_ISLNK(st.st_mode)){
		copy_link(source, destination);
	}else if(S_ISREG(st.st_mode)){
		copy_regular(source, destination);
	}else{
		fprintf(stderr, "Unsupported file type\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
