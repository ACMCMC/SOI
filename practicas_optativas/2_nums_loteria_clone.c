#define _GNU_SOURCE // Necesario para poder usar clone()

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
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

#define STACK_SIZE 65536

/*
.__   __.  __    __  .___  ___.      _______.    __        ______   .___________. _______ .______       __       ___          ___   
|  \ |  | |  |  |  | |   \/   |     /       |   |  |      /  __  \  |           ||   ____||   _  \     |  |     /   \        |__ \  
|   \|  | |  |  |  | |  \  /  |    |   (----`   |  |     |  |  |  | `---|  |----`|  |__   |  |_)  |    |  |    /  ^  \          ) | 
|  . `  | |  |  |  | |  |\/|  |     \   \       |  |     |  |  |  |     |  |     |   __|  |      /     |  |   /  /_\  \        / /  
|  |\   | |  `--'  | |  |  |  | .----)   |      |  `----.|  `--'  |     |  |     |  |____ |  |\  \----.|  |  /  _____  \      / /_  
|__| \__|  \______/  |__|  |__| |_______/       |_______| \______/      |__|     |_______|| _| `._____||__| /__/     \__\    |____| 
                                                                                                                                    


Aldán Creo Mariño, SOI 2020/21
*/

double suma_doble_precision = 0; // El valor actual de la suma en doble precisión, lo declaro como variable global porque la comparten los hilos de contabilidad y de mostrar el valor de la suma; habría medios alternativos para hacerlo como pasarle a los hilos referencia a la misma posición de memoria, que guarde el resultado, pero me parecía excesivamente complicado para hacer algo tan sencillo como esto. No necesitamos protección de memoria entre hilos, porque suponemos que van a colaborar entre ellos, ya que están bajo nuestro control

// Creamos un tipo de dato que es una lista enlhAazada, para guardar los números de lotería que hemos creado
typedef struct num_lista
{
    int num;               // Cada elemento de la lista guarda un número, y
    struct num_lista *sig; // una referencia al elemento siguiente
} num_lista;

num_lista *primero_lista_enlazada_nums; // Una referencia global al principio de la lista, ya que necesitamos saber dónde empezar a leerla. El motivo por el que es global es el mismo que para suma_doble_precision.

// También declaramos una cola para los hilos trabajadores; iremos introduciéndolos en ella y haciendo phtread_join por orden
typedef struct elem_cola_hilos
{
    int elem;                    // El identificador de proceso
    struct elem_cola_hilos *sig; // El siguiente de la cola
} elem_cola_hilos;

typedef struct cola_hilos // Lo usamos para encapsular la cola, nos permite insertar más fácilmente por el final y retirar por el principio. Además nos quedamos con el número de elementos de la cola, aunque podríamos calcularlo
{
    elem_cola_hilos *principio;
    elem_cola_hilos *final;
    int num_elems;
} cola_hilos;

cola_hilos cola_threads; // Una variable global también para la cola de hilos trabajadores. El motivo por el que es global es el mismo que para suma_doble_precision.

//Función privada que genera un número aleatorio entre 00000 y 99999
int gen_num_loteria()
{
    return (rand() % (100000));
}

// Mira si el número ya ha sido generado antes
int num_ya_generado(int num)
{
    num_lista elem_lista;
    elem_lista = *primero_lista_enlazada_nums; // Nos quedamos con el primer elemento de la lista para empezar
    while (elem_lista.sig != NULL)             // Mientras queden elementos en la lista...
    {
        if (elem_lista.num == num) // Si el número coincide, devolvemos TRUE
        {
            return 1;
        }
        elem_lista = *(elem_lista.sig); // Miramos el siguiente elemento
    }
    return 0; // No se ha encontrado ese número
}

// Añade un número a la lista de números generados
void anadir_lista(int num)
{
    num_lista *elem_lista = (num_lista *)malloc(sizeof(num_lista)); // Guardamos memoria para un nuevo elemento de la lista
    elem_lista->num = num;                                          // Establecemos el valor de este elemento
    elem_lista->sig = primero_lista_enlazada_nums;                  // Insertamos el elemento al principio, así que el siguiente elemento de la lista será el que era el primero
    primero_lista_enlazada_nums = elem_lista;                       // El primero de la lista es este elemento
}

