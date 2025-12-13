#include <stdio.h>

char a = 122;
int b = 41;

int main()
{
	printf("a = %d a = %c \n", a, a);
	a += 6;
	printf("a = %d a = %c b=%d  b=%c\n", a, a, b, b);
	
	/* sizeof devuelve el tamaño en bytes de los tipos de datos.
	 * Esto varía según la arquitectura (32 vs 64 bits).
	 */
	printf("Size of int: %lu\n", sizeof(int) );
	printf("Size of char: %lu\n", sizeof(char) );
	printf("Size of float: %lu\n", sizeof(float) );
	printf("Size of double: %lu\n", sizeof(double) );
	printf("Size of long: %lu\n", sizeof(long) );
	printf("Size of short: %lu\n", sizeof(short) );
	
	/* El tamaño de un puntero (void*, int*, char*) es el mismo para todos los tipos
	 * en una arquitectura dada (generalmente 8 bytes en 64-bit).
	 */
	printf("Size of void*: %lu\n", sizeof(void*) );
	printf("Size of void*: %lu\n", sizeof(int*) );
	printf("Size of void*: %lu\n", sizeof(char*) );

	return 0;
}
