#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

#define N 5
#define M 3

// Los procesos zombies se pueden generar fácilmente haciendo que mueran antes que el padre.
// Los procesos huérfanos se pueden generar haciendo que sigan vivos después de que muera su padre.

// Una forma de resolver esta cuestión es creando N procesos hijo, y que el primero de ellos genere M procesos hijo. Si N muere mientras M sigue vivo, entonces N será zombie y M huérfano.

// Sobre las preguntas planteadas:

// ¿Pueden coexistir al mismo tiempo?
// Sí pueden, se comprueba en este código.

// ¿Los huéfanos pueden llegar a ser zombies?
// Depende de cómo lo consideremos. Un proceso huérfano realmente sí que tiene padre, que es el proceso init, ya que todo proceso siempre tiene un padre. Cuando un proceso adoptado por el proceso init muere, sí se vuelve zombie, hasta que el proceso init lee su información de retorno (lo hace automáticamente). Pero no se vuelve zombie del que era su padre muerto, sino del proceso init. Así que si lo consideramos así, sí pueden ser zombies (de init). Pero no pueden ser zombies del padre original. En este código, los M procesos se vuelven zombies al morir (tras haber sido adoptados por init). Su nuevo padre será el que tiene la responsabilidad de leer la información de retorno de los procesos.

// ¿Y los zombies pueden llegar a ser huérfanos?
// No, no pueden, porque un proceso zombie nunca puede cambiar de estado, ya que ha muerto. Lo que sucede si el padre de un proceso zombie muere es que lo adopta el proceso init, para leer su información de retorno y borrar su entrada de la tabla de procesos, pero no podemos decir que sea un "huérfano" como tal, porque un huérfano es un proceso en ejecución cuyo padre ha muerto (y por eso lo ha adoptado init).

int main()
{
    int contN;
    int contM;
    pid_t pid;

    for (contN = 0; contN < N; contN++)
    {
        if (contN == 0)
        {
            pid = fork();
            if (pid < 0)
            {
                perror("Error en fork()");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            { // Este es el hijo
                for (contM = 0; contM < M; contM++)
                {
                    pid = fork();
                    if (pid < 0)
                    {
                        perror("Error en fork()");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid == 0)
                    { // Este es el hijo
                        pid_t pid_inicial = getppid();
                        sleep(10);
                        printf("Soy un proceso huérfano, antes mi padre era %d, pero ahora mi padre es el proceso init, con PID %d\n", pid_inicial, getppid());
                        exit(EXIT_SUCCESS); // Se convierte en huérfano, porque muere después que el padre
                    }
                }

                sleep(5);
                printf("Me voy a volver zombie, mis hijos tenían mi PID (%d), pero ahora los va a adoptar el proceso init.\n", getpid());
                exit(EXIT_SUCCESS); // Se convierte en zombie tras 5 segundos
            }
        }
        else
        {
            pid = fork();
            if (pid < 0)
            {
                perror("Error en fork()");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            { // Este es el hijo
                sleep(5);
                printf("Me voy a volver zombie\n");
                exit(EXIT_SUCCESS); // Se convierte en zombie tras 5 segundos
            }
        }
    }

    sleep(15); // El padre tiene que vivir más que los hijos para que se conviertan en zombies
    printf("Soy el padre de los N procesos (zombies) y el abuelo de los M procesos (huérfanos). Voy a morir ahora.\n");
    exit(EXIT_SUCCESS);
}