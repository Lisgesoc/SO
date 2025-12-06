#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"
#include <getopt.h>


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100


void registraLinea(FILE *file, student_t *student){
	char *token[4];
	char *line=malloc(MAXLEN_LINE_FILE*sizeof(char));
	char *cpy=line;
	fgets(line, MAXLEN_LINE_FILE, file);
	for(int j=0;j<4;j++){
		token[j]=strsep(&line,":");
	}
	student->first_name=malloc(strlen(token[2])+1);
	student->last_name=malloc(strlen(token[3])+1);

	student->student_id=atoi(token[0]);
	strcpy(student->NIF, token[1]);
	strcpy(student->first_name,token[2]);
	strcpy(student->last_name,token[3]);

	free(cpy);
}

void free_student(student_t* student){
	free(student->first_name);
	free(student->last_name);
}
void free_student_array(student_t* student_array, int nr_entries){
	for(int i=0; i<nr_entries; i++){
		free_student(&student_array[i]);
	}
	free(student_array);
}

student_t* read_student_text_file(FILE* students, int* nr_entries){
	fscanf(students , "%d\n", nr_entries);
	student_t* student_array=malloc((*nr_entries)*sizeof(student_t));
	for(int i=0; i<*nr_entries; i++){
		registraLinea(students, &student_array[i]);
	}
	return student_array;
}

student_t* read_student_binary_file(FILE* students, int* nr_entries){
	fread(nr_entries, sizeof(int), 1, students);
	printf("Number of entries: %d\n", *nr_entries);
	student_t* student_array=malloc((*nr_entries)*sizeof(student_t));
	for(int i=0; i<*nr_entries; i++){
		fread(&student_array[i].student_id, sizeof(int), 1, students);
		fread(&student_array[i].NIF, sizeof(student_array[i].NIF), 1, students);

		//first_name
		char buffer_first_name[MAXLEN_LINE_FILE];
		int index_fn=0;
		char ch_fn;
		do{
			fread(&ch_fn, sizeof(char), 1, students);
			buffer_first_name[index_fn]=ch_fn;
			index_fn++;
		}while(ch_fn!='\0' && index_fn<MAXLEN_LINE_FILE);
		student_array[i].first_name=malloc(strlen(buffer_first_name)+1);
		strcpy(student_array[i].first_name, buffer_first_name);

		//last_name
		char buffer_last_name[MAXLEN_LINE_FILE];
		int index_ln=0;
		char ch_ln;
		do{
			fread(&ch_ln, sizeof(char), 1, students);
			buffer_last_name[index_ln]=ch_ln;
			index_ln++;
		}while(ch_ln!='\0' && index_ln<MAXLEN_LINE_FILE);
		student_array[i].last_name=malloc(strlen(buffer_last_name)+1);
		strcpy(student_array[i].last_name, buffer_last_name);
	}
	return student_array;
}


int print_text_file(char *path){

	FILE *file=fopen(path, "r");
	if(file==NULL){
		perror("Error opening file");
		return EXIT_FAILURE;
	}

	int numStudents;
	student_t *students;
	students=read_student_text_file(file, &numStudents);

	printf("Number of students: %d\n", numStudents);



	for(int i=0; i<numStudents; i++){
		
		printf("\n--- Student %d ---\n", i+1);
		printf("Student ID: %d\n", students[i].student_id);
		printf("NIF: %s\n", students[i].NIF);
		printf("First Name: %s\n", students[i].first_name);
		printf("Last Name: %s\n", students[i].last_name);

	}

	free_student_array(students, numStudents);

	fclose(file);
	return 0;
}

int print_binary_file(char *path)
{
	/* To be completed  (part C) */

	FILE*file=fopen(path, "rb");
	if(file==NULL){
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	int numStudents;
	student_t *students;
	students=read_student_binary_file(file, &numStudents);
	for(int i=0; i<numStudents; i++){
		
		printf("\n--- Student %d ---\n", i+1);
		printf("Student ID: %d\n", students[i].student_id);
		printf("NIF: %s\n", students[i].NIF);
		printf("First Name: %s\n", students[i].first_name);
		printf("Last Name: %s\n", students[i].last_name);

	}

	free_student_array(students, numStudents);

	fclose(file);

	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part B) */
	FILE *inFile=fopen(input_file, "r");
	if(inFile==NULL){
		perror("Error opening input file");
		return EXIT_FAILURE;
	}
	FILE *outFile=fopen(output_file, "wb");
	if(outFile==NULL){
		perror("Error opening output file");
		return EXIT_FAILURE;
	}

	int numStudents;
	student_t *students;
	students=read_student_text_file(inFile, &numStudents);
	fwrite(&numStudents, sizeof(int), 1, outFile);

	student_t student;


	for(int i=0; i<numStudents; i++){

		fwrite(&students[i].student_id, sizeof(int), 1, outFile);
		fwrite(&students[i].NIF, sizeof(students[i].NIF), 1, outFile);
		fwrite(students[i].first_name, strlen(students[i].first_name)+1, 1, outFile);
		fwrite(students[i].last_name, strlen(students[i].last_name)+1, 1, outFile);

	}

	free_student_array(students, numStudents);

	fclose(inFile);
	fclose(outFile);

	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:po:b")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file| -o <output_file> ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			printf("DEBUG: input file: %s\n", optarg);
			options.input_file = strdup(optarg);
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;	
		case 'o':
			options.action = WRITE_BINARY_ACT;
			options.output_file = strdup(optarg);
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
