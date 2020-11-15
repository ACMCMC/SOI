#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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
        printf("Deseas volver a la gestión por defecto de SIGINT? (" ANSI_COLOR_GREEN "s" ANSI_COLOR_RESET "/" ANSI_COLOR_RED "N" ANSI_COLOR_RESET "): ");
        scanf(" %c", &opcion);
        if (opcion == 's' || opcion == 'S')
        {
            if (signal(SIGINT, SIG_DFL) == SIG_ERR)
            {
                perror("Error al asignar la señal. Abortando.");
                exit(EXIT_FAILURE);
            }
            printf(ANSI_COLOR_GREEN "Se ha vuelto a la gestión por defecto.\n" ANSI_COLOR_RESET);
        } else { // Cualquier opción que no sea S
            printf("Elegido: " ANSI_COLOR_RED "%c" ANSI_COLOR_RESET ". Se mantiene el comportamiento actual.\n", opcion);
        }
    }
    else
    {
        psignal(signal_id, "Se ha recibido la señal");
        contador_sigint++;
    }
}