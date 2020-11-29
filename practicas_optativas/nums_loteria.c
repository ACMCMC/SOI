#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

double suma_doble_precision = 0;

typedef struct num_lista
{
    unsigned int num;
    struct num_lista *sig;
} num_lista;

num_lista *lista_enlazada_nums;

typedef struct elem_cola_procesos
{
    pthread_t elem;
    struct elem_cola_procesos *sig;
} elem_cola_procesos;

typedef struct cola_procesos {
    elem_cola_procesos* principio;
    elem_cola_procesos* final;
    int num_elems;
} cola_procesos;

cola_procesos cola_proc;

//Función privada que genera un número aleatorio entre inf y sup
unsigned int gen_num_loteria()
{
    return (rand() % (100000));
}

// Mira si el número ya ha sido generado antes
int num_ya_generado(unsigned int num)
{
    num_lista elem_lista;
    elem_lista = *lista_enlazada_nums;
    while (elem_lista.sig != NULL)
    {
        if (elem_lista.num == num)
        {
            printf("Ya generado: %d\n", elem_lista.num);
            return 1;
        }
        elem_lista = *(elem_lista.sig);
    }
    return 0;
}

// Añade un número a la lista de números generados
void anadir_lista(unsigned int num)
{
    num_lista *elem_lista = (num_lista *)malloc(sizeof(num_lista));
    elem_lista->num = num;
    elem_lista->sig = lista_enlazada_nums;
    lista_enlazada_nums = elem_lista;
}

// Devuelve TRUE si la cola de procesos está vacía
int es_vacia_cola_procesos()
{
    return cola_proc.principio == NULL;
}

// Añade un proceso a la cola de procesos
void anadir_cola_procesos(pthread_t proc)
{
    // Vamos a crear un nuevo elemento en la cola de procesos.

    elem_cola_procesos* elem = (elem_cola_procesos*) malloc(sizeof(elem_cola_procesos)); // Reservamos memoria para un nuevo elemento de la cola
    elem->sig = NULL; // El elemento lo vamos a meter al final, así que el siguiente a este no existe
    elem->elem = proc; // El valor del elemento que vamos a meter en la cola será el identificador de proceso que le pasamos a esta función
    if (!es_vacia_cola_procesos()) {
        cola_proc.final->sig = elem;
    } else { // Si la cola es vacía, entonces el primero elemento será este
        cola_proc.principio = elem;
    }
    cola_proc.final = elem; // El final de la cola será este elemento
    cola_proc.num_elems++; // Incrementamos el contador de elementos de la cola
}

// Devuelve el primer elemento de la cola de procesos, y lo elimina de la cola
pthread_t primero_cola_procesos()
{
    pthread_t proc;
    elem_cola_procesos* sig_elem;
    proc = cola_proc.principio->elem;
    sig_elem = cola_proc.principio->sig;
    free(cola_proc.principio);
    cola_proc.principio = sig_elem;
    cola_proc.num_elems--;
    return proc;
}

static void gestor_sigusr1()
{
    // Liberamos la lista enlazada antes de salir
    num_lista *elemsig = lista_enlazada_nums;
    num_lista *elem;
    while (elemsig != NULL)
    {
        elem = elemsig;
        elemsig = elem->sig;
        free(elem);
    }

    printf("Salgo.\n");

    exit(EXIT_SUCCESS);
}

void *hilo_mostrar_sumas()
{
    while (1) {
        sleep(7);
        printf("\nValor calculado hasta ahora: %lf\n\n", suma_doble_precision);
    }
}

void *hilo_contabilidad()
{
    pthread_t hilo;
    void* retorno_hilo;
    unsigned int res_hilo;
    while (1)
    {
        if (!es_vacia_cola_procesos())
        {
            hilo = primero_cola_procesos();
            printf("Waiting to join thread: %p\n", hilo);
            pthread_join(hilo, &retorno_hilo);
            res_hilo = *((unsigned int*) retorno_hilo);
            suma_doble_precision += (double) res_hilo;
            free(retorno_hilo);
            printf("Joined thread: %p\n", hilo);
        } else {
            sched_yield();
        }
    }
}

void *hilo_trabajador(void *term)
{
    unsigned int* num_loteria = malloc(sizeof(unsigned int));
    int terminacion = (int)(uintptr_t)term;
    do
    {
        *num_loteria = gen_num_loteria();
    } while ((*num_loteria % 10) != terminacion || num_ya_generado(*num_loteria));
    anadir_lista(*num_loteria);
    sleep(8);
    printf("Numero generado: %d\n", *num_loteria);
    return num_loteria;
}

int main()
{
    pthread_t hilo_cont;
    pthread_t hilo_sumas;
    pthread_t hilo;
    int terminacion;

    cola_proc.num_elems = 0;

    // Inicializamos la lista
    lista_enlazada_nums = (num_lista *)malloc(sizeof(num_lista));
    lista_enlazada_nums->sig = NULL;
    lista_enlazada_nums->num = -1;

    srand((unsigned int)time(NULL)); // Semilla para generar números de lotería aleatoriamente

    if (signal(SIGUSR1, gestor_sigusr1) == SIG_ERR)
    { // El proceso se termina con SIGUSR1
        perror("Error en signal()");
    }
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    { // Ignoramos SIGINT, el proceso se termina con SIGUSR1
        perror("Error en signal()");
    }

    if (pthread_create(&hilo_cont, NULL, hilo_contabilidad, NULL))
    {
        perror("Error creando el hilo de contabilidad");
    }

    if (pthread_create(&hilo_sumas, NULL, hilo_mostrar_sumas, NULL))
    {
        perror("Error creando el hilo de mostrar sumas");
    }

    while (1)
    {
        printf("Introduce una terminación: ");
        scanf(" %d", &terminacion);
        if (pthread_create(&hilo, NULL, hilo_trabajador, (void *)(uintptr_t)(terminacion % 10)))
        { // Creamos un hilo que calcule esa terminación
            perror("Error creando el hilo");
        }
        else
        {
            anadir_cola_procesos(hilo);
        }
    }
}
