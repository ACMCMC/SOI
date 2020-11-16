#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/*
 _______ .__   __. .___________..______       _______   _______      ___      .______    __       _______     ___   
|   ____||  \ |  | |           ||   _  \     |   ____| /  _____|    /   \     |   _  \  |  |     |   ____|   |__ \  
|  |__   |   \|  | `---|  |----`|  |_)  |    |  |__   |  |  __     /  ^  \    |  |_)  | |  |     |  |__         ) | 
|   __|  |  . `  |     |  |     |      /     |   __|  |  | |_ |   /  /_\  \   |   _  <  |  |     |   __|       / /  
|  |____ |  |\   |     |  |     |  |\  \----.|  |____ |  |__| |  /  _____  \  |  |_)  | |  `----.|  |____     / /_  
|_______||__| \__|     |__|     | _| `._____||_______| \______| /__/     \__\ |______/  |_______||_______|   |____| 
                                                                                                                    
Aldán Creo Mariño, SOI 2020/21
*/

pid_t pid1, pid2, pidpadre; // Las usaremos para guardar los PIDs de los procesos para imprimir sus códigos
struct timespec tinicio;    // Guardaremos el tiempo de inicio del proceso padre

static void gestor(int numSenal); // El gestor de las señales SIGUSR1 y SIGUSR2

static void imprimirCabeceraLinea() // Función auxiliar para invocar al imprimir una línea. Indica el proceso actual y el tiempo transcurrido.
{
    struct timespec tactual;
    clock_gettime(CLOCK_MONOTONIC, &tactual); // Obtenemos el tiempo actual
    printf("| " ANSI_COLOR_BLUE "%-5s" ANSI_COLOR_RESET " | " ANSI_COLOR_CYAN "%9.6f" ANSI_COLOR_RESET " | ", pidpadre == getpid() ? "padre" : pid1 == 0 ? "hijo1" : "hijo2", ((double)tactual.tv_sec + 1.0e-9 * tactual.tv_nsec) - ((double)tinicio.tv_sec + 1.0e-9 * tinicio.tv_nsec));
}

int main()
{
    struct sigaction newAction; // Struct para indicarle a sigaction qué hacer

    int stat_loc;
    sigset_t set_sig_sigusr1, sig_pend;

    clock_gettime(CLOCK_MONOTONIC, &tinicio); // Guardamos el tiempo de inicio del programa

    pidpadre = getpid(); // Establecemos la variable global del PID del padre

    printf("| " ANSI_COLOR_BLUE "pr_id" ANSI_COLOR_RESET " |    " ANSI_COLOR_CYAN "tiempo" ANSI_COLOR_RESET " |\n");
    printf("--------------------\n");
    imprimirCabeceraLinea();
    printf("Entramos al proceso.\n");

    newAction.sa_handler = SIG_IGN;  // Vamos a ignorar SIGINT
    sigemptyset(&newAction.sa_mask); // El mask de señales que vamos a bloquear mientras estamos en la función de gestión. Es vacío porque no nos interesa bloquear ninguna señal en especial.
    newAction.sa_flags = SA_RESTART; // Si hay una llamada al sistema o estamos en una función de la librería estándar cuando se recibe una interrupción, procesarla y reiniciar la llamada al sistema/función. Si no se especifica, la función que se interrumpiera saldría con código de error.

    if (sigaction(SIGINT, &newAction, NULL)) // Ignoramos SIGINT
    {
        perror("Error en signal");
        exit(EXIT_FAILURE);
    }

    newAction.sa_handler = gestor; // SIGUSR1 y SIGUSR2 invocarán a la función gestor
    if (sigaction(SIGUSR1, &newAction, NULL))
    {
        perror("Error en signal");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR2, &newAction, NULL))
    {
        perror("Error en signal");
        exit(EXIT_FAILURE);
    }

    // Vamos a BLOQUEAR SIGUSR1
    sigemptyset(&set_sig_sigusr1);                    // Ponemos el set de señales que vamos a bloquear a vacío
    sigaddset(&set_sig_sigusr1, SIGUSR1);             // Añadimos SIGUSR1 a ese set de señales que vamos a bloquear
    sigprocmask(SIG_SETMASK, &set_sig_sigusr1, NULL); // Tenemos que bloquear la señal antes de hacer el fork() si queremos estar 100% seguros de que va a estar bloqueada cuando el hijo se la mande al padre. Podemos hacerlo, porque en los hijos no vamos a recibir ninguna SIGUSR1

    imprimirCabeceraLinea();
    printf("Se ha bloqueado " ANSI_COLOR_MAGENTA "SIGUSR1" ANSI_COLOR_RESET ".\n");

    pid1 = fork(); // Creamos el primer hijo
    if (pid1 < 0)
    {
        perror("Error en fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    { // Hijo 1
        // Aunque se resetean los tiempos de ejecución con fork(), como aquí estamos usando clock_gettime(), esta es una llamada al sistema que nos devuelve un valor global del sistema
        imprimirCabeceraLinea();
        printf("Entramos al proceso.\n");
        if (kill(getppid(), SIGUSR1))
        {
            perror("Hubo un error en kill()");
            exit(EXIT_FAILURE);
        }
        sleep(10); // No comprobamos si dio error o no, porque no nos interesa manejar aquí un error del sleep(). Aun así, como antes especificamos que si hay una interrupción, las llamadas al sistema se reinicien, no debería haber problema.
        if (kill(getppid(), SIGUSR2))
        {
            perror("Hubo un error en kill()");
            exit(EXIT_FAILURE);
        }
        sleep(5); // No comprobamos si dio error o no, porque no nos interesa manejar aquí un error del sleep(). Aun así, como antes especificamos que si hay una interrupción, las llamadas al sistema se reinicien, no debería haber problema.
    }
    else
    { // Padre

        pid2 = fork();
        if (pid2 < 0)
        {
            perror("Error en fork()");
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0)
        { // Hijo 2
            imprimirCabeceraLinea();
            printf("Entramos al proceso.\n");
            if (kill(getppid(), SIGINT)) // Mandamos SIGINT al padre, va a ser ignorada
            {
                perror("Hubo un error en kill()");
                exit(EXIT_FAILURE);
            }
            imprimirCabeceraLinea();
            printf("He enviado al padre " ANSI_COLOR_MAGENTA "SIGINT" ANSI_COLOR_RESET ".\n");
        }
        else
        {              // Padre
            sleep(11); // Esperamos 11 segundos, esto debería darle tiempo al proceso a esperar a SIGUSR1. Podríamos usar un pause(), pero haría falta bloquear también SIGCHLD porque el proceso 2 va a acabar, y entonces se produciría esa señal, que despertaría a este proceso. Cuando recibamos SIGUSR2, el sleep() se interrumpirá, y esto deberá pasar a los 10 segundos.

            // Nos ha llegado una señal, como tenemos SIGUSR1 bloqueada no puede ser ella. Comprobamos que está bloqueada:
            sigpending(&sig_pend);
            if (sigismember(&sig_pend, SIGUSR1)) // Está SIGUSR1 en el set de las señales pendientes?
            {
                imprimirCabeceraLinea();
                printf("La señal " ANSI_COLOR_MAGENTA "SIGUSR1" ANSI_COLOR_RESET " está pendiente. Vamos a desbloquearla.\n" ANSI_COLOR_RESET); // Esto es lo que debería ocurrir
                sigprocmask(SIG_UNBLOCK, &set_sig_sigusr1, NULL);                                                                               // La misma señal que bloqueamos antes, la desbloqueamos ahora
            }
            else
            {
                imprimirCabeceraLinea();
                printf(ANSI_COLOR_RED "La señal SIGUSR1 no está pendiente.\n" ANSI_COLOR_RESET); // Esto no debería ocurrir si se deja que el programa siga su flujo habitual, pero podría llegar a suceder por factores externos
            }
            waitpid(pid1, &stat_loc, 0); // Esperamos a que acabe el proceso hijo 1
            if (WIFEXITED(stat_loc))
            {
                imprimirCabeceraLinea();
                printf("El " ANSI_COLOR_BLUE "hijo 1" ANSI_COLOR_RESET " salió con código " ANSI_COLOR_MAGENTA "%d" ANSI_COLOR_RESET ".\n", WEXITSTATUS(stat_loc));
            }
            else
            {
                fprintf(stderr, "(padre): El proceso hijo no salió de forma controlada.\n");
            }

            waitpid(pid2, &stat_loc, 0); // Comprobamos la salida del proceso hijo 2, que debería ser zombie en este punto
            if (WIFEXITED(stat_loc))
            {
                imprimirCabeceraLinea();
                printf("El " ANSI_COLOR_BLUE "hijo 2" ANSI_COLOR_RESET " salió con código " ANSI_COLOR_MAGENTA "%d" ANSI_COLOR_RESET ".\n", WEXITSTATUS(stat_loc));
            }
            else
            {
                fprintf(stderr, "(padre): El proceso hijo no salió de forma controlada.\n");
            }
        }
    }

    imprimirCabeceraLinea(); // Los hijos y el padre dicen que van a salir
    printf("Voy a salir con EXIT_SUCCESS.\n");

    exit(EXIT_SUCCESS);
}

static void gestor(int numSenal) // Esta función gestiona las señales SIGUSR1 y SIGUSR2
{
    imprimirCabeceraLinea();
    printf("Se ha recibido la señal " ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET "\n", strsignal(numSenal));
}
