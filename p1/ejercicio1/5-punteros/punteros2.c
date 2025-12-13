#include <stdio.h>
#include <stdlib.h>

int nelem;

int main(void)
{
	int *ptr;
	int * ptr2;

	nelem = 127;
	/* Reservamos memoria dinámica para 127 enteros. */
	ptr = (int*) malloc(nelem*sizeof(int));
	
	/* Asignamos valor a la primera posición (ptr[0]). */
	*ptr = 5;
	ptr[0] = 7; // Sobrescribe el 5 anterior
	
	/* ptr2 apunta a la misma dirección que ptr. */
	ptr2 = ptr;

	printf("Address pointed by ptr %p. Memory content at that address: %d \n",
			ptr, *ptr);

	ptr[1] = 10;
	printf("Address of the element ptr[1] %p. Memory content at that address: %d \n",
			&ptr[1], ptr[1]);

	/* Aritmética de punteros: ptr2++ avanza el puntero sizeof(int) bytes.
	 * Ahora ptr2 apunta a ptr[1].
	 */
	ptr2++;
	*ptr2 = 15; // Modifica ptr[1]
	printf("Address of the element ptr[1] %p. Memory content at that address: %d \n",
			&ptr[1], ptr[1]);

	/* Liberamos la memoria reservada.
	 * IMPORTANTE: ptr sigue apuntando a esa dirección, pero ahora es inválida (dangling pointer).
	 */
	free(ptr);
	
	/* ERROR GRAVE: Uso de memoria después de liberarla (Use-After-Free).
	 * Esto puede causar corrupción de memoria o segmentation fault.
	 */
	*ptr = 3;
	printf("Address pointed by ptr %p. Memory content at that address: %d \n",
			ptr, *ptr);
	return 0;
}

