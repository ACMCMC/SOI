// Aldán Creo Mariño, SOI 2020/21

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int fichero1, fichero2;
char caracter;

int leer_escribir();

int main(int argc, char *argv[])
{
    pid_t pid = -1;
    int num_proc, i, stat_loc;
    if (argc != 4)
    { // Salimos con error si no se han especificado los nombres de los archivos 1 y 2
        fprintf(stderr, "Deben especificarse los nombres de dos ficheros, para leer y escribir, y del número de procesos a ejecutar en paralelo\n");
        exit(EXIT_FAILURE);
    }
    fichero1 = open(argv[1], 0444); // Abrimos el archivo 1 en modo lectura
    if (fichero1 < 0)
    {
        perror("Error al abrir el fichero 1");
        exit(EXIT_FAILURE);
    }
    fichero2 = open(argv[2], 0666); // Abrimos el archivo 2 en modo escritura
    if (fichero2 < 0)
    {
        exit(EXIT_FAILURE);
        perror("Error al abrir el fichero 2");
    }
    num_proc = atoi(argv[3]); // El número de procesos es el tercer argumento

    for (i = 0; i < num_proc && pid != 0; i++)
    {
        pid = fork();
        if (pid < 0) // Comprobamos que no haya habido errores en el fork
        {
            perror("Error ejecutando fork()");
            close(fichero1);
            close(fichero2);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {                    // Este es el hijo
            leer_escribir(); // Llamamos a la función, tanto en el padre como en el hijo
        }
    }
    if (pid != 0)
    {
        for (i = 0; i < num_proc; i++)
        {
            if ((pid = wait(&stat_loc)) == -1)
            {
                perror("Error esperando por el proceso");
                exit(EXIT_FAILURE);
            }
            else if (!WIFEXITED(stat_loc))
            {
                fprintf(stderr, "Error en el proceso, no ha salido normalmente");
                exit(EXIT_FAILURE);
            }
            else if (WEXITSTATUS(stat_loc) != EXIT_SUCCESS)
            {
                fprintf(stderr, "Error en el proceso, no ha devuelto EXIT_SUCCESS");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (close(fichero1) == EOF)
    {
        fprintf(stderr, "Error cerrando el fichero 1 en el %s: %s\n", (pid == 0) ? "hijo" : "padre", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (close(fichero2) == EOF)
    {
        fprintf(stderr, "Error cerrando el fichero 2 en el %s: %s\n", (pid == 0) ? "hijo" : "padre", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

int leer_escribir()
{
    for (;;)
    {
        if (read((int)fichero1, &caracter, 1) != 1)
        {
            return (EXIT_SUCCESS);
        }
        write((int)fichero2, &caracter, 1);
    }
}
