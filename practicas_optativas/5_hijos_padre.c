#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

// Podemos usar la llamada al sistema pipe() para averiguar el PID del hijo 2, desde el hijo 1. EL hijo 1 intenta leer del pipe, y esa llamada se bloqueará hasta que el padre escriba algo en el pipe. El padre, cuando cree el hijo 2, escribirá su PID en el pipe, permitiéndoselo leer al hijo 1.

void hijo1(int p_read)
{
    pid_t pid;
    printf("Hijo 1: PID %d\n", getpid());
    read(p_read, &pid, sizeof(pid_t));
    kill(pid, SIGUSR1);
}

void recibir_senal(int pid)
{
    printf("Soy el hijo 2 (%d). He recibido SIGUSR1 de %d\n", getpid(), pid);
}

void hijo2()
{
    printf("Hijo 2: PID %d\n", getpid());
    signal(SIGUSR1, recibir_senal);
    sleep(5);
}

int main()
{
    pid_t pid;
    int p[2];

    printf("Padre: PID %d\n", getpid());

    if (pipe(p) < 0)
    {
        perror("Error en pipe()");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("Error en fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // Es el hijo 1
        hijo1(p[0]);
        printf("Soy el hijo 1. Voy a morir ahora.\n");
    }
    else // Es el padre
    {
        pid = fork();
        if (pid < 0)
        {
            perror("Error en fork()");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        { // Es el hijo 2
            hijo2();
            printf("Soy el hijo 2. Voy a morir ahora.\n");
        }
        else
        {
            write(p[1], &pid, sizeof(pid_t));

            sleep(2);

            printf("Soy el padre. Voy a morir ahora.\n");
        }
    }

    exit(EXIT_SUCCESS);
}