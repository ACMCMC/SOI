#define _GNU_SOURCE // Necesario para poder usar clone()

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>
#include <sys/mman.h>

#define NUM_CORES 4
#define STACK_SIZE (1024*1024)

#define MAX_SQRT 100000000000
#define INCR_SQRT 100

int calcular(void *flagEsperar)
{
    long i;
    long res;
    for (i = 0; i < MAX_SQRT; i += INCR_SQRT)
    {
        res = sqrt(i);
    }
    printf("He acabado!\n");
    *((int*)flagEsperar) = 1;
    return 0;
}

int calcularGeneroso(void *flagEsperar)
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
    printf("(GENEROSO) He acabado!\n");
    *((int*)flagEsperar) = 1;
    return 0;
}

int main()
{
    pthread_t id1, id2, id3, id4, id5;
    int* flags_hilos = (int*) calloc(NUM_CORES, sizeof(int));
    int i, flag_esperar;
    void** stack = (void**) malloc(sizeof(void*)*NUM_CORES);

    printf("Mi PID es %d\n", getpid());

    for (i = 0; i < NUM_CORES; i++) {
        stack[i] = (void*) mmap(0, STACK_SIZE, PROT_WRITE|PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (stack[i] == MAP_FAILED) {
            perror("Ha fallado mmap");
            exit(EXIT_FAILURE);
        }
        if (i==0) { // El primer hilo que creemos será generoso; el resto, egoístas
        clone(calcularGeneroso, stack[i]+STACK_SIZE, CLONE_VM | CLONE_THREAD | CLONE_SIGHAND, (void*)(flags_hilos+i)); // stack+STACK_SIZE porque crece hacia abajo
        } else {
        clone(calcular, stack[i]+STACK_SIZE, CLONE_VM | CLONE_THREAD | CLONE_SIGHAND, (void*)(flags_hilos+i)); // stack+STACK_SIZE porque crece hacia abajo
        }
    }

    do {
        flag_esperar = 0;
        for (int i = 0; i < NUM_CORES; i++) {
            if (flags_hilos[i] == 0) {
                flag_esperar = 1;
            }
        }
    } while (flag_esperar);

    for (int i = 0; i < NUM_CORES; i++) {
        munmap(stack[i], STACK_SIZE);
    }

    printf("El padre acaba.\n");

    exit(EXIT_SUCCESS); // Esto mata a todos los hilos, porque esta es una llamada al sistema que finaliza el proceso en sí. Aun así, si este hilo acabase, como es el principal, acabaría todo el proceso igualmente porque se haría un exit() implícito
}