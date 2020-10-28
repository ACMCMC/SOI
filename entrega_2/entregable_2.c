#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define EXIT_HIJO1 10
#define EXIT_HIJO2 20

// Se puede usar ps -v -g (PID DEL PADRE) para ver información de los procesos

int main()
{
    pid_t pid1, pid2, pid_proc;
    int stat_loc;    // Información sobre la salida del hijo, se guarda todo en un int
    int exit_status; // El código de salida del hijo, se obtiene a partir de stat_loc

    if (printf("PADRE (%d)\n", (int)getpid()) < 0)
    {
        perror("Error ejecutando printf()");
        exit(EXIT_FAILURE);
    };

    pid1 = fork();

    if (((int)pid1) == -1)
    {
        perror("Error ejecutando fork(). Abortamos.");
        exit(EXIT_FAILURE);
    }
    else if (((int)pid1) == 0)
    { // Este proceso es el primer hijo. Va a acabar casi al instante, porque no hace más que un printf
        if (printf("HIJO 1 (%d)\n", (int)getpid()) < 0)
        {
            perror("Error ejecutando printf()");
            exit(EXIT_FAILURE);
        };
        exit(EXIT_HIJO1); // Código arbitrario para la salida. Debe poder leerlo el padre.
    }
    else
    { // Este es el proceso padre

        sleep(10);     // El padre hace un sleep, y en ese tiempo el primer hijo se vuelve un proceso zombie
        pid2 = fork(); // Creamos un nuevo proceso

        if (((int)pid2) == -1)
        {
            perror("Error ejecutando fork(). Abortamos.");
            exit(EXIT_FAILURE);
        }
        else if (((int)pid2) == 0)
        { // Este proceso es el segundo hijo. Va a tardar 10 segundos en acabar.
            if (printf("HIJO 2 (%d)\n", (int)getpid()) < 0)
            {
                perror("Error ejecutando printf()");
                exit(EXIT_FAILURE);
            }
            sleep(10);

            if (execlp("echo", "echo", "Prueba del comando echo", (char *)NULL) == -1)
            { // Probamos a ejecutar otro comando. Debería imprimirse "prueba del comando echo" por consola y este código dejaría de ejecutarse aquí, ya que hemos cambiado la imagen del proceso por la del echo
                perror("Error ejecutando execlp");
            }

            printf("Este texto no se debería imprimir\n"); // No se debería imprimir porque hemos cambiado el ejecutable

            exit(EXIT_HIJO2); // Código arbitrario para la salida. Debe poder leerlo el padre.
        }
        else
        {
            waitpid(pid1, &stat_loc, 0);         // Esperar por el primer hijo a que acabe, guarda en stat_loc la información sobre su salida. La información que obtendremos será del primer hijo, porque estamos usando waitpid
            exit_status = WEXITSTATUS(stat_loc); // Obtenemos el código de salida del hijo
            if (exit_status == EXIT_HIJO1)       // Sabemos qué hijo es el que sale por el código de salida. También podríamos saberlo usando el resultado del wait(), que nos da el PID
            {
                if (printf("Se ha obtenido el código de salida esperado del primer hijo (%d)\n", exit_status) < 0)
                {
                    perror("Error ejecutando printf()");
                    exit(EXIT_FAILURE);
                }
            }
            else if (exit_status == EXIT_HIJO2)
            {
                if (printf("Se ha obtenido el código de salida esperado del segundo hijo (%d)\n", exit_status) < 0)
                {
                    perror("Error ejecutando printf()");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (printf("Se ha obtenido un código de salida inesperado (%d)\n", exit_status) < 0)
                {
                    perror("Error ejecutando printf()");
                    exit(EXIT_FAILURE);
                }
            }

            pid_proc = wait(&stat_loc); // Esperamos a que acabe el segundo hijo (ya que el primero no puede acabar porque ya hemos esperado por él)
            exit_status = WEXITSTATUS(stat_loc);

            if (printf("El código de salida del hijo %s es %d\n", (pid_proc == pid1) ? "1" : (pid_proc == pid2) ? "2" : "DESCONOCIDO", exit_status) < 0)
            {
                perror("Error ejecutando printf()");
                exit(EXIT_FAILURE);
            } // Esto siempre debería imprimir el código de salida del hijo 2, que debería ser 0 porque es lo que devuelve el comando echo

            if (printf("El padre sale\n") < 0)
            {
                perror("Error ejecutando printf()");
                exit(EXIT_FAILURE);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
