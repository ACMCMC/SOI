#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main()
{
    int i;
    double res = 0;
    pid_t pid1, pid2, pid3;
    FILE *out;

    out = fopen("salida_calculo", "w");
    if (out == NULL)
    {
        perror("Error abriendo el archivo de salida");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();

    if (pid1 < 0)
    {
        perror("Error ejecutando el fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    { // Este es el hijo
        for (i = 0; i < 10000000; i += 2)
        {
            res += cos(sqrt(i));
        }
        res = res / (10000000.0 / 2.0);
        printf("Resultado pares: %f\n", res);

        out = fopen("salida_hijo1", "w");

        fwrite(&res, sizeof(double), 1, out);

        fclose(out);

        exit(EXIT_SUCCESS);
    }
    else
    { // Este es el padre
        pid2 = fork();
        if (pid2 < 0)
        {
            perror("Error ejecutando el fork()");
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0)
        { // Este es el hijo
            for (i = 1; i < 10000000; i += 2)
            {
                res += cos(sqrt(i));
            }
            res = res / (10000000.0 / 2.0);
            printf("Resultado impares: %f\n", res);

            fwrite(&res, sizeof(double), 1, out);
        }
        else
        { // Este es el padre
            pid3 = fork();
            if (pid3 < 0)
            {
                perror("Error ejecutando el fork()");
                exit(EXIT_FAILURE);
            }
            else if (pid3 == 0)
            { // Este es el hijo
                for (i = 0; i < 10000000; i++)
                {
                    res += cos(sqrt(i));
                }
                res = res / (10000000.0);
                printf("Resultado completo: %f\n", res);

                fwrite(&res, sizeof(double), 1, out);
            }
            else
            { // Este es el padre
            }
        }
    }
    fclose(out);

    exit(EXIT_SUCCESS);
}
