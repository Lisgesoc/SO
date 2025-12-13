#include <stdio.h>
#include <string.h>

/* Copia la dirección de memoria.
 * Recibe un puntero a puntero (char** dst).
 * Modifica el puntero original para que apunte a 'org'.
 */
void copy2(char* org, char** dst)
{
	*dst = org;
}

/* Intenta copiar la dirección, pero solo modifica la copia local de 'dst'.
 * No tiene efecto fuera de la función.
 */
void copy(char* org, char* dst)
{
	dst = org;
}

/* Modifica el contenido de la cadena.
 * Resta 32 a cada carácter (convierte minúsculas a mayúsculas en ASCII).
 */
void mod(char* org, char* dst)
{
	int i;

	for (i=0;i<strlen(org);i++)
		dst[i] = org[i] - 32;
}

int main()
{
	/* Cadenas literales. Se almacenan en memoria de solo lectura.
	 * Intentar modificarlas directamente causaría Segmentation Fault.
	 */
	char* str1 = "original";
	char* str2 = "other";
	char str3[32]; // Buffer en stack, modificable.

	//copy(str1, str2); // No haría nada
	
	/* Pasamos la dirección de str2 (&str2) para poder cambiar a dónde apunta. */
	copy2(str1, &str2);
	printf("str1 %s str2 %s\n", str1, str2);

	/* Modificamos el contenido y lo guardamos en str3. */
	mod(str1, str3);
	printf("str1 %s str3 %s\n", str1, str3);

	return 0;
}

