#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define DESPL_ARCHIVO 4

int main()
{
    int fd;
    struct stat info;
    char *cadena_texto; // Aquí guardaré la cadena de texto con la que voy a trabajar
    struct flock res_fcntl; // Lo usaré para controlar la sincronización
    int esperando_por_proceso2; // Flags de control
    int prompt_al_usuario_no_impreso = 1;

    // Abro fichero.txt para lectura y escritura con la función open
    fd = open("./fichero.txt", O_RDWR);
    if (fd < 0)
    {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    if (fstat(fd, &info))
    {
        perror("Error en fstat()");
        exit(EXIT_FAILURE);
    }

    /*
     * Proyecto el fichero en memoria con permisos de lectura y escritura y área de memoria compartida
     */
    void *ubicacion_mapeo;
    ubicacion_mapeo = mmap(0, info.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ubicacion_mapeo == MAP_FAILED)
    {
        perror("Error en mmap()");
        exit(EXIT_FAILURE);
    }

    res_fcntl.l_type = F_WRLCK;

    do
    {
        fcntl(fd, F_GETLK, &res_fcntl);
        esperando_por_proceso2 = res_fcntl.l_type != F_WRLCK;
        if (esperando_por_proceso2 && prompt_al_usuario_no_impreso)
        {
            printf("Inicie el proceso 2 para continuar...\n");
            prompt_al_usuario_no_impreso = 0;
        }
    } while (esperando_por_proceso2);

    if (flock(fd, LOCK_EX))
    { // Este proceso se quedará esperando a que el otro libere el bloqueo sobre el fichero.
        perror("Error en flock()");
        exit(EXIT_FAILURE);
    }

    cadena_texto = (char *)malloc(sizeof(char) * info.st_size); // Reservamos memoria para las modificaciones. El tamaño de un char siempre es 1 byte.
    memcpy((void *)cadena_texto, (void *)ubicacion_mapeo, sizeof(char) * 5);
    memcpy((void *)(cadena_texto + 5), "", sizeof(char));  // Añado un '\0' a la cadena
    strncat(cadena_texto, "2345", sizeof(char) * 4);       // Añado 2345, y se va a copiar también un '\0'
    cadena_texto = (char*) realloc((void*) cadena_texto, sizeof(char)*(strlen(cadena_texto)+1+4));
    strncat(cadena_texto, (char *)ubicacion_mapeo + 5, 4); // Copio los siguientes 4 caracteres desde la proyección

    ftruncate(fd, info.st_size + 2); // Expandimos en 2 el tamaño del fichero

    // Cierra la proyección con la función munmap
    if (munmap(ubicacion_mapeo, info.st_size))
    {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }

    ubicacion_mapeo = mmap(0, info.st_size + 2, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ubicacion_mapeo == MAP_FAILED)
    {
        perror("Error en mmap()");
        exit(EXIT_FAILURE);
    }

    memcpy(ubicacion_mapeo, (void *)cadena_texto, sizeof(char) * (strlen(cadena_texto))); // Copio la cadena de vuelta, sin el '\0'

    if (flock(fd, LOCK_UN))
    { // Desbloqueamos el fichero
        perror("Error en flock()");
        exit(EXIT_FAILURE);
    }

    // Cierra la proyección con la función munmap
    if (munmap(ubicacion_mapeo, info.st_size))
    {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }

    // Cierro el archivo
    if (close(fd))
    {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
    fd = 0;

    free(cadena_texto);
    exit(EXIT_SUCCESS);
}
