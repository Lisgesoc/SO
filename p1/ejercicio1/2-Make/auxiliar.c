#include <math.h>

/* M_PI es una constante definida en math.h que representa el valor de PI. */
double pi = M_PI;

/* Convierte grados a radianes y calcula el seno. */
double sinDegrees(double x) {
	double rad = (x*M_PI/180);
	return sin(rad);
}

/* Convierte grados a radianes y calcula el coseno. */
double cosDegrees(double x) {
	double rad = (x*M_PI/180);
	return cos(rad);
}
