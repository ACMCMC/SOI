#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define NUM_CORES 4
#define NUM_HILOS 2

// Ver información de los hilos con ps -M

void *realizar_calculos(void *arg_proc);
int realizar_calculo(int i);

int d;

/*
 __    __   __   __        ______        _______.
|  |  |  | |  | |  |      /  __  \      /       |
|  |__|  | |  | |  |     |  |  |  |    |   (----`
|   __   | |  | |  |     |  |  |  |     \   \    
|  |  |  | |  | |  `----.|  `--'  | .----)   |   
|__|  |__| |__| |_______| \______/  |_______/    
                                                 

Aldán Creo Mariño, SOI 2020/21

*/

int main()
{
    int i, sumatorio_hilos, sumatorio_secuencial;
    int *arg_proc;
    void *ret_proc;
    pthread_t threads[NUM_HILOS];
    int res_parciales[NUM_HILOS];

    printf("Introduzca el valor de " ANSI_COLOR_YELLOW "d" ANSI_COLOR_RESET ": ");
    scanf(" %d", &d);

    for (i = 0; i < NUM_HILOS; i++)
    {
        pthread_create(threads + i, NULL, realizar_calculos, (void *)(intptr_t)(i));
    }

    for (i = 0; i < NUM_HILOS; i++)
    {
        pthread_join(threads[i], &ret_proc);
        res_parciales[i] = (int)ret_proc;
    }

    sumatorio_hilos = 0;
    for (i = 0; i < NUM_HILOS; i++)
    {
        sumatorio_hilos += res_parciales[i];
    }

    printf("Valor del cálculo con hilos: " ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, sumatorio_hilos);

    sumatorio_secuencial = 0;
    for (i = 0; i < d; i++)
    {
        sumatorio_secuencial += realizar_calculo(i);
    }

    printf("Valor del cálculo secuencial: " ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, sumatorio_secuencial);
    printf("Diferencia de los resultados: " ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, abs(sumatorio_secuencial - sumatorio_hilos));
}

void *realizar_calculos(void *arg_proc)
{
    int i, j, x, sumatorio_parcial;
    j = (int)arg_proc;
    sumatorio_parcial = 0;
    for (i = j; i < d; i += NUM_HILOS)
    {
        sumatorio_parcial += realizar_calculo(i);
    }
    return (void *)(intptr_t)(sumatorio_parcial);
}

int realizar_calculo(int i)
{
    int x, res;
    x = (i * 1000) / d - 500;
    res = -1 * (x * sin(sqrt(abs(x))));
    //printf("%d -> %d\n", x, res);
    return res;
}