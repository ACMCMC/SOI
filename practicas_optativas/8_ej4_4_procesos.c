#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>

#define NUM_CORES 4

#define MAX_SQRT 100000000000
#define INCR_SQRT 100

void calcular()
{
    long i;
    long res;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
    }
    printf("He acabado! (%d)\n", getpid());
}

void calcularGeneroso()
{
    long i, j;
    long res;
    j = 0;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
        if ((j++ % 100) == 0)
        {
            sched_yield();
        }
    }
    printf("(GENEROSO) He acabado! (%d)\n", getpid());
}

int main()
{
    pthread_t id1, id2, id3, id4, id5;
    int i;
    int stat_loc;
    pid_t pid;

    printf("(Padre) Mi PID es %d\n", getpid());

    for (i = 0; i < NUM_CORES; i++)
    {
        if (i == 0)
        { // El primer proceso que creemos será el generoso; el resto serán egoístas
            pid = fork();
            if (pid < 0)
            {
                perror("Error en fork()");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            { // Este es el hijo
                calcularGeneroso();
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            pid = fork();
            if (pid < 0)
            {
                perror("Error en fork()");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            { // Este es el hijo
                calcular();
                exit(EXIT_SUCCESS);
            }
        }
    }

    for (i = 0; i < NUM_CORES; i++)
    {
        if (wait(&stat_loc) < 0)
        {
            perror("Error en wait()");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}