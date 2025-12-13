#include <stdio.h>
#include <stdlib.h>

int c = 7;
int main(void)
{
	int *ptr;
	/* %p imprime la dirección de memoria en hexadecimal.
	 * &ptr es la dirección de la variable puntero.
	 * ptr es el contenido de la variable puntero (una dirección).
	 * &c es la dirección de la variable entera c.
	 * c es el valor entero.
	 */
	printf("Address of ptr %p. ptr points to %p. Address of c: %p Value of c %d\n",
			&ptr, ptr, &c, c);

	/* Asignamos la dirección de c al puntero ptr. */
	ptr = &c;
	printf("Address of ptr %p,. ptr points to %p. Address of c: %p Value of c %d\n",
			&ptr, ptr, &c, c);

	/* Desreferenciamos ptr para modificar el valor almacenado en la dirección apuntada (c). */
	*ptr = 4;
	printf("ptr points to %p. Content in the address pointed by ptr: %d Address of c: %p Value of c %d\n",
			ptr, *ptr, &c, c);

	/* Asignación directa de una dirección de memoria (peligroso y no portable).
	 * 0x600a48 es una dirección arbitraria que probablemente no sea válida.
	 */
	ptr =  (int*) 0x600a48;
	printf("Address of ptr %p. Value of c %d\n", ptr, c);
	
	/* malloc reserva memoria en el heap.
	 * Devuelve un puntero al bloque reservado.
	 */
	ptr=malloc(sizeof(int));

	*ptr = 13;	
	printf("Address of ptr %p. ptr points to %p.Value of ptr points direction %d. Address of c: %p Value of c %d\n",
			&ptr, ptr,*ptr, &c, c);

	/* Liberamos la memoria reservada. */
	free(ptr);

    return 0;
}

