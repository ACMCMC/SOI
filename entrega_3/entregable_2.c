#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

static void gestor(int numSenal);

int main()
{
    struct sigaction newAction;
    newAction.sa_handler = SIG_IGN;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    pid_t pid1, pid2;
    int stat_loc;

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
            perror("Hubo un error en kill(). Abortamos.");
            exit(EXIT_FAILURE);
        }
        sleep(10); // No comprobamos si dio error o no, porque no nos interesa manejar aquí un error del sleep()
        if (kill(getppid(), SIGUSR2))
        {
            perror("Hubo un error en kill(). Abortamos.");
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
        }
        else
        { // Padre
            waitpid(pid1, &stat_loc, 0);
            if (WIFEXITED(stat_loc))
            {
                printf("El hijo 1 salió con código %d.\n", WEXITSTATUS(stat_loc));
            }
            else
            {
                fprintf(stderr, "El proceso hijo no salió de forma controlada.\n");
            }
        }
    }
    exit(EXIT_SUCCESS);
}

static void gestor(int numSenal)
{
    printf("Se ha recibido la señal %s\n", strsignal(numSenal));
}
