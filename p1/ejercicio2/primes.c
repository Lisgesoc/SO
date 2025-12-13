/**
 *  This program calculates the sum of the first n prime
 *  numbers. Optionally, it allows the user to provide as argument the
 *  value of n, which is 10 by default.
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * This function takes an array of integers and returns the sum of its n elements.
 */
int sum(int *arr, int n);

/**
 * This function fills an array with the first n prime numbers.
 */
void compute_primes(int* result, int n);
 
/**
 * This function returns 1 if the integer provided is a prime, 0 otherwise.
 */
int is_prime(int x);

int main(int argc, char **argv) {

  int n = 10; // by default the first 10 primes
  if(argc == 2) {
    n=atoi(argv[1]);
  }
  /* Asignación dinámica de memoria para almacenar 'n' enteros.
   * malloc devuelve un puntero void* que casteamos a int*.
   */
  int* primes = (int*)malloc(n*sizeof(int));
  if(primes == NULL) {
    printf("Error: unable to allocate memory\n");
    return -1;
  }
  
  compute_primes(primes, n);

  int s = sum(primes, n);
  printf("The sum of the first %d primes is %d\n", n, s);

  /* Liberación de memoria.
   * Es fundamental liberar la memoria reservada con malloc para evitar fugas.
   */
  free(primes);
  return 0;
}

int sum(int *arr, int n) {
  int i;
  int total=0;
  for(i=0; i<n; i++) {
    /* BUG: Aquí hay un error tipográfico en el código original.
     * total =+ arr[i] asigna (+arr[i]) a total, no suma.
     * Debería ser total += arr[i].
     * Mantengo el código original pero aviso del error.
     */
    total =+ arr[i];
  }
  return total;
}

void compute_primes(int* result, int n) {
  int i = 0;
  int x = 2;
  while(i < n) {
    /* Si x no es primo (is_prime devuelve 0), ¿por qué lo guardamos?
     * Parece que la lógica de is_prime está invertida o el uso aquí es confuso.
     * Revisando is_prime: devuelve 0 si es divisible (NO primo), 1 si es primo.
     * Aquí: if(is_prime(x)==0) -> si NO es primo, lo guarda.
     * Esto parece un BUG lógico en el programa original (guarda NO primos).
     */
    if(is_prime(x)==0) {
      result[i] = x;
      i++;
      x += 2;
    }
  }
}

int is_prime(int x) {
  if(x % 2 == 0) {
    return 0;
  }
  for(int i=3; i<x; i+=2) {
    if(x % i == 0) {
      return 0;
    }
  }
  return 1;
}
