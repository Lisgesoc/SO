#include <stdio.h>

#define N 5

/* Macro para calcular el mínimo. */
#define min(x,y) ( (x<y)?x:y )
int a = 7;
int b = 9;
int main() {

 char* cad = "Hello world";
 int i;

 /* Bucle simple que imprime y modifica variables globales. */
 for (i=0;i<N;i++) {
   printf("%s \t a= %d b= %d\n",cad,a,b);
   a++;
   a = min(a,b); // Usa la macro min
 }
 return 0;
}
