#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

#define DESPL_ARCHIVO 4

int main() {
    int fd;
    struct stat info;

    // Abro fichero.txt para lectura y escritura con la función open
    fd = open("./fichero.txt", O_RDWR);
    if (fd < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    if (fstat(fd, &info)) { // Usamos fstat para obtener la longitud del fichero
        perror("Error en fstat()");
        exit(EXIT_FAILURE);
    }

    /*
     * Proyecto el fichero en memoria con permisos de lectura y escritura y área de memoria compartida
     */
    void* ubicacion_mapeo;
    ubicacion_mapeo = mmap(0, info.st_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (ubicacion_mapeo==MAP_FAILED) {
        perror("Error en mmap()");
        exit(EXIT_FAILURE);
    }

    if (flock(fd, LOCK_EX)) { // Bloqueamos el fichero. Esto le indicará al otro proceso que debe esperar.
        perror("Error en flock()");
        exit(EXIT_FAILURE);
    }

    printf("Escribe un número cualquiera para proceder.\n"); // Esperamos al usuario
    scanf("%*d");

    // "El segundo proceso añade los 3 asteriscos usando memset"
    memset(((char*) ubicacion_mapeo) + 2, '*', sizeof(char)*3); //Escribo *** en el archivo

    if (flock(fd, LOCK_UN)) { // Desbloqueamos el fichero
        perror("Error en flock()");
        exit(EXIT_FAILURE);
    }
    
    // Cierro la proyección con la función munmap
    if(munmap(ubicacion_mapeo, info.st_size)) {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }

    // Cierro el archivo
    if (close(fd)) {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
    fd = 0;
    
    exit(EXIT_SUCCESS);
}
