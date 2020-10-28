#include <printf.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define FILE_OUT "salida_calculo.bin"

int main()
{
    int i; // Variable auxiliar
    double res = 0, res_hijo1, res_hijo2; // Donde guardaremos los resulatados de los procesos hijo 1, 2; y 3 y 4
    pid_t pid1, pid2, pid3, pid4, pid5, pid; // Para guardar los PID de los procesos
    int stat_loc1, stat_loc2, stat_loc5, stat_loc; // Códigos de retorno de los procesos cuando hagamos el wait()
    FILE *out; // Puntero a archivo donde guardaremos resultados

    if (printf("PID DEL PROCESO PADRE: %d\n", getpid()) < 0)
    {
        perror("Error ejecutando printf()");
        exit(EXIT_FAILURE);
    }

    out = fopen(FILE_OUT, "wb+"); // Abrimos el archivo en modo lectura y escritura
    if (out == NULL)
    {
        perror("Error abriendo el archivo de salida");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();

    if (pid1 < 0)
    {
        perror("Error ejecutando el fork()");
        fclose(out);
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    { // Este es el hijo
        for (i = 0; i < 10000000; i += 2)
        {
            res += cos(sqrt(i));
        }
        res = res / (10000000.0 / 2.0);

        if (printf("Resultado pares: %lf\n", res) < 0)
        {
            perror("Error ejecutando printf()");
            fclose(out);
            exit(EXIT_FAILURE);
        }

        fwrite(&res, sizeof(double), 1, out);
        if (ferror(out))
        { // Comprobamos si se ha producido algún error en el archivo
            perror("Error. Abortando el hijo 1.\n");
            fclose(out);
            exit(EXIT_FAILURE);
        }
    }
    else
    { // Este es el padre
        pid2 = fork();
        if (pid2 < 0)
        {
            perror("Error ejecutando el fork()");
            fclose(out);
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0)
        { // Este es el hijo
            for (i = 1; i < 10000000; i += 2)
            {
                res += cos(sqrt(i));
            }
            res = res / (10000000.0 / 2.0);
            if (printf("Resultado impares: %lf\n", res) < 0)
            {
                perror("Error ejecutando printf()");
                fclose(out);
                exit(EXIT_FAILURE);
            }

            fwrite(&res, sizeof(double), 1, out);
            if (ferror(out))
            { // Comprobamos si se ha producido algún error en el archivo
                perror("Error. Abortando el hijo 2.\n");
                fclose(out);
                exit(EXIT_FAILURE);
            }
        }
        else
        { // Este es el padre
            pid3 = fork();
            if (pid3 < 0)
            {
                perror("Error ejecutando el fork()");
                fclose(out);
                exit(EXIT_FAILURE);
            }
            else if (pid3 == 0)
            { // Este es el hijo
                for (i = 0; i < 10000000; i++)
                {
                    res += cos(sqrt(i));
                }
                res = res / (10000000.0);

                if (printf("Resultado completo: %lf\n", res) < 0)
                {
                    perror("Error ejecutando printf()");
                    fclose(out);
                    exit(EXIT_FAILURE);
                }

                fseek(out, 0, SEEK_END); // Puede que el hijo 4 acabe antes que el 3, así que para eso nos movemos al final del archivo antes de escribir. Esta es una situación de carrera crítica.
                fwrite(&res, sizeof(double), 1, out);
                if (ferror(out))
                { // Comprobamos si se ha producido algún error en el archivo
                    perror("Error. Abortando el hijo 3.\n");
                    fclose(out);
                    exit(EXIT_FAILURE);
                }
            }
            else
            { // Este es el padre
                // Esperamos por los hijos 1 y 2
                pid = waitpid(pid1, &stat_loc1, 0);
                if (pid == -1)
                {
                    perror("Error al ejecutar el wait. Abortando.");
                    fclose(out);
                    exit(EXIT_FAILURE);
                }
                if (!WIFEXITED(stat_loc1) || WEXITSTATUS(stat_loc1) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "Algo ha ido mal en el proceso %d. Abortando. Código de retorno: %d\n", pid1, WEXITSTATUS(stat_loc1));
                    fclose(out);
                    exit(EXIT_FAILURE);
                }

                pid = waitpid(pid2, &stat_loc2, 0);
                if (pid == -1)
                {
                    perror("Error al ejecutar el wait. Abortando.");
                    fclose(out);
                    exit(EXIT_FAILURE);
                }
                if (!WIFEXITED(stat_loc2) || WEXITSTATUS(stat_loc2) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "Algo ha ido mal en el proceso %d. Abortando. Código de retorno: %d\n", pid2, WEXITSTATUS(stat_loc2));
                    fclose(out);
                    exit(EXIT_FAILURE);
                }

                // Ahora que los hijos 1 y 2 han terminado, calculamos el resultado en base a las contribuciones de los dos

                pid4 = fork();

                if (pid4 < 0)
                {
                    perror("Error ejecutando el fork()");
                    fclose(out);
                    exit(EXIT_FAILURE);
                }
                else if (pid4 == 0)
                {                // Este es el hijo 4
                    fclose(out); // Cerramos el archivo que había abierto el padre y lo abrimos de nuevo para obtener un nuevo descriptor de archivo independiente, así si nos movemos por él no moveremos al resto de procesos
                    out = fopen(FILE_OUT, "ab+");
                    if (out == NULL)
                    {
                        perror("Error. Abortando el hijo 4.\n");
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }
                    fseek(out, 0, SEEK_SET); // Nos situamos al principio del archivo
                    fread(&res_hijo1, sizeof(double), 1, out); // Leemos los resultados de los dos primeros hijos, que son los dos primeros valores del archivo
                    fread(&res_hijo2, sizeof(double), 1, out);
                    res = (res_hijo1 + res_hijo2) / 2.0;  // Calculamos su media
                    fseek(out, 0, SEEK_END);              // Nos situamos al final del archivo, ya que podría haber escrito el tercer hijo su resultado
                    fwrite(&res, sizeof(double), 1, out); // Escribimos el resultado de la media de los dos valores en el archivo
                    if (ferror(out))
                    { // Comprobamos si alguna de estas 3 operaciones ha producido algún error en el archivo
                        perror("Error. Abortando el hijo 4.\n");
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }
                }
                else
                { // Este es el padre

                    // Antes de ejecutar el quinto proceso, tenemos que esperar por el 3 y el 4. Ejecutamos dos wait, porque sabemos que tenemos que esperar a dos hijos, pero no nos importa el orden.

                    pid = wait(&stat_loc);
                    if (pid == -1)
                    {
                        perror("Error al ejecutar el wait. Abortando.");
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }
                    if (!WIFEXITED(stat_loc) || WEXITSTATUS(stat_loc) != EXIT_SUCCESS)
                    {
                        fprintf(stderr, "Algo ha ido mal en el proceso %d. Abortando. Código de retorno: %d\n", pid, WEXITSTATUS(stat_loc));
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }

                    pid = wait(&stat_loc);
                    if (pid == -1)
                    {
                        perror("Error al ejecutar el wait. Abortando.");
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }
                    if (!WIFEXITED(stat_loc) || WEXITSTATUS(stat_loc) != EXIT_SUCCESS)
                    {
                        fprintf(stderr, "Algo ha ido mal en el proceso %d. Abortando. Código de retorno: %d\n", pid, WEXITSTATUS(stat_loc));
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }

                    // Antes de entrar al quinto proceso, leemos el archivo de resultados para saber lo que devolvieron los procesos 3 y 4

                    fseek(out, -2 * sizeof(double), SEEK_END); // Vamos a leer los dos últimos números que hemos escrito (resultados del proceso 3 y 4, indistintamente)
                    fread(&res_hijo1, sizeof(double), 1, out);
                    fread(&res_hijo2, sizeof(double), 1, out);
                    if (ferror(out))
                    { // Comprobamos si alguna de estas 3 operaciones ha producido algún error en el archivo
                        perror("Error. Abortando.\n");
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }

                    // Ahora ejecutamos el quinto proceso

                    pid5 = fork();
                    if (pid5 < 0)
                    {
                        perror("Error ejecutando el fork()");
                        fclose(out);
                        exit(EXIT_FAILURE);
                    }
                    else if (pid5 == 0)
                    { // Este es el quinto hijo
                        if (printf("Diferencia de los resultados: %lf\n", fabs(res_hijo1 - res_hijo2)) < 0)
                        {
                            perror("Error ejecutando printf()");
                            fclose(out);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    { // Este es el padre

                        // Esperamos a que acabe el quinto y último proceso desde el padre

                        pid = waitpid(pid5, &stat_loc5, 0);
                        if (pid == -1)
                        {
                            perror("Error al ejecutar el wait. Abortando.");
                            fclose(out);
                            exit(EXIT_FAILURE);
                        }
                        if (!WIFEXITED(stat_loc2) || WEXITSTATUS(stat_loc2) != EXIT_SUCCESS)
                        {
                            fprintf(stderr, "Algo ha ido mal en el proceso %d. Abortando. Código de retorno: %d\n", pid2, WEXITSTATUS(stat_loc2));
                            fclose(out);
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }
    }

    //printf("\tSale el proceso con PID %d\n", getpid());

    fclose(out); // Esto deben ejecutarlo todos los procesos

    exit(EXIT_SUCCESS);
}
