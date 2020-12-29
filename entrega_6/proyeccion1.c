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

Se recomienda empezar ejecutando el segundo proceso.
*/

int main()
{
    int fd;
    struct stat info;
    char *cadena_texto;         // Aquí guardaré la cadena de texto con la que voy a trabajar
    struct flock res_fcntl;     // Lo usaré para controlar la sincronización
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

    // La estrategia que voy a seguir es: bloquear el fichero en el proceso 2, cosa que detectaré desde este proceso; desbloquearlo (lo detecto también), y entonces bloquearlo desde este y realizar las operaciones necesarias.

    do
    { // Espero a que el proceso 2 bloquee el fichero
        memset(&res_fcntl, 0, sizeof(struct flock));
        res_fcntl.l_type = F_WRLCK;
        res_fcntl.l_start = SEEK_SET;
        if (fcntl(fd, F_GETLK, &res_fcntl))
        {
            perror("Error en fcntl()");
        }
        esperando_por_proceso2 = res_fcntl.l_type != F_WRLCK;
        if (esperando_por_proceso2 && prompt_al_usuario_no_impreso)
        {
            printf("Inicia el proceso 2 para continuar...\n");
            prompt_al_usuario_no_impreso = 0;
        }
    } while (esperando_por_proceso2);

    prompt_al_usuario_no_impreso = 1;
    do
    { // Espero a que el proceso 2 lo desbloquee
        memset(&res_fcntl, 0, sizeof(struct flock));
        res_fcntl.l_type = F_WRLCK;
        res_fcntl.l_start = SEEK_SET;
        if (fcntl(fd, F_GETLK, &res_fcntl))
        {
            perror("Error en fcntl()");
        }
        esperando_por_proceso2 = res_fcntl.l_type != F_UNLCK;
        if (esperando_por_proceso2 && prompt_al_usuario_no_impreso)
        {
            printf("Introduce cualquier número en el proceso 2 para continuar...\n");
            prompt_al_usuario_no_impreso = 0;
        }
    } while (esperando_por_proceso2);

    memset(&res_fcntl, 0, sizeof(struct flock));
    res_fcntl.l_type = F_WRLCK;
    res_fcntl.l_start = SEEK_SET;
    if (fcntl(fd, F_SETLK, &res_fcntl))
    { // Bloqueamos el fichero
        perror("Error en fcntl()");
        exit(EXIT_FAILURE);
    }

    cadena_texto = (char *)malloc(sizeof(char) * info.st_size); // Reservamos memoria para las modificaciones. El tamaño de un char siempre es 1 byte.
    memcpy((void *)cadena_texto, (void *)ubicacion_mapeo, sizeof(char) * 5);
    memcpy((void *)(cadena_texto + 5), "", sizeof(char)); // Añado un '\0' a la cadena
    strncat(cadena_texto, "2345", sizeof(char) * 5);      // Añado 2345, y se va a copiar también un '\0'
    cadena_texto = (char *)realloc((void *)cadena_texto, sizeof(char) * (info.st_size+2));
    strncat(cadena_texto, (char *)ubicacion_mapeo + 5, 4); // Copio los siguientes 4 caracteres desde la proyección
    cadena_texto[info.st_size+2-1] = ((char*)ubicacion_mapeo)[info.st_size-1]; // Si al final del archivo tenemos un '\n', lo copiamos. Si no lo tenemos, se copiarán datos basura, pero no importa, porque con ftruncate cuando copiemos la memoria de vuelta, estos datos basura no entrarán en el archivo.

    printf("Tras el proceso 2, he realizado el resto de operaciones sobre el archivo.\n");

    // Cierra la proyección con la función munmap
    if (munmap(ubicacion_mapeo, info.st_size))
    {
        perror("Error en munmap()");
        exit(EXIT_FAILURE);
    }

    ftruncate(fd, (info.st_size + 2)*sizeof(char)); // Expandimos en 2 chars (2 bytes) el tamaño del fichero

    ubicacion_mapeo = mmap(0, (info.st_size + 2)*sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ubicacion_mapeo == MAP_FAILED)
    {
        perror("Error en mmap()");
        exit(EXIT_FAILURE);
    }

    memcpy(ubicacion_mapeo, (void *)cadena_texto, sizeof(char) * (info.st_size+2)); // Copio la cadena de vuelta, sin el '\0'

    memset(&res_fcntl, 0, sizeof(struct flock));
    res_fcntl.l_type = F_UNLCK;
    res_fcntl.l_start = SEEK_SET;
    if (fcntl(fd, F_SETLK, &res_fcntl))
    { // Desbloqueamos el fichero
        perror("Error en fcntl()");
        exit(EXIT_FAILURE);
    }

    // Cierra la proyección con la función munmap
    if (munmap(ubicacion_mapeo, info.st_size+2))
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

    free(cadena_texto);
    exit(EXIT_SUCCESS);
}
