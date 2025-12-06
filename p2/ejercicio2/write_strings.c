#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[]){

	if(argc < 3){
		errx(EXIT_FAILURE, "Usage: %s <outFileName> <string1> <string2> ... <stringN>", argv[0]);
		return 0;
	}

	FILE *file=fopen(argv[1], "w");
	if(file == NULL){
		errx(EXIT_FAILURE, "Error opening file");
		return 0;
	}

	for(int i = 2; i < argc; i++){
		fwrite(argv[i], sizeof(char), strlen(argv[i]), file);
		fwrite("\0", sizeof(char), 1, file);
	}

	fclose(file);

	return 0;
}
