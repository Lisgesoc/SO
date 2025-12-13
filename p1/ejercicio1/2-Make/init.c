#include "auxiliar.h"
#include <stdio.h>

/* Declaración externa de la variable global 'pi' definida en auxiliar.c.
 * 'extern' indica al compilador que la variable está definida en otro lugar.
 */
extern double pi;

int main()
{
	int degrees;
	double rad;

	printf("Introduce an angle expressed in degrees: ");
	scanf("%d",&degrees);
	printf("\n");

	/* Conversión manual de grados a radianes usando la variable global externa. */
	rad = (degrees*pi)/180;
	printf("%d degress are equivalent to %f radians\n", degrees, rad);

	/* Llamada a funciones definidas en auxiliar.c */
	printf("sin(%d) = %f. cos(%d) = %f\n",degrees, sinDegrees(degrees), degrees, cosDegrees(degrees));	
	return 0;
}
