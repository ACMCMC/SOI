#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

double suma_doble_precision = 0;

typedef struct num_lista {
    unsigned int num;
    struct num_lista* sig;
} num_lista;

num_lista* lista_enlazada_nums;

//Función privada que genera un número aleatorio entre inf y sup
unsigned int gen_num_loteria()
{
    return (rand() % (100000));
}

// Mira si el número ya ha sido generado antes
int num_ya_generado(unsigned int num) {
    num_lista elem_lista;
    elem_lista = *lista_enlazada_nums;
    while (elem_lista.sig!=NULL) {
        if (elem_lista.num==num) {
            printf("Ya generado: %d\n", elem_lista.num);
            return 1;
        }
        elem_lista = *(elem_lista.sig);
    }
    return 0;
}

// Añade un número a la lista de números generados
void anadir_lista(unsigned int num) {
    num_lista* elem_lista = (num_lista*) malloc(sizeof(num_lista));
    elem_lista->num = num;
    elem_lista->sig = lista_enlazada_nums;
    lista_enlazada_nums = elem_lista;
}

static void gestor_sigusr1() {
    // Liberamos la lista enlazada antes de salir
    num_lista *elemsig = lista_enlazada_nums;
    num_lista *elem;
    while (elemsig!=NULL) {
        elem = elemsig;
        elemsig = elem->sig;
        free(elem);
    }

    printf("Salgo.\n");

    exit(EXIT_SUCCESS);
}

void* hilo_trabajador(void* term) {
    unsigned int num_loteria;
    int terminacion = (int)(uintptr_t)term;
    do {
        num_loteria = gen_num_loteria();
    } while ((num_loteria%10)!=terminacion || num_ya_generado(num_loteria));
    anadir_lista(num_loteria);
    sleep(8);
    printf("Numero generado: %d\n", num_loteria);
}

int main() {
    pthread_t* hilos = NULL;
    int terminacion;
    int num_hilos = 0;

    // Inicializamos la lista
    lista_enlazada_nums = (num_lista*) malloc(sizeof(num_lista));
    lista_enlazada_nums->sig=NULL;
    lista_enlazada_nums->num=-1;

    srand((unsigned int)time(NULL)); // Semilla para generar números de lotería aleatoriamente

    if(signal(SIGUSR1, gestor_sigusr1)==SIG_ERR) { // El proceso se termina con SIGUSR1
        perror("Error en signal()");
    }
    if(signal(SIGINT, SIG_IGN)==SIG_ERR) { // Ignoramos SIGINT, el proceso se termina con SIGUSR1
        perror("Error en signal()");
    }

    while (1) {
        printf("Introduce una terminación: ");
        scanf(" %d", &terminacion);
        hilos = (pthread_t *) realloc(hilos, sizeof(pthread_t)*(num_hilos+1));
        pthread_create(hilos+num_hilos, NULL, hilo_trabajador, (void*)(uintptr_t)(terminacion%10));
        num_hilos++;
    }
}
