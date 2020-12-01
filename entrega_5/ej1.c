#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Este programa es para ver la estructura de memoria de un programa en C y cómo esto se expresa en la tabla de páginas de Linux.

int var_global;
int var_global_inicializada = 1;

int main() {
    int var_local;
    int var_local_inicializada = 1;
    int vector[4];
    printf("var_global: %p\n", &var_global);
    printf("var_global_inicializada: %p\n", &var_global_inicializada);
    printf("var_local: %p\n", &var_local);
    printf("var_local_inicializada: %p\n", &var_local_inicializada);
    printf("vector: %p\n", vector);
    printf("main: %p\n", main);

    getchar(); // Nos quedamos esperando para mantener vivo el proceso
}