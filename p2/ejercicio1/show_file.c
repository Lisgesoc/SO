#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

	st= malloc(sizeof(struct stat));
	if (st==NULL){
		err(5,"Memory allocation failed");
	}
	buffer= malloc(1);

	if (buffer==NULL){
		err(4,"Memory allocation failed");
	}

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		err(2,"The input file %s could not be opened",argv[1]);
	
	fstat(fd, st);
	/* Read file byte by byte */
	while (1) {
		if(read(fd,buffer, 1)<=0) break;

		printf("%c", buffer[0]);
		bytes_read+= sizeof(buffer);
	}
	free(buffer);
	close(fd);
	return 0;
}
