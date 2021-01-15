#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>

// Como los procesos no tienen ningún tipo de relación entre ellos, tendrán que usar un medio externo para comunicarse, sí o sí. La mejor opción, creo yo, es usar un archivo mapeado en común en ambos procesos.

void procesar_sigusr1() {
    printf("(%d): He recibido SIGUSR 1\n", getpid());
}

void hijo1() {
    int f1, f2;
    pid_t pid_propio, pid_hijo2;

    signal(SIGUSR1, procesar_sigusr1);

    pid_propio = getpid();
    printf("Soy el hijo 1. Mi PID es %d\n", pid_propio);

    if (mkfifo("f1", 0600) < 0) {
        perror("Error en mkfifo()");
        exit(EXIT_FAILURE);
    }
    if (mkfifo("f2", 0600) < 0) {
        perror("Error en mkfifo()");
        exit(EXIT_FAILURE);
    }

    f1 = open("f1", O_WRONLY, 0600);
    if (f1 < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }

    write(f1, &pid_propio, sizeof(pid_t));
    f2 = open("f2", O_RDONLY, 0600);
    if (f2 < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    read(f2, &pid_hijo2, sizeof(pid_t));

    kill(pid_hijo2, SIGUSR1);

    if (close(f1)) {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
    if (close(f2)) {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
    if (unlink("f1")) {
        perror("Error en unlink()");
        exit(EXIT_FAILURE);
    }
    if (unlink("f2")) {
        perror("Error en unlink()");
        exit(EXIT_FAILURE);
    }
}

void hijo2() {
    int f1, f2;
    pid_t pid_hijo1, pid_propio;

    signal(SIGUSR1, procesar_sigusr1);

    pid_propio = getpid();
    printf("Soy el hijo 2. Mi PID es %d\n", pid_propio);

    f1 = open("f1", O_RDONLY, 0600);
    if (f1 < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    read(f1, &pid_hijo1, sizeof(pid_t));

    f2 = open("f2", O_WRONLY, 0600);
    if (f2 < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    write(f2, &pid_propio, sizeof(pid_t));

    pause();

    kill(pid_hijo1, SIGUSR1);

    if (close(f1)) {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
    if (close(f2)) {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
}

int main() {
    if (access("f1", F_OK)==0) {
        hijo2();
    } else {
        hijo1();
    }

    exit(EXIT_SUCCESS);
}