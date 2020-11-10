#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

static void gestor(int num);

int main()
{
    pid_t pid_hijo;

    pid_hijo = fork();

    if (pid_hijo < 0)
    {
        perror("Hubo un error en fork(). Abortamos.");
        exit(EXIT_FAILURE);
    }
    else if (pid_hijo == 0)
    { // Este es el hijo
        printf("PID hijo: %d.\n", getpid());
        if (signal(SIGUSR1, gestor))
        { // Por defecto, SIGUSR1 se ignora. Tenemos que cambiar este comportamiento por defecto para que el proceso despierte en el pause
            perror("Hubo un error en signal(). Abortamos.");
            exit(EXIT_FAILURE);
        }
        pause(); // Siempre devuelve -1, porque "esperar para siempre" es fallido en el momento en que el programa continúa
        printf("Soy el hijo y he despertado.\n");
    }
    else
    { // Este es el padre
        printf("PID padre: %d.\n", getpid());
        sleep(5); // No la comprobamos porque aunque no se espere el tiempo especificado no vamos a abortar el programa
        if (kill(pid_hijo, SIGUSR1))
        {
            perror("Hubo un error en kill(). Abortamos.");
            exit(EXIT_FAILURE);
        }
        printf("Soy el padre y he despertado al hijo.\n");
    }
    exit(EXIT_SUCCESS);
}

static void gestor(int num)
{
    printf("SEÑAL: %s\n", strsignal(num));
}
