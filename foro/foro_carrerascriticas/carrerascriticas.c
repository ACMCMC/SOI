#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int fichero1, fichero2;
char caracter;

int leer_escribir();

int main(int argc, char *argv[])
{
    pid_t pid;
    if (argc != 3)
    { // Salimos con error si no se han especificado los nombres de los archivos 1 y 2
        fprintf(stderr, "Deben especificarse los nombres de dos ficheros, para leer y escribir\n");
        exit(EXIT_FAILURE);
    }
    fichero1 = open(argv[1], 0444); // Abrimos el archivo 1 en modo lectura
    if (fichero1 == NULL)
    {
        perror("Error al abrir el fichero 1");
        exit(EXIT_FAILURE);
    }
    fichero2 = open(argv[2], 0666); // Abrimos el archivo 2 en modo escritura
    if (fichero2 == NULL)
    {
        exit(EXIT_FAILURE);
        perror("Error al abrir el fichero 2");
    }
    pid = fork();
    if (pid < 0) // Comprobamos que no haya habido errores en el fork
    {
        perror("Error ejecutando fork()");
        fclose(fichero1);
        fclose(fichero2);
        exit(EXIT_FAILURE);
    }
    leer_escribir(); // Llamamos a la función, tanto en el padre como en el hijo
    if (fclose(fichero1) == EOF)
    {
        fprintf(stderr, "Error cerrando el fichero 1 en el %s: %s\n", (pid == 0) ? "hijo" : "padre", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (fclose(fichero2) == EOF)
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
        if (read(fichero1, &caracter, 1) != 1)
            return (0);
        write(fichero2, &caracter, 1);
    }
}
