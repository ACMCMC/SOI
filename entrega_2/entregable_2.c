#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    pid_t pid;
    int stat_loc; // Información sobre la salida del hijo, se guarda todo en un int
    int exit_status; // El código de salida del hijo, se obtiene a partir de stat_loc

    pid = fork();

    if (((int)pid) == -1) {
        perror("Error ejecutando fork(). Abortamos.");
        exit(EXIT_FAILURE);
    } else if (((int)pid) == 0) { // Este proceso es el primer hijo. Va a acabar casi al instante, porque no hace más que un printf
        printf("HIJO (%d)\n", (int) getpid());
        exit(10); // Código arbitrario para la salida. Debe poder leerlo el padre.
    }

    pid = fork();

    if (((int)pid) == -1) {
        perror("Error ejecutando fork(). Abortamos.");
        exit(EXIT_FAILURE);
    } else if (((int)pid) == 0) { // Este proceso es el segundo hijo. Va a acabar casi al instante, porque no hace más que un printf
        printf("HIJO (%d)\n", (int) getpid());
        sleep(20);
        exit(20); // Código arbitrario para la salida. Debe poder leerlo el padre.
    } else {
        printf("PADRE (%d)\n", (int) getpid());
        sleep(1); // El padre hace un sleep, y en ese tiempo el primer hijo se vuelve un proceso zombie
        waitpid(pid, &stat_loc, 0); // Esperar por el hijo a que acabe, guarda en stat_loc la información sobre su salida
        exit_status = WEXITSTATUS(stat_loc); // Obtenemos el código de salida del hijo
        printf("Estado de salida del hijo: %d\n", exit_status);
    }

    printf("El %s sale.\n", ((int)pid) == 0 ? "hijo" : "padre");

    exit(EXIT_SUCCESS);
}
