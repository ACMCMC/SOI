#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

/*
 * Compara los mapas de memoria del proceso antes y después de hacer la proyección del archivo en memoria para determinar la zona del mapa de memoria donde se proyecta el archivo. Estudia las diferencias en el mapa de memoria del proceso entre establecer permisos de solo lectura o de lectura y escritura para la proyección, y entre definir el área de memoria como privada o compartida.
 * 
 * Básicamente, el fichero se proyecta en una ubicación de memoria aleatoria, con los permisos que hemos especificado.
 * 
 * 
 * 
 * 
 * 
 * 
 */

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
    
    // Cierra la proyección con la función munmap
    if(munmap(ubicacion_mapeo, info.st_size)) {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }
}