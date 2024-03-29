#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Códigos de color para formatear la salida en consola
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*
 _______ .__   __. .___________..______       _______   _______      ___      .______    __       _______     __  
|   ____||  \ |  | |           ||   _  \     |   ____| /  _____|    /   \     |   _  \  |  |     |   ____|   /_ | 
|  |__   |   \|  | `---|  |----`|  |_)  |    |  |__   |  |  __     /  ^  \    |  |_)  | |  |     |  |__       | | 
|   __|  |  . `  |     |  |     |      /     |   __|  |  | |_ |   /  /_\  \   |   _  <  |  |     |   __|      | | 
|  |____ |  |\   |     |  |     |  |\  \----.|  |____ |  |__| |  /  _____  \  |  |_)  | |  `----.|  |____     | | 
|_______||__| \__|     |__|     | _| `._____||_______| \______| /__/     \__\ |______/  |_______||_______|    |_| 
                                                                                                                  
Aldán Creo Mariño, SOI 2020/21
*/

int contador_sigint = 0; // Esta variable es el contador global del cuántas veces hemos recibido SIGINT. Tiene que ser global, porque no hay conexión entre el main y el gestor_sigint

static void gestor_sigint(int signal); // La función que gestiona las interrupciones

int main()
{
    if (signal(SIGINT, gestor_sigint) == SIG_ERR)
    {
        perror("Error al asignar la señal. Abortando.");
        exit(EXIT_FAILURE);
    }
    for(;;); // Bucle infinito
}

static void gestor_sigint(int signal_id)
{
    char opcion; // Para guardar la opción
    if (contador_sigint >= 4) // Si ya hemos recibido SIGINT 4 veces
    {
        printf("Deseas volver a la gestión por defecto de SIGINT? (" ANSI_COLOR_GREEN "s" ANSI_COLOR_RESET "/" ANSI_COLOR_RED "N" ANSI_COLOR_RESET "): ");
        scanf(" %c", &opcion);
        if (opcion == 's' || opcion == 'S')
        {
            if (signal(SIGINT, SIG_DFL) == SIG_ERR) // Volvemos a la gestióin por defecto de SIGINT (interrumpimos el proceso)
            {
                perror("Error al asignar la señal. Abortando.");
                exit(EXIT_FAILURE);
            }
            printf(ANSI_COLOR_GREEN "Se ha vuelto a la gestión por defecto.\n" ANSI_COLOR_RESET);
        } else { // Cualquier opción que no sea S
            printf("Elegido: " ANSI_COLOR_RED "%c" ANSI_COLOR_RESET ". Se mantiene el comportamiento actual.\n", opcion); // No volvemos a la gestión por defecto de SIGINT, imprimimos el carácter de la opción seleccionada
        }
    }
    else
    {
        psignal(signal_id, "Se ha recibido la señal"); // Las 4 primeras veces, imprimimos la señal que hemos recibido
        contador_sigint++;
    }
}
