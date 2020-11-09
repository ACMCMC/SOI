#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int contador_sigint = 0; // Esta variable es el contador global del cuántas veces hemos recibido SIGINT. Tiene que ser global, porque no hay conexión entre el main y el gestor_sigint

static void gestor_sigint(int signal); // La función que gestiona las interrupciones

int main()
{
    if (signal(SIGINT, gestor_sigint) == SIG_ERR)
    {
        perror("Error al asignar la señal. Abortando.");
        exit(EXIT_FAILURE);
    }
    for(;;);
}

static void gestor_sigint(int signal_id)
{
    char opcion;
    if (contador_sigint >= 4)
    {
        printf("Deseas volver a la gestión por defecto de SIGINT? (s/N): ");
        scanf(" %c", &opcion);
        if (opcion == 's' || opcion == 'S')
        {
            if (signal(SIGINT, SIG_DFL) == SIG_ERR)
            {
                perror("Error al asignar la señal. Abortando.");
                exit(EXIT_FAILURE);
            }
        } else { // Cualquier opción que no sea S
            printf("Elegido: %c. Se mantiene el comportamiento actual.\n", opcion);
        }
    }
    else
    {
        psignal(signal_id, "Se ha recibido la señal: ");
        contador_sigint++;
    }
}