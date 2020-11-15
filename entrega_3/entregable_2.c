#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

pid_t pid1, pid2, pidpadre; // Las usaremos para guardar los PIDs de los procesos para imprimir sus códigos
clock_t clock_inicial;

static void gestor(int numSenal);

static void imprimirCabeceraLinea() {
    clock_t clock_actual;
    clock_actual = clock();
    printf("(" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET ", " ANSI_COLOR_CYAN "%ld" ANSI_COLOR_RESET "): ", pidpadre == getpid() ? "padre" : pid1 == getpid() ? "hijo 1" : "hijo 2", clock_actual - clock_inicial);
}

int main()
{
    struct sigaction newAction;
    newAction.sa_handler = SIG_IGN;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = SA_RESTART;
    int stat_loc;
    sigset_t set_sig_sigusr1, sig_pend;

    clock_inicial = clock(); // Guardamos el tiempo de entrada al programa

    sigemptyset(&set_sig_sigusr1);
    sigaddset(&set_sig_sigusr1, SIGUSR1);

    pidpadre = getpid();

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
    printf("Se ha bloqueado SIGUSR1\n");

    pid1 = fork();
    if (pid1 < 0)
    {
        perror("Error en fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    { // Hijo 1
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

        pause(); // Siempre devuelve -1, porque "esperar para siempre" es fallido en el momento en que el programa continúa

        // Nos ha llegado una señal, como tenemos SIGUSR1 bloqueada no puede ser ella. Comprobamos si está bloqueada:
        sigpending(&sig_pend);
        if (sigismember(&sig_pend, SIGUSR1)) {
    imprimirCabeceraLinea();
            printf(ANSI_COLOR_GREEN "La señal SIGUSR1 está pendiente. Vamos a desbloquearla.\n" ANSI_COLOR_RESET); // Esto es lo que debería ocurrir
            sigprocmask(SIG_UNBLOCK, &set_sig_sigusr1, NULL); // La misma señal que bloqueamos antes, la desbloqueamos ahora
        } else {
    imprimirCabeceraLinea();
            printf(ANSI_COLOR_RED "La señal SIGUSR1 no está pendiente.\n" ANSI_COLOR_RESET); // Esto no debería ocurrir si se deja que el programa siga su flujo habitual, pero podría suceder si 
        }

        pid2 = fork();
        if (pid2 < 0)
        {
            perror("Error en fork()");
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0)
        { // Hijo 2
        if (kill(getppid(), SIGINT))
        {
            perror("Hubo un error en kill()");
            exit(EXIT_FAILURE);
        }
        }
        else
        { // Padre
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
    exit(EXIT_SUCCESS);
}

static void gestor(int numSenal)
{
    imprimirCabeceraLinea();
    printf("Se ha recibido la señal " ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET "\n", strsignal(numSenal));
}
