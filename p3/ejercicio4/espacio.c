#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define tamBlock 512

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks){
	struct stat st;
	
	/* lstat obtiene información del archivo, similar a stat pero si es un enlace simbólico,
	 * obtiene información del enlace mismo, no del archivo apuntado.
	 */
	if(lstat(fname, &st)<0){
		return -1; // Error al obtener información
	}
	
	/* S_ISDIR comprueba si el modo del archivo indica que es un directorio. */
	if(S_ISDIR(st.st_mode)){
		return get_size_dir(fname, blocks); // Llamada recursiva para directorios
	}
	
	/* st_blocks contiene el número de bloques de 512 bytes asignados al archivo. */
	*blocks += st.st_blocks;
	return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks){

	DIR *dir;
	/* opendir abre un flujo de directorio correspondiente al nombre dado. */
	dir = opendir(dname);
	if(dir == NULL){
		return -1;
	}
	struct dirent *entry;
	
	/* readdir lee la siguiente entrada del directorio.
	 * Devuelve NULL cuando se llega al final o hay error.
	 */
	while((entry = readdir(dir)) != NULL){
		/* Ignoramos las entradas especiales "." (directorio actual) y ".." (padre)
		 * para evitar bucles infinitos en la recursión.
		 */
		if((strcmp(entry->d_name, ".") == 0) ||
		   (strcmp(entry->d_name, "..") == 0)){
			continue;
		}
		char path[PATH_MAX];
		/* Construimos la ruta completa del archivo/directorio encontrado. */
		snprintf(path, sizeof(path), "%s/%s", dname, entry->d_name);

		/* d_type indica el tipo de archivo. DT_DIR es para directorios.
		 * Nota: d_type no es estándar POSIX completo pero es común en Linux/BSD.
		 */
		if(entry->d_type == DT_DIR){
			if(get_size_dir(path, blocks)<0){
				closedir(dir);
				return -1;
			}
		}else{
			struct stat st;
			/* Obtenemos el tamaño si no es directorio (o si d_type no estaba disponible y falló la lógica anterior, aunque aquí se asume d_type funciona). */
			if(lstat(path, &st)<0){
				closedir(dir);
				return -1;
			}
			*blocks += st.st_blocks;
		}
	}
	return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[]){

	if(argc < 2){
		fprintf (stderr, "Usage: %s file1 [file2 ... fileN]\n", argv[0]);
		return -1;
	}

	for(int i=1;i<argc;i++){
		size_t blocks = 0;
		if(get_size(argv[i], &blocks) < 0){
			fprintf(stderr, "Error getting size of %s\n", argv[i]);
			continue;
		}
		printf("Size of %s: %zu KB\n", argv[i], (blocks * tamBlock)/1024 );	
	}


	return 0;
}
