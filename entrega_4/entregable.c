#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>

// Códigos de color para formatear la salida en consola
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
    int i; // Variable auxiliar
    int sumatorio_hilos, sumatorio_secuencial; // Resultados de los sumatorios, usando hilos, y en un solo hilo
    void *ret_proc; // Para recibir el valor de retorno de un hilo
    pthread_t threads[NUM_HILOS]; // Los identificadores de los hilos que vamos a crear
    int res_parciales[NUM_HILOS]; // Los resultados parciales de cada hilo

    printf("Introduzca el valor de " ANSI_COLOR_YELLOW "d" ANSI_COLOR_RESET ": "); // Obtenemos el valor de d
    scanf(" %d", &d);

    for (i = 0; i < NUM_HILOS; i++) // Creamos NUM_HILOS para hacer los cálculos en paralelo
    {
        pthread_create(threads + i, NULL, realizar_calculos, (void *)(intptr_t)(i)); // El primer argumento es dónde guardar el ID de hilo, el segundo es NULL porque no queremos especificar ningún parámetro en especial, el tercero es la función de entrada, y el cuarto es el argumento que le pasamos a esa función (un puntero, pero en este caso no nos importará a qué dirección de memoria apunta sino solamente su valor)
    }

    for (i = 0; i < NUM_HILOS; i++) // Esperamos a que acaben todos los hilos y guardamos sus resultados
    {
        pthread_join(threads[i], &ret_proc);
        res_parciales[i] = (int)ret_proc;
    }

    sumatorio_hilos = 0;
    for (i = 0; i < NUM_HILOS; i++) // Sumamos los resultados parciales para obtener el resultado global
    {
        sumatorio_hilos += res_parciales[i];
    }

    printf("Valor del cálculo con hilos: " ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, sumatorio_hilos); // Lo imprimimos


    sumatorio_secuencial = 0;
    for (i = 0; i < d; i++)
    {
        sumatorio_secuencial += realizar_calculo(i); // Realizamos todos los cálculos de forma secuencial
    }

    printf("Valor del cálculo secuencial: " ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, sumatorio_secuencial);
    printf("Diferencia de los resultados: " ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, abs(sumatorio_secuencial - sumatorio_hilos));
}

void *realizar_calculos(void *arg_proc)
{
    int i; // Variable auxiliar
    int j; // El número de este hilo
    int sumatorio_parcial; // El valor que devolverá este hilo
    j = (int)arg_proc;
    sumatorio_parcial = 0;
    for (i = j; i < d; i += NUM_HILOS) // Vamos realizando los cálculos en la secuencia (j + NUM_HILOS*i): Si tenemos 3 hilos y este es el 2, sería 1, 4, 7, ...
    {
        sumatorio_parcial += realizar_calculo(i);
    }
    return (void *)(intptr_t)(sumatorio_parcial); // Devolvemos el resultado, como debe ser casteado a forma de puntero trataremos el valor del puntero como el valor de retorno
}

int realizar_calculo(int i) // Realizamos el cálculo para un valor x
{
    int x; // Dado el índice de este cálculo, obtendremos aquí el valor de x para la fórmula
    int res; // El resultado
    x = (i * 1000) / d - 500; // Calculamos x
    res = -1 * (x * sin(sqrt(abs(x)))); // Aplicamos la fórmula
    return res;
}
