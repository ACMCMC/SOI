#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Ver información de los hilos con ps -M

void* esperar(void* flagEsperar) {
    while (*((char*)flagEsperar)) {};
}

void* crear_hijo(void* args) {
    pid_t pid;
    int stat_loc;

    printf("Voy a hacer fork()\n");

    pid = fork();
    if (pid==0) { // Es el hijo.
        printf("Soy el hijo, mi PID es %d\n", getpid());
        sleep(15);
    } else if (pid > 0) { // Es el padre. Asumimos que fork() no ha fallado.
        waitpid(pid, &stat_loc, 0);
        printf("El hijo salió con stat_loc: %d\n", WEXITSTATUS(stat_loc));
    }
}

int main() {
    pthread_t id1, id2, id3;

    int flagEsperar = 1;

    printf("Soy el padre, mi PID es %d\n", getpid());

    pthread_create(&id1, NULL, esperar, (void*) &flagEsperar);
    pthread_create(&id2, NULL, esperar, (void*) &flagEsperar);
    pthread_create(&id3, NULL, crear_hijo, NULL);

    pthread_join(id3, NULL);
    flagEsperar = 0;
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    exit(EXIT_SUCCESS);
}