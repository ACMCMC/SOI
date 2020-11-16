#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>

// Ver información de los hilos con ps -M

#define MAX_SQRT 1000000000
#define INCR_SQRT 100

void* calcular(void *flagEsperar)
{
    int i;
    int res;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
    }
    printf("He acabado! (%d)\n", pthread_self());
    return NULL;
}

void* calcularGeneroso(void *flagEsperar)
{
    int i;
    int res;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
        sched_yield();
    }
    printf("(GENEROSO) He acabado! (%d)\n", pthread_self());
    return NULL;
}

int main()
{
    pthread_t id1, id2, id3;

    printf("Mi PID es %d\n", getpid());

    pthread_create(&id1, NULL, calcular, NULL);
    pthread_create(&id2, NULL, calcular, NULL);
    pthread_create(&id3, NULL, calcularGeneroso, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);

    exit(EXIT_SUCCESS); // Esto mata a todos los hilos, porque esta es una llamada al sistema que finaliza el proceso en sí. Aun así, si este hilo acabase, como es el principal, acabaría todo el proceso igualmente porque se haría un exit() implícito
}