// Devuelve TRUE si la cola de hilos está vacía
int es_vacia_cola_hilos()
{
    return cola_threads.principio == NULL; // Si el prinncipio de la cola es NULL, entonces la cola está vacía
}

// Añade un hilo a la cola de hilos
void anadir_cola_hilos(int thread)
{
    // Vamos a crear un nuevo elemento en la cola de hilos.

    elem_cola_hilos *elem = (elem_cola_hilos *)malloc(sizeof(elem_cola_hilos)); // Reservamos memoria para un nuevo elemento de la cola
    elem->sig = NULL;                                                           // El elemento lo vamos a meter al final, así que el siguiente a este no existe
    elem->elem = thread;                                                        // El valor del elemento que vamos a meter en la cola será el identificador de hilo que le pasamos a esta función
    if (!es_vacia_cola_hilos())
    {
        cola_threads.final->sig = elem;
    }
    else
    { // Si la cola es vacía, entonces el primero elemento será este
        cola_threads.principio = elem;
    }
    cola_threads.final = elem; // El final de la cola será este elemento
    cola_threads.num_elems++;  // Incrementamos el contador de elementos de la cola
}

// Devuelve el primer elemento de la cola de hilos, y lo elimina de la cola
int primero_cola_hilos()
{
    int thread;
    elem_cola_hilos *sig_elem;              // Necesitamos una referencia al siguente elemento del primero de la cola, para borrar al primero y apuntar al siguiente
    thread = cola_threads.principio->elem;  // Obtenemos el valor del primero de la cola
    sig_elem = cola_threads.principio->sig; // Guardamos la referencia al siguiente
    free(cola_threads.principio);           // Liberamos la memoria del primero
    cola_threads.principio = sig_elem;      // Ahora el primero es el siguiente elemento
    cola_threads.num_elems--;               // Hay un elemento menos en la cola
    return thread;                          // Devolvemos el valor que estaba almacenado en el primer elemento
}

// Si recibimos SIGUSR1, entonces acabamos este proceso
static void gestor_sigusr1()
{
    // Liberamos la lista enlazada antes de salir, aunque realmente tampoco haría falta porque como se va a acabar el proceso toda nuestra memoria se va a volver libre al momento.
    num_lista *elemsig = primero_lista_enlazada_nums;
    num_lista *elem;
    while (elemsig != NULL)
    {
        elem = elemsig;
        elemsig = elem->sig;
        free(elem);
    }

    while (!es_vacia_cola_hilos())
    { // Vacío también la cola de hilos, de nuevo no es tan importante hacerlo pero es una buena práctica. Eso sí, no espero a que acaben.
        primero_cola_hilos();
    }

    printf("Salgo.\n");

    exit(EXIT_SUCCESS); // Acabamos el proceso
}

// Muestra cada 7 segundos el valor de suma_doble_precision
int hilo_mostrar_sumas()
{
    while (1)
    {
        sleep(7);
        printf("\nValor calculado hasta ahora: " ANSI_COLOR_MAGENTA "%lf" ANSI_COLOR_RESET "\n\n", suma_doble_precision);
    }
    return 0; // Nunca vamos a llegar aquí
}

// Si hay hilos trabajadores ejecutándose, espera a que acaben y lee su valor de retorno (el número de lotería elegido) para sumarlo a suma_doble_precision
int hilo_contabilidad()
{
    int hilo;
    int res_hilo;
    while (1)
    {
        if (!es_vacia_cola_hilos()) // Si hay hilos trabajadores ejecutándose (están en la cola)...
        {
            hilo = primero_cola_hilos();
            waitpid(hilo, &res_hilo, 0);
            suma_doble_precision += (double)res_hilo; // Sumo su valor de retorno
        }
        else
        {
            sched_yield(); // Cedemos el procesador a otros hilos
        }
    }
    return 0;
}

int hilo_trabajador(void *term)
{
    int num_loteria;
    int terminacion = (int)(intptr_t)term; // Obtenemos la terminación, que se nos había pasado como argumento
    do
    {
        num_loteria = gen_num_loteria();                                         // Generamos nuevos números de lotería...
    } while ((num_loteria % 10) != terminacion || num_ya_generado(num_loteria)); // Mientras no terminen en el número que queremos o ya hayan sido creados antes
    anadir_lista(num_loteria);                                                   // Añadimos el número a la lista de números de lotería que ya han salido
    sleep(8);                                                                    // Esperamos 8 segundos, no hace falta comprobar esta llamada al sistema porque siempre devuelve -1
    printf("Numero generado: %d\n", num_loteria);                                // Imprimimos el número, info para el usuario solamente
    return num_loteria;
}

