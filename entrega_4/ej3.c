#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Ver información de los hilos con ps -M

void* esperar(void* flagEsperar) {
    while (*((char*)flagEsperar)) {};
}

int main() {
    pthread_t id1, id2, id3;

    int flagEsperar = 1; // Los hilos van a esperar para siempre

    printf("Mi PID es %d\n", getpid());

    pthread_create(&id1, NULL, esperar, (void*) &flagEsperar);
    pthread_create(&id2, NULL, esperar, (void*) &flagEsperar);
    pthread_create(&id3, NULL, esperar, (void*) &flagEsperar);

    sleep(5); // En sistemas con gestión de procesos a nivel usuario, esto provocaría un syscall (interrupción) que dormiría todo el proceso. En sistemas UNIX no pasa porque los gestiona el SO

    exit(EXIT_SUCCESS); // Esto mata a todos los hilos, porque esta es una llamada al sistema que finaliza el proceso en sí. Aun así, si este hilo acabase, como es el principal, acabaría todo el proceso igualmente porque se haría un exit() implícito
}