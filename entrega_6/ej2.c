#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define DESPL_ARCHIVO 4

int main() {
    int fd;
    struct stat info;

    // Abre un archivo para lectura con la función open
    fd = open("./texto.txt", O_RDWR);
    if (fd < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    if (fstat(fd, &info)) {
        perror("Error en fstat()");
        exit(EXIT_FAILURE);
    }

    /*
     * Haz un programa que proyecte un archivo en memoria con permisos de lectura y escritura y área de memoria compartida.
     */
    void* ubicacion_mapeo;
    ubicacion_mapeo = mmap(0, info.st_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (ubicacion_mapeo==MAP_FAILED) {
        perror("Error en mmap()");
        exit(EXIT_FAILURE);
    }

    // Cierra el archivo
    if (close(fd)) {
        perror("Error en close()");
        exit(EXIT_FAILURE);
    }
    fd = 0;

    // Modifica el valor de alguno de los caracteres de la proyección del archivo.
    printf("Antes: %c\n", *(((char*) ubicacion_mapeo)+DESPL_ARCHIVO));
    *(((char*) ubicacion_mapeo)+DESPL_ARCHIVO) = 'X';
    printf("Después: %c\n", *(((char*) ubicacion_mapeo)+DESPL_ARCHIVO));

    // Comprueba si las modificaciones que se hacen en memoria se reflejan en el archivo.

    // (Sí se hacen, porque es memoria compartida)
    
    // Cierra la proyección con la función munmap
    if(munmap(ubicacion_mapeo, info.st_size)) {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }
}