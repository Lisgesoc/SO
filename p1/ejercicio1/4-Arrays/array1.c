#include <stdio.h>

#define N 5

void init_array(int array[], int size) ;
void init_array2(int array[N]);

int main(void)
{
	int i,list[N];
	/* list es un array. Su nombre actúa como puntero al primer elemento.
	 * &list[0] es la dirección del primer elemento.
	 * sizeof(list) devuelve el tamaño total del array (N * sizeof(int)).
	 */
	printf("Address of list %p Address of list[0]: %p Address of list[1]: %p. Sizeof list %lu \n",
			list, &list[0], &list[1], sizeof(list));

	init_array(list, N);
	for (i = 0; i < N; i++)
		printf("next: %d ", list[i]);
	printf("\n-------------------------\n");

	init_array2(list);
	for (i = 0; i < N; i++)
		printf("next: %d ", list[i]);
	printf("\n-------------------------\n");
}

/* Inicializa el array con valores secuenciales.
 * Recibe el array y su tamaño.
 */
void init_array(int array[], int size)
{
	int i;
	/* Cuando se pasa un array a una función, decae a un puntero.
	 * Por tanto, sizeof(array) aquí devuelve el tamaño del puntero (8 bytes en 64-bit),
	 * NO el tamaño del array original.
	 */
	printf("Address of the array: %p Sizeof array %lu \n", array, sizeof(array));
	for (i = 0; i < size; i++)
		array[i] = i;
	printf("Array initialized\n\n");
}

/* Inicializa el array con valores pares secuenciales.
 * Aunque se declare array[N], se recibe como puntero.
 */
void init_array2(int array[N])
{
	int i;
	/* Incluso si especificamos el tamaño en el argumento (int array[N]),
	 * el compilador lo trata como un puntero (int *array).
	 * sizeof(array) sigue siendo el tamaño del puntero.
	 */
	printf("Address of the array: %p Sizeof array %lu \n", array, sizeof(array));
	for (i = 0; i < N; i++)
		array[i] = i*2;
	printf("Array initialized\n\n");
}


