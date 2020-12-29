#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

// Códigos de color para formatear la salida en consola
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define NUM_CORES 4
#define NUM_HILOS NUM_CORES * 2

void realizar_calculos(int arg_proc, double *ubicacion_retorno, int *d);
double realizar_calculo(int i, double d);

/*
.______   .______        ______     ______  _______     _______.  ______        _______.
|   _  \  |   _  \      /  __  \   /      ||   ____|   /       | /  __  \      /       |
|  |_)  | |  |_)  |    |  |  |  | |  ,----'|  |__     |   (----`|  |  |  |    |   (----`
|   ___/  |      /     |  |  |  | |  |     |   __|     \   \    |  |  |  |     \   \    
|  |      |  |\  \----.|  `--'  | |  `----.|  |____.----)   |   |  `--'  | .----)   |   
| _|      | _| `._____| \______/   \______||_______|_______/     \______/  |_______/    
                                                                                        
                                                 

Aldán Creo Mariño, SOI 2020/21

*/

int main()
{
    int i;                                                       // Variable auxiliar
    double sumatorio_paralelo = 0.0, sumatorio_secuencial = 0.0; // Resultados de los sumatorios, usando hilos, y en un solo hilo
    int stat_loc;                                                // Para recibir el valor de salida de un proceso
    pid_t pids[NUM_HILOS];                                       // Para el valor de retorno de wait()
    struct timespec tinicio, tfin;                               // Para medir tiempos
    double *region_compartida;                                   // La región donde los procesos almacenarán sus resultados
    int *d;                                                      // La región donde los procesos almacenarán sus resultados

    d = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // Hacemos un mapeo compartido de la variable d, ya que aunque podríamos leerla en el padre y pasarla a los hijos, en el enunciado pone que hay que definir "una zona de memoria compartida para almacenar los datos que deben compartirse entre los procesos", interpreto que d también debe estar en una zona de memoria compartida.
    printf("Introduzca el valor de " ANSI_COLOR_YELLOW "d" ANSI_COLOR_RESET ": ");                 // Obtenemos el valor de d
    scanf(" %d", d);

    region_compartida = (double *)mmap(NULL, sizeof(double) * NUM_HILOS, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // Mapeamos la memoria. Básicamente, crearemos una región en el espacio virtual para guardar NUM_HILOS doubles. Por ejemplo, si tenemos 8 hilos, reservaremos espacio para guardar 8 doubles.

    clock_gettime(CLOCK_MONOTONIC, &tinicio); // Obtenemos el tiempo de inicio para el cálculo multihilo

    for (i = 0; i < NUM_HILOS; i++) // Creamos NUM_HILOS para hacer los cálculos en paralelo
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("Error en fork().");
            exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0)
        {                                                   // Este es el hijo
            realizar_calculos(i, region_compartida + i, d); // En el hijo, la función realizar_calculos se encarga de realizar los cálculos necesarios, y además los guarda en el i-lugar de la región compartida, para que así después el padre pueda leer el resultado.
            exit(EXIT_SUCCESS);                             // Como este es el hijo, salimos con EXIT_SUCCESS tras ejecutar la función
        }                                                   // Si no, es el padre, y el flujo continúa normalmente.
    }

    for (i = 0; i < NUM_HILOS; i++) // Esperamos a que acaben todos los procesos
    {
        if (waitpid(pids[i], &stat_loc, 0) != pids[i])
        { // Vamos esperando a todos los hijos
            perror("Error esperando al proceso hijo");
            exit(EXIT_FAILURE);
        }
        if (!(WIFEXITED(stat_loc) && WEXITSTATUS(stat_loc) == EXIT_SUCCESS))
        {
            fprintf(stderr, "El hijo ha salido de forma inesperada. Abortamos.\n");
            exit(EXIT_FAILURE);
        }
        sumatorio_paralelo += region_compartida[i]; // Sumamos el resultado que nos ha almacenado en la región de memoria compartida
    }

    clock_gettime(CLOCK_MONOTONIC, &tfin); // Obtenemos el tiempo de fin para el cálculo multihilo

    printf("Valor del cálculo con procesos: " ANSI_COLOR_GREEN "%lf" ANSI_COLOR_RESET " ( %.6f segundos )\n", sumatorio_paralelo, ((double)tfin.tv_sec + 1.0e-9 * tfin.tv_nsec) - ((double)tinicio.tv_sec + 1.0e-9 * tinicio.tv_nsec)); // Lo imprimimos

    clock_gettime(CLOCK_MONOTONIC, &tinicio); // Obtenemos el tiempo de inicio para el cálculo secuencial

    for (i = 0; i <= *d; i++)
    {
        sumatorio_secuencial += realizar_calculo(i, *d); // Realizamos todos los cálculos de forma secuencial
    }

    clock_gettime(CLOCK_MONOTONIC, &tfin); // Obtenemos el tiempo de fin para el cálculo secuencial

    printf("Valor del cálculo secuencial: " ANSI_COLOR_GREEN "%lf" ANSI_COLOR_RESET " ( %.6f segundos )\n", sumatorio_secuencial, ((double)tfin.tv_sec + 1.0e-9 * tfin.tv_nsec) - ((double)tinicio.tv_sec + 1.0e-9 * tinicio.tv_nsec));
    printf("Diferencia de los resultados: " ANSI_COLOR_RED "%lf\n" ANSI_COLOR_RESET, fabs(sumatorio_secuencial - sumatorio_paralelo));

    munmap(region_compartida, sizeof(double) * NUM_HILOS); // Eliminamos el mapeo de las regiones
    munmap(d, sizeof(int));

    exit(EXIT_SUCCESS);
}

void realizar_calculos(int arg_proc, double *ubicacion_retorno, int *d)
{
    int i;                    // Variable auxiliar
    int j;                    // El número de este hilo
    double sumatorio_parcial; // El valor que devolverá este hilo
    j = arg_proc;
    sumatorio_parcial = 0.0;
    for (i = j; i <= *d; i += NUM_HILOS) // Vamos realizando los cálculos en la secuencia (j + NUM_HILOS*i): Si tenemos 3 hilos y este es el 2, sería 1, 4, 7, ...
    {
        sumatorio_parcial += realizar_calculo(i, (double)*d);
    }
    (*ubicacion_retorno) = sumatorio_parcial; // Guardamos el resultado en la zona de memoria compartida, que dentro de esta función sólo es un puntero al lugar adecuado
}

double realizar_calculo(int i, double d) // Realizamos el cálculo para un valor x
{
    double x;                               // Dado el índice de este cálculo, obtendremos aquí el valor de x para la fórmula
    double res;                             // El resultado
    x = (((double)i) * 1000.0) / (d)-500.0; // Calculamos x
    res = -1.0 * (x * sin(sqrt(fabs(x))));  // Aplicamos la fórmula
    return res;
}
