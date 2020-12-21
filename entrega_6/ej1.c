#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main() {
    int fd;
    struct stat info;
    long long i, long_archivo;
    char caracter_arch;

    // Abre un archivo para lectura con la función open
    fd = open("./texto.txt", O_RDONLY);
    if (fd < 0) {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }

    // Averigua su longitud con fstat
    if (fstat(fd, &info)) {
        perror("Error en fstat()");
        exit(EXIT_FAILURE);
    }

    // Imprime su contenido por pantalla utilizando un lazo con tantas iteraciones como la longitud del archivo
    long_archivo = (long long) info.st_size;
    printf("CON SYSCALLS:\n");
    for (i = 0; i < long_archivo; i++) {
        read(fd, &caracter_arch, 1);
        printf("%c", caracter_arch);
    }
    printf("\n\n");

    /*
     * Proyecta el archivo en memoria con mmap. Deja que el sistema operativo elija la dirección de memoria. Establece accesos de solo lectura, área de memoria privada y un desplazamiento desde el inicio del archivo (offset) de 0.
     */
    void* ubicacion_mapeo;
    ubicacion_mapeo = mmap(0, long_archivo, PROT_READ, MAP_PRIVATE, fd, 0);
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

    // Imprime el contenido de la proyección en memoria carácter a carácter
    printf("MAPEADO:\n");
    for (i = 0; i < long_archivo; i++) {
        printf("%c", *(((char*) ubicacion_mapeo)+i));
    }
    printf("\n\n");
    
    // Cierra la proyección con la función munmap
    if(munmap(ubicacion_mapeo, long_archivo)) {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }
}