int main()
{
    int hilo_cont;   // El identificador del hilo que se encarga de ir recibiendo los valores de retorno de los hilos del cálculo y va sumando sus valores
    int hilo_sumas;  // Muestra el valor actual cada 7 segundos
    int hilo;        // Lo usamos para identificar el hilo trabajador que crearemos después, no se refiere a un hilo concreto sino que se usará dentro de un bucle para identificar al hilo que en ese momento queramos identificar. Los identificadores de todos los hilos se guardan en una cola, para que el hilo de contabilidad vaya leyendo sus valores de retorno
    int terminacion; // La terminación que hemos elegido
    void *stack;     // Puntero al stack que reservaremos para cada hilo

    printf("Para matarme, ejecuta " ANSI_COLOR_RED "kill " ANSI_COLOR_MAGENTA "-s SIGUSR1 " ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET "\n", getpid());

    cola_threads.num_elems = 0; // Inicializamos el número de elementos de la cola a 0. Podríamos hacerlo con una función auxiliar, pero me parece complicar innecesariamente el código

    // Inicializamos la lista de números, al principio solo contendrá el -1, que indica que no hay números
    primero_lista_enlazada_nums = (num_lista *)malloc(sizeof(num_lista)); // Reservamos espacio para el primer elemento de la lista
    primero_lista_enlazada_nums->sig = NULL;                              // No hay un elemento siguiente al primero
    primero_lista_enlazada_nums->num = -1;                                // El primero vale -1, para indicar que no tiene un valor (decisión arbitraria, podríamos implementarlo de forma más estándar con constantes y funciones auxiliares pero me parece escribir código innecesario para un programa tan sencillo)

    srand((int)time(NULL)); // Semilla para generar números de lotería aleatoriamente

    if (signal(SIGUSR1, gestor_sigusr1) == SIG_ERR)
    { // El proceso se termina con SIGUSR1
        perror("Error en signal()");
    }
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    { // Ignoramos SIGINT, el proceso se termina con SIGUSR1
        perror("Error en signal()");
    }

    stack = malloc(STACK_SIZE); // Porque el stack crece hacia abajo, tendremos que sumarle STACK_SIZE al pasarlo a clone()

    if ((hilo_cont = clone(hilo_contabilidad, stack + STACK_SIZE, CLONE_VM | CLONE_THREAD | CLONE_SIGHAND, NULL)) < 0) // Creamos el hilo de contabilidad
    {
        perror("Error creando el hilo de contabilidad");
    }

    stack = malloc(STACK_SIZE);
    if ((hilo_sumas = clone(hilo_mostrar_sumas, stack + STACK_SIZE, CLONE_VM | CLONE_THREAD | CLONE_SIGHAND, NULL)) < 0) // El hilo que va mostrando el valor de las sumas cada 7 segundos
    {
        perror("Error creando el hilo de mostrar sumas");
    }

    while (1)
    {
        printf("Introduce una terminación: ");
        scanf(" %d", &terminacion);
        if (cola_threads.num_elems < 4)
        { // Solo creamos un hilo trabajador si hay menos de 4
            stack = malloc(STACK_SIZE);
            if ((hilo = clone(hilo_trabajador, stack + STACK_SIZE, CLONE_VM | CLONE_THREAD | CLONE_SIGHAND, (void *)(intptr_t)(terminacion % 10))) < 0) // Creamos un hilo trabajador que calcule un número con esa terminación (la ponemos en módulo 10 por si acaso el usuario escribiese mal el argumento)
            {                                                                                                                                           // Creamos un hilo que calcule esa terminación
                perror("Error creando el hilo");
            }
            else
            {
                anadir_cola_hilos(hilo);
            }
        }
        else
        { // Informamos al usuario de que ya hay 4 hilos
            printf(ANSI_COLOR_RED "Ya hay 4 hilos trabajando" ANSI_COLOR_RESET ". Espera a que acabe uno antes de calcular un nuevo número.\n");
        }
    }
}
