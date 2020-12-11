#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Crea un proceso hijo que incluya un malloc y a continuación cambie su imagen con execv a un código cualquiera creado por ti.
 * Compara los mapas de memoria del proceso padre y el proceso hijo antes y después del malloc y comprueba como el cambio de
 * imagen afecta al mapa de memoria del proceso hijo tras el execv.
 * 
 * Aldán Creo Mariño, SOI 2020/21
 */

int main()
{
    pid_t hijo;
    void *p;

    char arg1[] = "a.out";
    char *args[2] = {arg1, NULL};

    hijo = fork();

    if (hijo < 0)
    {
        perror("Error en fork()");
        exit(EXIT_FAILURE);
    }

    if (hijo == 0)
    { // Este es el padre
    printf("Soy el padre\n");
    getchar();
    }
    else
    { // Este es el hijo
        getchar();
        p = malloc(1024 * 1024 * 1024);
        printf("Resultado malloc: %p\n", p);
        getchar();
        execv("./a.out", args);
    }

    exit(EXIT_SUCCESS);
}