#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define EXIT_HIJO1 10
#define EXIT_HIJO2 20

int main()
{
    pid_t pid1, pid2;
    int stat_loc;    // Información sobre la salida del hijo, se guarda todo en un int
    int exit_status; // El código de salida del hijo, se obtiene a partir de stat_loc
    
    printf("PADRE (%d)\n", (int)getpid());

    pid1 = fork();

    if (((int)pid1) == -1)
    {
        perror("Error ejecutando fork(). Abortamos.");
        exit(EXIT_FAILURE);
    }
    else if (((int)pid1) == 0)
    { // Este proceso es el primer hijo. Va a acabar casi al instante, porque no hace más que un printf
        printf("HIJO 1 (%d)\n", (int)getpid());
        exit(EXIT_HIJO1); // Código arbitrario para la salida. Debe poder leerlo el padre.
    }
    else
    { // Este es el proceso padre

        sleep(10);                            // El padre hace un sleep, y en ese tiempo el primer hijo se vuelve un proceso zombie
        pid2 = fork(); // Creamos un nuevo proceso

        if (((int)pid2) == -1)
        {
            perror("Error ejecutando fork(). Abortamos.");
            exit(EXIT_FAILURE);
        }
        else if (((int)pid2) == 0)
        { // Este proceso es el segundo hijo. Va a tardar 10 segundos en acabar.
            printf("HIJO 2 (%d)\n", (int)getpid());
            sleep(10);

            if (execlp("echo", "echo", "prueba del comando echo", (char*) NULL) == -1 ) { // Probamos a ejecutar otro comando. Debería imprimirse "prueba del comando echo" por consola
                perror("Error ejecutando execlp");
            }

            printf("Este texto no se debería imprimir\n"); // No se debería imprimir porque hemos cambiado el ejecutable

            exit(EXIT_HIJO2); // Código arbitrario para la salida. Debe poder leerlo el padre.
        }
        else
        {
            wait(&stat_loc);                     // Esperar por el hijo a que acabe, guarda en stat_loc la información sobre su salida. La información que obtendremos será del primer hijo, porque el segundo aún estará en el sleep(10) de arriba
            exit_status = WEXITSTATUS(stat_loc); // Obtenemos el código de salida del hijo
            if (exit_status == EXIT_HIJO1) // Sabemos qué hijo es el que sale por el código de salida. También podríamos saberlo usando el resultado del wait(), que nos da el PID
            {
                printf("Se ha obtenido el código de salida esperado del primer hijo (%d)\n", exit_status);
            }
            else if (exit_status == EXIT_HIJO2)
            {
                printf("Se ha obtenido el código de salida esperado del segundo hijo (%d)\n", exit_status);
            }
            else
            {
                printf("Se ha obtenido un código de salida inesperado (%d)\n", exit_status);
            }

            printf("El padre sale\n");
        }
    }

    exit(EXIT_SUCCESS);
}
