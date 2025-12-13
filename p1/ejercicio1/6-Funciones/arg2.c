#include <stdio.h>
#include <stdlib.h> // Necesario para malloc y free

/* Structured data type */
struct _complex_ {
	float re;
	float im;
};

/* Forward declaration */
int sum(int *pa, int *pb);
void sumC( struct _complex_* a,  struct _complex_* b,  struct _complex_* c);

int main(void)
{
	int x = 4, y = 5;
	int* ptr = &y;
	struct _complex_ xc = {.re = 1.0, .im = 2.0};
	struct _complex_ yc = {.re = 3.0, .im = 1.0};
	/* Asignación dinámica de memoria para una estructura. */
	struct _complex_ *zc= (struct _complex_*) malloc(sizeof(struct _complex_));


	printf("Complex numbers addition. (%f,%f i) + (%f,%f i) = ",
			xc.re, xc.im, yc.re, yc.im);

	/* Paso por referencia (punteros).
	 * Pasamos las direcciones de memoria de las estructuras.
	 * La función puede modificar el contenido original.
	 */
	sumC(&xc, &yc, zc);
	printf("(%f,%f i)\n", zc->re, zc->im);

	/* Paso por referencia de enteros. */
	int total = sum(&x,ptr);
	printf("Integer addition:  x +y = %d + %d = %d \n", x, y, total);
	printf("xc = (%f,%f i)  yc = (%f,%f i) zc = (%f,%f i)\n",
			xc.re, xc.im, yc.re, yc.im, zc->re, zc->im);

	free(zc);
	return 0;
}

/* Suma dos enteros pasados por referencia (punteros).
 * Modifica los valores originales a través de los punteros.
 */
int sum(int *pa, int *pb)
{
	/* args passed by reference */
	/* Accedemos al valor apuntado usando el operador de desreferencia (*). */
	int c = *pa + *pb;

	/* Modificamos las variables originales en el main a través de los punteros. */
	*pa = 7;
	*pb = 8;
	return c;  /* return by value */
}
/* Suma dos números complejos pasados por referencia.
 * El resultado se almacena en la estructura apuntada por c.
 */
void sumC( struct _complex_* a,  struct _complex_* b,  struct _complex_* c)
{
	/* Acceso a miembros de estructura a través de puntero usando -> */
	c->re = a->re + b->re;
	c->im = a->im + b->im;

	// We modify the first argument
	/* Modifica la estructura original 'xc' del main. */
	a->re = 12.5;
	a->im = 13.4;
}
