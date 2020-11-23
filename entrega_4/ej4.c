#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>

// Ver información de los hilos con ps -M

#define MAX_SQRT 100000000000
#define INCR_SQRT 100

void* calcular(void *flagEsperar)
{
    long i;
    long res;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
    }
    printf("He acabado! (%d)\n", pthread_self());
    return NULL;
}

void* calcularGeneroso(void *flagEsperar)
{
    long i, j;
    long res;
    j = 0;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
        if ((j++ % 100)==0) {
            sched_yield();
        }
    }
    printf("(GENEROSO) He acabado! (%d)\n", pthread_self());
    return NULL;
}

int main()
{
    pthread_t id1, id2, id3, id4, id5;

    printf("Mi PID es %d\n", getpid());

    pthread_create(&id1, NULL, calcular, NULL);
    pthread_create(&id2, NULL, calcular, NULL);
    pthread_create(&id5, NULL, calcular, NULL);
    pthread_create(&id4, NULL, calcular, NULL);
    pthread_create(&id3, NULL, calcularGeneroso, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);
    pthread_join(id4, NULL);
    pthread_join(id5, NULL);

    exit(EXIT_SUCCESS); // Esto mata a todos los hilos, porque esta es una llamada al sistema que finaliza el proceso en sí. Aun así, si este hilo acabase, como es el principal, acabaría todo el proceso igualmente porque se haría un exit() implícito
}