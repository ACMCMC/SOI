#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid_hijo1, pid_hijo2;
    int stat_loc;

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
        if ((pid_hijo1 = fork()) == 0)
        { // Hijo 2
            printf("Soy el hijo 2. Quieres matar al hijo 1?\n");
            scanf("%*c"); // Esperamos a que el usuario escriba algo, nos sirve cualquier cosa
            printf("Matando al hijo 1...\n");
            if (kill(pid_hijo1, SIGKILL))
            { // Matamos al primer hijo
                perror("Error ejecutando kill()");
                exit(EXIT_FAILURE);
            }
        }
        else if (pid_hijo1 < 0)
        {
            perror("Hubo un error en fork(). Abortamos");
            exit(EXIT_FAILURE);
        }
        else
        {                                     // Este proceso es el padre
            waitpid(pid_hijo1, &stat_loc, 0); // Esperamos a que acabe el hijo 1
            waitpid(pid_hijo2, &stat_loc, 0); // Podríamos usar también un wait(), ya que en este punto del código solo va a haber un proceso hijo (el hijo 2)
        }
    }

    printf("El %s termina.\n", getpid() == pid_hijo1 ? "hijo 1" : getpid() == pid_hijo2 ? "hijo 2" : "padre");

    return (EXIT_SUCCESS);
}