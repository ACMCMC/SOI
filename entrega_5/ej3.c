#include <stdio.h>
#include <stdlib.h>

int main() {

    printf("Antes del malloc\n");
    scanf("%*d");

    void* p = malloc(1024);
    if (p==NULL) {
        perror("Error en malloc");
    }

    printf("Dirección asignada: %p\n", p);
    scanf("%*d");

    free(p); // No podemos comprobar que ha tenido éxito, ya que no se devuelve nada
    
    printf("Liberada la memoria\n");
    scanf("%*d");

    printf("Antes del malloc\n");
    scanf("%*d");

    p = malloc(1024*1024*1024);
    if (p==NULL) {
        perror("Error en malloc");
    }

    printf("Dirección asignada: %p\n", p);
    scanf("%*d");

    free(p); // No podemos comprobar que ha tenido éxito, ya que no se devuelve nada
    
    printf("Liberada la memoria\n");
    scanf("%*d");

    exit(EXIT_SUCCESS);
}