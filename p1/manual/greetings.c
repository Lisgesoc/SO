#include <stdio.h>

int main(void) {
    char name[100]; // Buffer para almacenar el nombre

    printf("Enter your name: ");
    
    /* Lectura de entrada estándar con scanf.
     * scanf lee caracteres de stdin hasta encontrar un espacio en blanco.
     * Devuelve el número de items asignados correctamente.
     * Es importante verificar el valor de retorno para detectar errores o EOF.
     */
    if (scanf("%s", name) != 1) {
            printf("Error/EOF\n");
            return 1;
    } else {
            printf("Hi %s!!\n", name);
            return 0;
    }
}
