#include <stdio.h>


#define ARRAY_SIZE  10

int a = 7;
unsigned long b = 8;
short c;
char x;
char* pc;

int array1[ARRAY_SIZE];
int array2[ARRAY_SIZE];

char* str1    = "STRING OF CHARACTERS";
char str2[]   = "STRING OF CHARACTERS";
int main()
{
	pc = &x;
	a = 16;
	/* sizeof devuelve el tamaño en bytes del tipo o variable.
	 * %p se usa para imprimir punteros (direcciones de memoria).
	 * %lu se usa para unsigned long (el tipo devuelto por sizeof).
	 */
	printf("Address of a: %p Size: %lu \n",&a,sizeof(a));
	printf("Address of b: %p Size: %lu \n",&b,sizeof(b));
	printf("Address of c: %p Size: %lu \n",&c,sizeof(c));
	printf("Address of x: %p Size: %lu \n",&x,sizeof(x));
	
	/* pc es un puntero. &pc es la dirección donde se almacena el puntero.
	 * pc (valor) es la dirección a la que apunta.
	 */
	printf("Address of pc: %p Address pointed by pc: %p Size: %lu \n",
			&pc, pc, sizeof(pc));
			
	/* array1 es un array. Su nombre decae a un puntero al primer elemento.
	 * &array1[0] es la dirección del primer elemento (igual a array1).
	 * sizeof(array1) devuelve el tamaño TOTAL del array (elementos * tamaño_elemento).
	 */
	printf("Address of array: %p Addres of elemnt 0: %p Size of the array: %lu \n",
			array1, &array1[0], sizeof(array1));
			
	/* str1 es un puntero a char (char*). Apunta a una cadena constante en memoria de solo lectura.
	 * sizeof(str1) es el tamaño del puntero (4 u 8 bytes).
	 */
	printf("Address of str1: %p Address pointed by it: %p Size: %lu \n",
			&str1, str1, sizeof(str1));
			
	/* str2 es un array de char. Contiene una copia de la cadena.
	 * sizeof(str2) es el tamaño de la cadena incluyendo el terminador nulo.
	 */
	printf("Address of str2: %p Address pointed by it: %p Size: %lu \n",
			&str2, str2, sizeof(str2));

	printf(str1);
	printf("\n");
	printf(str2);
	printf("\n");
    return 0;
}
