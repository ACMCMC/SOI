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

pid_t pid1, pid2, pidpadre; // Las usaremos para guardar los PIDs de los procesos para imprimir sus códigos
struct timespec tinicio;

static void gestor(int numSenal);

static void imprimirCabeceraLinea()
{
    struct timespec tactual;
    clock_gettime(CLOCK_MONOTONIC, &tactual); // Obtenemos el tiempo actual
    printf("| " ANSI_COLOR_BLUE "%-5s" ANSI_COLOR_RESET " | " ANSI_COLOR_CYAN "%9.6f" ANSI_COLOR_RESET " | ", pidpadre == getpid() ? "padre" : pid1 == 0 ? "hijo1" : "hijo2", ((double)tactual.tv_sec + 1.0e-9 * tactual.tv_nsec) - ((double)tinicio.tv_sec + 1.0e-9 * tinicio.tv_nsec));
}

int main()
{
    struct sigaction newAction;
    newAction.sa_handler = SIG_IGN;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = SA_RESTART;
    int stat_loc;
    sigset_t set_sig_sigusr1, sig_pend;

    clock_gettime(CLOCK_MONOTONIC, &tinicio); // Guardamos el tiempo de inicio del programa

    pidpadre = getpid();

    printf("| " ANSI_COLOR_BLUE "pr_id" ANSI_COLOR_RESET " |    " ANSI_COLOR_CYAN "tiempo" ANSI_COLOR_RESET " |\n");
    printf("--------------------\n");
    imprimirCabeceraLinea();
    printf("Entramos al proceso.\n");

    sigemptyset(&set_sig_sigusr1);
    sigaddset(&set_sig_sigusr1, SIGUSR1);

    if (sigaction(SIGINT, &newAction, NULL))
    {
        perror("Error en signal");
        exit(EXIT_FAILURE);
    }
    newAction.sa_handler = gestor;
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

    sigprocmask(SIG_SETMASK, &set_sig_sigusr1, NULL); // Tenemos que bloquear la señal antes de hacer el fork() si queremos estar 100% seguros de que va a estar bloqueada cuando el hijo se la mande al padre. Podemos hacerlo, porque en los hijos no vamos a recibir ninguna SIGUSR1
    imprimirCabeceraLinea();
    printf("Se ha bloqueado " ANSI_COLOR_MAGENTA "SIGUSR1" ANSI_COLOR_RESET ".\n");

    pid1 = fork();
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
        sleep(10); // No comprobamos si dio error o no, porque no nos interesa manejar aquí un error del sleep()
        if (kill(getppid(), SIGUSR2))
        {
            perror("Hubo un error en kill()");
            exit(EXIT_FAILURE);
        }
        sleep(5); // No comprobamos si dio error o no, porque no nos interesa manejar aquí un error del sleep()
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
            if (kill(getppid(), SIGINT))
            {
                perror("Hubo un error en kill()");
                exit(EXIT_FAILURE);
            }
            sleep(1);
            imprimirCabeceraLinea();
            printf("He enviado al padre " ANSI_COLOR_MAGENTA "SIGINT" ANSI_COLOR_RESET ".\n");
        }
        else
        {              // Padre
            sleep(11); // Esperamos 11 segundos, esto debería darle tiempo al proceso a esperar a SIGUSR1. Podríamos usar un pause(), pero haría falta bloquear también SIGCHLD porque el proceso 2 va a acabar, y entonces se produciría esa señal, que despertaría a este proceso. Cuando recibamos SIGUSR2, el sleep() se interrumpirá, y esto deberá pasar a los 10 segundos.

            // Nos ha llegado una señal, como tenemos SIGUSR1 bloqueada no puede ser ella. Comprobamos que está bloqueada:
            sigpending(&sig_pend);
            if (sigismember(&sig_pend, SIGUSR1))
            {
                imprimirCabeceraLinea();
                printf("La señal " ANSI_COLOR_MAGENTA "SIGUSR1" ANSI_COLOR_RESET " está pendiente. Vamos a desbloquearla.\n" ANSI_COLOR_RESET); // Esto es lo que debería ocurrir
                sigprocmask(SIG_UNBLOCK, &set_sig_sigusr1, NULL);                                                                               // La misma señal que bloqueamos antes, la desbloqueamos ahora
            }
            else
            {
                imprimirCabeceraLinea();
                printf(ANSI_COLOR_RED "La señal SIGUSR1 no está pendiente.\n" ANSI_COLOR_RESET); // Esto no debería ocurrir si se deja que el programa siga su flujo habitual, pero podría suceder si
            }
            waitpid(pid1, &stat_loc, 0);
            if (WIFEXITED(stat_loc))
            {
                imprimirCabeceraLinea();
                printf("El " ANSI_COLOR_BLUE "hijo 1" ANSI_COLOR_RESET " salió con código " ANSI_COLOR_MAGENTA "%d" ANSI_COLOR_RESET ".\n", WEXITSTATUS(stat_loc));
            }
            else
            {
                fprintf(stderr, "(padre): El proceso hijo no salió de forma controlada.\n");
            }

            waitpid(pid2, &stat_loc, 0);
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

    imprimirCabeceraLinea();
    printf("Voy a salir con EXIT_SUCCESS.\n");

    exit(EXIT_SUCCESS);
}

static void gestor(int numSenal)
{
    imprimirCabeceraLinea();
    printf("Se ha recibido la señal " ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET "\n", strsignal(numSenal));
}
