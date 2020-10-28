#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_THREADS 10

void func(void* i) {
    printf("Número de hilo: %d\n", (int) i);
}

int main() {
    int i;
    pthread_t* p = (pthread_t*) malloc(sizeof(pthread_t)*NUMBER_THREADS);
    for (i = 0; i < NUMBER_THREADS; i++) {
        if (pthread_create(p+i, NULL, &func, (void*) i)) {
            perror("Error creando el hilo.");
        }
    }
    for (i = 0; i < NUMBER_THREADS; i++) {
        if (pthread_join(p[i], NULL)) {
            perror("Error ejecutando pthread_join");
        }
    }
    exit(EXIT_SUCCESS);
}
