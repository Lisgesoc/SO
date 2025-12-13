#include <stdio.h>
#include <string.h>

int main()
{
	char msg[10]; /* array of 10 chars. Memoria estática (stack). */
	char *p;      /* pointer to a char. */
	char msg2[28] = "Hello";  /* Inicialización con literal de cadena. */

	//p   = "Bonjour";
	/* Imprimimos contenido (cadenas) y direcciones de memoria.
	 * msg no está inicializado, contendrá basura.
	 * p no está inicializado, apuntará a basura.
	 */
	printf("msg: %s, p: %s, msg2: %s\n", msg, p, msg2);
	printf("dir de msg: %p, dir de p: %p, dir de msg2: %p\n", msg, p, msg2);

	/* Hacemos que p apunte al inicio del array msg2. */
	p = msg2;
	printf("msg: %s, p: %s, msg2: %s\n", msg, p, msg2);
	printf("dir de msg: %p, dir de p: %p, dir de msg2: %p\n", msg, p, msg2);

	/* Modificamos msg2 a través del puntero p. */
	p[0] = 'H', p[1] = 'i', p[2] = '\0';
	printf("msg: %s, p: %s, msg2: %s\n", msg, p, msg2);
	
	/* strlen devuelve la longitud de la cadena (hasta el primer \0).
	 * sizeof devuelve el tamaño reservado en memoria.
	 */
	printf("msg len: %lu p len %lu msg2 len %lu\n", strlen(msg), strlen(p), strlen(msg2));
	printf("msg size: %lu p size %lu msg2 size %lu\n", sizeof(msg), sizeof(p), sizeof(msg2));

	/* Modificamos msg. OJO: msg no tenía terminador nulo garantizado antes. */
	msg[0] = 'B', msg[1] = 'y';
	printf("msg: %s, p: %s, msg2: %s\n", msg, p, msg2);
}

