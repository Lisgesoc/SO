#include <stdio.h>
#include <stdlib.h>

int nelem;
int c;

int main(void)
{
	int *ptr,*a;
	int i;

	c = 37;
	nelem = 127;
	/* Reservamos memoria para un array de 127 enteros. */
	ptr = (int*) malloc(nelem * sizeof(int));
	for (i=0; i < nelem; i++)
		ptr[i] = i;

	/* Acceso normal a elementos del array dinámico. */
	printf("ptr[0]= %d ptr[13]=%d \n", ptr[0], ptr[13]);
	
	/* Guardamos el puntero original en 'a' para poder liberarlo después. */
	a=ptr;
	
	/* Cambiamos 'ptr' para que apunte a la dirección de la variable global 'c'.
	 * Ahora ptr ya no apunta al bloque de malloc.
	 */
	ptr = &c;
	
	/* ptr[0] es equivalente a *ptr, que es el valor de c (37).
	 * ptr[13] accede a la memoria 13 enteros más allá de &c.
	 * Esto es un ACCESO FUERA DE LÍMITES (undefined behavior), ya que c no es un array.
	 */
	printf("ptr[0]= %d ptr[13]=%d \n", ptr[0], ptr[13]);

	/* Liberamos la memoria original usando el puntero guardado 'a'. */
	free(a);

}

