#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    pid_t pid_hijo1, pid_hijo2;
    int stat_loc;

    printf("PID del padre: %d\n", getpid());

    if ((pid_hijo1 = fork()) == 0)
    { // Hijo 1
        while (1)
        {
        };
    }
    else if (pid_hijo1 < 0)
    {
        perror("Hubo un error en fork(). Abortamos.");
        exit(EXIT_FAILURE);
    }
    else
    { // Este proceso es el padre
        if ((pid_hijo2 = fork()) == 0)
        { // Hijo 2
            printf("Soy el hijo 2. Pulsa cualquier tecla para matar al hijo 1.\n");
            scanf("%*c"); // Esperamos a que el usuario escriba algo, nos sirve cualquier cosa
            printf("Matando al hijo 1...\n");
            if (kill(pid_hijo1, SIGKILL))
            { // Matamos al primer hijo
                perror("Error ejecutando kill()");
                exit(EXIT_FAILURE);
            }
        }
        else if (pid_hijo2 < 0)
        {
            perror("Hubo un error en fork(). Abortamos");
            exit(EXIT_FAILURE);
        }
        else
        {                                     // Este proceso es el padre
            waitpid(pid_hijo1, &stat_loc, 0); // Esperamos a que acabe el hijo 1
            if (WIFSIGNALED(stat_loc) && WTERMSIG(stat_loc) == SIGKILL)
            {
                printf("Soy el padre, y puedo ver que el hijo 1 se ha matado adecuadamente.\n");
            }
            else if (WIFSIGNALED(stat_loc)) // El hijo 1 no acabó con la señal que esperábamos (SIGKILL), sino que acabó con otra
            {
                fprintf(stderr, "El hijo 1 no ha salido con la señal esperada, sino que ha salido con: %s.\n", strsignal(WTERMSIG(stat_loc)));
            }
            else
            { // Error genérico
                fprintf(stderr, "Se ha obtenido un valor de retorno inesperado del hijo 1.\n");
            }
            waitpid(pid_hijo2, &stat_loc, 0); // Podríamos usar también un wait(), ya que en este punto del código solo va a haber un proceso hijo (el hijo 2)
            if (WIFEXITED(stat_loc) && WEXITSTATUS(stat_loc) == EXIT_SUCCESS)
            { // Todo fue bien en el hijo 2
                printf("El hijo 2 ha acabado correctamente.\n");
            }
            else if (WIFEXITED(stat_loc))
            { // Si el hijo 2 salió con un código distinto de EXIT_SUCCESS, no va a pasar porque no está en su código pero es buena práctica incluírlo
                fprintf(stderr, "El hijo 2 no ha salido con el estado esperado, sino que ha salido con: %d.\n", WEXITSTATUS(stat_loc));
            }
            else if (WIFSIGNALED(stat_loc))
            { // Podría pasar que el hijo 2 terminara por una señal, en ese caso también lo indicamos
                fprintf(stderr, "El hijo 2 no ha salido con el estado esperado, sino que ha salido con la senal: %s.\n", strsignal(stat_loc));
            }
            printf("El padre ha acabado correctamente.\n");
        }
    }

    return (EXIT_SUCCESS);
}