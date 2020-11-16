#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int varGlobal;

void* imprimirYAumentarVars(void* args) {
    printf("Soy el hilo %d. Var global: %d. Var pasada como argumento: %d.\n", pthread_self(), varGlobal, *((int*)args));
    varGlobal++;
}

int main() {
    int varLocal;
    int* varDinamica;

    varGlobal = 10;
    varLocal = 5;
    varDinamica = (int*) malloc(sizeof(int));
    (*varDinamica)=7;

    pthread_t id1;
    pthread_t id2;

    pthread_create(&id1, NULL, imprimirYAumentarVars, &varLocal);
    pthread_create(&id1, NULL, imprimirYAumentarVars, varDinamica);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    free(varDinamica);
    exit(EXIT_SUCCESS);
}