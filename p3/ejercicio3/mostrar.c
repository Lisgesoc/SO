#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#define FRONT 0
#define BACK 1

int backwards = 0;

void read_file(const char* filename, int since) {
	char *buffer=malloc(128);
	int bytes_read=0;
	int cont;
	/* Open file */
	int fd = open(filename, O_RDONLY);
	if (fd < 0){
		err(2,"The input file %s could not be opened",filename);
	}

	/* lseek mueve el puntero de lectura/escritura del archivo.
	 * SEEK_SET indica que el desplazamiento 'since' es relativo al inicio del archivo.
	 * Esto permite saltar los primeros 'since' bytes.
	 */
	lseek(fd, since, SEEK_SET);	

	while (1) {
		cont=read(fd,buffer, 127);
		if(cont<=0) break;
		buffer[cont]='\0';
		printf("%s", buffer);
		bytes_read+= cont;
	}
	free(buffer);
	close(fd);

}

int calculaSince(const char* filename, int numBytes) {
	int since=0;

	struct stat *st=malloc(sizeof(struct stat));
	if (st==NULL){
		err(5,"Memory allocation failed");
	}

	/* stat obtiene información del archivo (tamaño, permisos, etc.). */
	if(stat(filename, st) < 0){
		perror("Error getting file size");
		free(st);
		return -1;
	}

	/* Lógica para calcular el offset inicial 'since'.
	 * Si numBytes >= tamaño total, empezamos desde 0 (todo el archivo).
	 * Si backwards está activo (-e), leemos los últimos 'numBytes' (offset = tamaño - numBytes).
	 * Si no, leemos desde el byte 'numBytes' (saltamos los primeros 'numBytes').
	 */
	if(numBytes >= st->st_size){
		since=0;
	} else if(backwards == BACK){
		since=st->st_size - numBytes;
	}else{
		since=numBytes;
	}
	free(st);

	return since;
}
int main(int argc, char *argv[]){

	int since=0, numBytes=0;

	if(argc<2){
		fprintf(stderr, "Usage: %s <file_name> [-n:NumBytes] [-e]\n",argv[0]);
	}
	char *filename=argv[1];
	int opt;
	while ((opt = getopt(argc, argv, "hn:e")) != -1)
	{
		switch (opt){
			case 'n':
				numBytes=atoi(optarg);
				break;
			case 'e':
				backwards=BACK;
				break;
			case 'h':
				printf("Usage: %s <file_name> [-n:NumBytes | -e ]\n",argv[0]);
				return 0;
				break;
			default:
				printf("Unknown option: %c\n", opt);
				return -1;
				break;
		}
	}

	since=calculaSince(filename, numBytes);

	read_file(filename, since);

	return 0;
}
