#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/file.h>

/*
.______   .______        ______   ____    ____  _______   ______   ______  __    ______   .__   __.  _______     _______.
|   _  \  |   _  \      /  __  \  \   \  /   / |   ____| /      | /      ||  |  /  __  \  |  \ |  | |   ____|   /       |
|  |_)  | |  |_)  |    |  |  |  |  \   \/   /  |  |__   |  ,----'|  ,----'|  | |  |  |  | |   \|  | |  |__     |   (----`
|   ___/  |      /     |  |  |  |   \_    _/   |   __|  |  |     |  |     |  | |  |  |  | |  . `  | |   __|     \   \    
|  |      |  |\  \----.|  `--'  |     |  |     |  |____ |  `----.|  `----.|  | |  `--'  | |  |\   | |  |____.----)   |   
| _|      | _| `._____| \______/      |__|     |_______| \______| \______||__|  \______/  |__| \__| |_______|_______/    
                                                                                                                         
Aldán Creo Mariño, SOI 2020/21

Se recomienda empezar ejecutando el segundo proceso (que corresponde a este código).
*/

int main()
{
    int fd;
    struct stat info;
    struct flock fl;

    // Abro fichero.txt para lectura y escritura con la función open
    fd = open("./fichero.txt", O_RDWR);
    if (fd < 0)
    {
        perror("Error en open()");
        exit(EXIT_FAILURE);
    }
    if (fstat(fd, &info))
    { // Usamos fstat para obtener la longitud del fichero
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

    memset(&fl, 0, sizeof(struct flock));
    fl.l_type = F_WRLCK;
    fl.l_start = SEEK_SET;
    if (fcntl(fd, F_SETLK, &fl))
    { // Bloqueamos el fichero. Esto le indicará al otro proceso que debe esperar.
        perror("Error en fcntl()");
        exit(EXIT_FAILURE);
    }

    printf("Escribe un número cualquiera para proceder.\n"); // Esperamos al usuario
    scanf("%*d");

    // "El segundo proceso añade los 3 asteriscos usando memset"
    memset(((char *)ubicacion_mapeo) + 2, '*', sizeof(char) * 3); //Escribo *** en el archivo

    printf("He escrito *** en el archivo.\n");

    memset(&fl, 0, sizeof(struct flock));
    fl.l_type = F_UNLCK;
    fl.l_start = SEEK_SET;
    if (fcntl(fd, F_SETLK, &fl))
    { // Bloqueamos el fichero
        perror("Error en fcntl()");
        exit(EXIT_FAILURE);
    }

    // Cierro la proyección con la función munmap
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

    exit(EXIT_SUCCESS);
}
