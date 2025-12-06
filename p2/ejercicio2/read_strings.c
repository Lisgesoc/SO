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

	do{
		if((fread(&c, sizeof(char), 1, file))==0){
			return NULL;
		}		
		len++;
	} while (c!='\0');
	
	fseek(file,-len,SEEK_CUR);
	char *str=malloc(len);
	if(str!=NULL){
		fread(str, sizeof(char), len, file);
		return str;
	}

	return NULL;	
}

int main(int argc, char *argv[])
{
	if(argc < 2){
		errx(EXIT_FAILURE, "Usage: %s <inFileName>", argv[0]);
		return 0;
	}

	FILE *file=fopen(argv[1], "r");
	if(file == NULL){
		errx(EXIT_FAILURE, "Error opening file");
		return 0;
	}

	char *str;
	str=loadstr(file);

	while(str!=NULL){
		printf("%s\n", str);
		free(str);
		str=loadstr(file);
	}

	fclose(file);

	return 0;
}
